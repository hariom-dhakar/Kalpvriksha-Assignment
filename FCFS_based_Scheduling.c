
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PID 20000
#define MAX_NAME 50

typedef enum { READY, RUNNING, WAITING, TERMINATED, KILLED } State;

typedef struct PCB {
    char name[MAX_NAME];
    int pid;
    int total_cpu;
    int remaining_cpu;
    int io_start;
    int io_duration;
    int io_remaining;
    int executed_cpu;
    int arrival_time;
    int completion_time;
    int wait_enter_time;
    int io_ready_time;
    State state;
    struct PCB *next;
} PCB;

typedef struct Queue {
    PCB *head, *tail;
} Queue;

typedef struct KillEvent {
    int pid;
    int time;
    struct KillEvent *next;
} KillEvent;

void initQueue(Queue *q);
void enqueue(Queue *q, PCB *p);
PCB* dequeue(Queue *q);
PCB* removeFromQueue(Queue *q, int pid);
PCB* createPCB(char name[], int pid, int cpu, int io_s, int io_d);
void addKillEvent(int pid, int time);
void applyKillEvents(int current_time);
void updateIO(int current_time);
void moveIOCompleted(int current_time);
void scheduleCPU();
void executeOneTick(int current_time);
int allDone();
void printFinalTable();

PCB* pid_map[MAX_PID];
Queue readyQ, waitQ, termQ;
KillEvent *killList = NULL;
PCB *running = NULL;

int main() {
    initQueue(&readyQ);
    initQueue(&waitQ);
    initQueue(&termQ);

    char word[32], name[MAX_NAME];
    int pid, cpu, ios, iod, time;

    while (scanf("%s", word) != EOF) {
        if (strcmp(word, "KILL") == 0) {
            if (scanf("%d %d", &pid, &time) == 2) addKillEvent(pid, time);
        } else {
            strcpy(name, word);
            if (scanf("%d %d %d %d", &pid, &cpu, &ios, &iod) == 4) {
                PCB *p = createPCB(name, pid, cpu, ios, iod);
                enqueue(&readyQ, p);
            }
        }
    }

    int current_time = 0;

    while (!allDone()) {
        applyKillEvents(current_time);
        updateIO(current_time);
        moveIOCompleted(current_time);
        scheduleCPU();
        executeOneTick(current_time);
        current_time++;
    }

    printFinalTable();
    return 0;
}

void initQueue(Queue *q) {
    q->head = NULL;
    q->tail = NULL;
}

void enqueue(Queue *q, PCB *p) {
    p->next = NULL;
    if (!q->tail) q->head = q->tail = p;
    else { q->tail->next = p; q->tail = p; }
}

PCB* dequeue(Queue *q) {
    if (!q->head) return NULL;
    PCB *p = q->head;
    q->head = p->next;
    if (!q->head) q->tail = NULL;
    return p;
}

PCB* removeFromQueue(Queue *q, int pid) {
    PCB *cur = q->head, *prev = NULL;
    while (cur) {
        if (cur->pid == pid) {
            if (prev) prev->next = cur->next;
            else q->head = cur->next;
            if (q->tail == cur) q->tail = prev;
            cur->next = NULL;
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

PCB* createPCB(char name[], int pid, int cpu, int io_s, int io_d) {
    PCB *p = malloc(sizeof(PCB));
    strcpy(p->name, name);
    p->pid = pid;
    p->total_cpu = cpu;
    p->remaining_cpu = cpu;
    p->io_start = io_s;
    p->io_duration = io_d;
    p->io_remaining = 0;
    p->executed_cpu = 0;
    p->arrival_time = 0;
    p->completion_time = -1;
    p->wait_enter_time = -1;
    p->io_ready_time = -1;
    p->state = READY;
    p->next = NULL;
    if (pid >= 0 && pid < MAX_PID) pid_map[pid] = p;
    return p;
}

void addKillEvent(int pid, int time) {
    KillEvent *k = malloc(sizeof(KillEvent));
    k->pid = pid;
    k->time = time;
    k->next = killList;
    killList = k;
}

void applyKillEvents(int current_time) {
    KillEvent *k = killList, *prev = NULL;
    while (k) {
        if (k->time == current_time) {
            PCB *target = NULL;
            if (k->pid >= 0 && k->pid < MAX_PID) target = pid_map[k->pid];
            if (target && target->state != TERMINATED && target->state != KILLED) {
                if (running && running->pid == target->pid) {
                    running->state = KILLED;
                    running->completion_time = current_time;
                    enqueue(&termQ, running);
                    running = NULL;
                } else {
                    PCB *x = removeFromQueue(&readyQ, target->pid);
                    if (!x) x = removeFromQueue(&waitQ, target->pid);
                    if (x) {
                        x->state = KILLED;
                        x->completion_time = current_time;
                        enqueue(&termQ, x);
                    }
                }
            }
            KillEvent *old = k;
            if (!prev) killList = k->next;
            else prev->next = k->next;
            k = (prev ? prev->next : killList);
            free(old);
        } else {
            prev = k;
            k = k->next;
        }
    }
}

void updateIO(int current_time) {
    PCB *p = waitQ.head;
    while (p) {
        if (p->state == WAITING && current_time > p->wait_enter_time && p->io_remaining > 0) {
            p->io_remaining--;
            if (p->io_remaining == 0) p->io_ready_time = current_time + 1;
        }
        p = p->next;
    }
}

void moveIOCompleted(int current_time) {
    PCB *p = waitQ.head, *next;
    while (p) {
        next = p->next;
        if (p->state == WAITING && p->io_remaining == 0 && p->io_ready_time != -1 && current_time >= p->io_ready_time) {
            removeFromQueue(&waitQ, p->pid);
            p->state = READY;
            p->io_ready_time = -1;
            enqueue(&readyQ, p);
        }
        p = next;
    }
}

void scheduleCPU() {
    if (!running)
        running = dequeue(&readyQ);
    if (running)
        running->state = RUNNING;
}

void executeOneTick(int current_time) {
    if (!running) return;
    running->executed_cpu++;
    running->remaining_cpu--;
    int finished = (running->remaining_cpu == 0);
    int needIO = (
        running->io_duration > 0 &&
        running->executed_cpu == running->io_start &&
        running->remaining_cpu > 0
    );
    if (finished) {
        running->state = TERMINATED;
        running->completion_time = current_time + 1;
        enqueue(&termQ, running);
        running = NULL;
    }
    else if (needIO) {
        running->state = WAITING;
        running->io_remaining = running->io_duration;
        running->wait_enter_time = current_time;
        running->io_ready_time = -1;
        enqueue(&waitQ, running);
        running = NULL;
    }
}

int allDone() {
    return (!running && !readyQ.head && !waitQ.head && !killList);
}

int cmp_pid(const void *a, const void *b) {
    PCB *pa = *(PCB**)a;
    PCB *pb = *(PCB**)b;
    return pa->pid - pb->pid;
}

void printFinalTable() {
    PCB **list = malloc(sizeof(PCB*) * MAX_PID);
    int count = 0;
    for (int i = 0; i < MAX_PID; ++i) {
        if (pid_map[i]) {
            PCB *p = pid_map[i];
            if (p->completion_time != -1) {
                list[count++] = p;
            }
        }
    }
    qsort(list, count, sizeof(PCB*), cmp_pid);
    printf("%-5s %-10s %-5s %-5s %-15s %-11s %-7s\n", "PID", "Name", "CPU", "IO", "Status", "Turnaround", "Waiting");
    for (int i = 0; i < count; ++i) {
        PCB *p = list[i];
        if (p->state == KILLED) {
            char status_buf[32];
            snprintf(status_buf, sizeof(status_buf), "KILLED at %d", p->completion_time);
            printf("%-5d %-10s %-5d %-5d %-15s %-11s %-7s\n",
                   p->pid, p->name, p->total_cpu, p->io_duration, 
                   status_buf, "-", "-");
        } else {
            int turn = p->completion_time - p->arrival_time;
            int wait = turn - p->total_cpu; 
            if (wait < 0) wait = 0;
            printf("%-5d %-10s %-5d %-5d %-15s %-11d %-7d\n",
                   p->pid, p->name, p->total_cpu, p->io_duration, "OK", turn, wait);
        }
    }
    free(list);
}
