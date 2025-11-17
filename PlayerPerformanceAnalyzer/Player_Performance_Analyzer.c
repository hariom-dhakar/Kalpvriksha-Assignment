#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "Players_data.h"

#define TOTAL_TEAMS 10
#define PLAYER_ID_MIN 1
#define PLAYER_ID_MAX 1000
#define TEAM_MIN_PLAYERS 11
#define TEAM_MAX_PLAYERS 50

typedef struct PlayerNode
{
    int id;
    char name[51];
    char team[51];
    char role[25];
    int runs;
    float avg;
    float sr;
    int wkts;
    float eco;
    float perfScore;
    struct PlayerNode *next;
} PlayerNode;

typedef struct Team
{
    int id;
    char name[51];
    int countPlayers;
    float battingSRsum;
    int battingSRcount;
    float avgBattingSR;
    PlayerNode *batsmen;
    PlayerNode *bowlers;
    PlayerNode *allRounders;
} Team;

typedef struct
{
    PlayerNode *ref;
} HeapNode;

Team teamList[TOTAL_TEAMS];

void clearBuffer();
int getTeamIndex(const char *);
float computeScore(PlayerNode *);
void sortedInsert(PlayerNode **, PlayerNode *);
bool isUniqueID(int);
void initTeams();
int chooseRole();
void addPlayer();
void displayTeam();
int cmpTeam(const void *, const void *);
void sortTeams();
void topKPlayers();
void heapSwap(HeapNode *, HeapNode *);
void siftDown(HeapNode[], int, int);
void pushHeap(HeapNode[], int *, PlayerNode *);
PlayerNode *popMax(HeapNode[], int *);
void showAllRole();
void freeAll();

int main()
{
    initTeams();

    int c;
    do
    {
        printf("1.Add Player\n2.Show Team\n3.Sort Teams\n4.Top K\n5.Show Role All Teams\n6.Exit\nChoice: ");
        if (scanf("%d", &c) != 1)
        {
            clearBuffer();
            continue;
        }
        clearBuffer();

        if (c == 1)
        {
            addPlayer();
        }
        else if (c == 2)
        {
            displayTeam();
        }
        else if (c == 3)
        {
            sortTeams();
        }
        else if (c == 4)
        {
            topKPlayers();
        }
        else if (c == 5)
        {
            showAllRole();
        }

    } while (c != 6);

    freeAll();
    return 0;
}

int getTeamIndex(const char *teamName)
{
    for (int i = 0; i < teamCount; i++)
    {
        if (strcmp(teamName, teams[i]) == 0)
        {
            return i + 1;
        }
    }
    return -1;
}

float computeScore(PlayerNode *p)
{
    if (strcmp(p->role, "Batsman") == 0)
    {
        return (p->avg * p->sr) / 100.0f;
    }
    if (strcmp(p->role, "Bowler") == 0)
    {
        return (p->wkts * 2.0f) + (100.0f - p->eco);
    }
    if (strcmp(p->role, "All-rounder") == 0)
    {
        return ((p->avg * p->sr) / 100.0f) + (p->wkts * 2.0f);
    }
    return 0;
}

void sortedInsert(PlayerNode **head, PlayerNode *p)
{
    if (*head == NULL || p->perfScore > (*head)->perfScore)
    {
        p->next = *head;
        *head = p;
        return;
    }
    PlayerNode *cur = *head;
    while (cur->next && cur->next->perfScore >= p->perfScore)
    {
        cur = cur->next;
    }
    p->next = cur->next;
    cur->next = p;
}

bool isUniqueID(int pid)
{
    for (int t = 0; t < TOTAL_TEAMS; t++)
    {
        PlayerNode *ptr = teamList[t].batsmen;
        while (ptr)
        {
            if (ptr->id == pid)
            {
                return false;
            }
            ptr = ptr->next;
        }
        ptr = teamList[t].bowlers;
        while (ptr)
        {
            if (ptr->id == pid)
            {
                return false;
            }
            ptr = ptr->next;
        }
        ptr = teamList[t].allRounders;
        while (ptr)
        {
            if (ptr->id == pid)
            {
                return false;
            }
            ptr = ptr->next;
        }
    }
    return true;
}

void initTeams()
{
    for (int i = 0; i < TOTAL_TEAMS; i++)
    {
        teamList[i].id = i + 1;
        strcpy(teamList[i].name, teams[i]);
        teamList[i].countPlayers = 0;
        teamList[i].battingSRsum = 0;
        teamList[i].battingSRcount = 0;
        teamList[i].batsmen = NULL;
        teamList[i].bowlers = NULL;
        teamList[i].allRounders = NULL;
    }

    for (int i = 0; i < playerCount; i++)
    {
        const Player *src = &players[i];
        int tid = getTeamIndex(src->team);
        int idx = tid - 1;

        PlayerNode *p = malloc(sizeof(PlayerNode));
        p->id = src->id;
        strcpy(p->name, src->name);
        strcpy(p->team, src->team);
        strcpy(p->role, src->role);
        p->runs = src->totalRuns;
        p->avg = src->battingAverage;
        p->sr = src->strikeRate;
        p->wkts = src->wickets;
        p->eco = src->economyRate;
        p->next = NULL;
        p->perfScore = computeScore(p);

        Team *T = &teamList[idx];
        T->countPlayers++;

        if (strcmp(p->role, "Batsman") == 0)
        {
            sortedInsert(&T->batsmen, p);
            T->battingSRsum += p->sr;
            T->battingSRcount++;
        }
        else if (strcmp(p->role, "Bowler") == 0)
        {
            sortedInsert(&T->bowlers, p);
        }
        else
        {
            sortedInsert(&T->allRounders, p);
            T->battingSRsum += p->sr;
            T->battingSRcount++;
        }
    }
}

int chooseRole()
{
    int r = 0;
    while (r < 1 || r > 3)
    {
        printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): ");
        if (scanf("%d", &r) != 1)
        {
            clearBuffer();
            continue;
        }
        clearBuffer();
    }
    return r;
}

void clearBuffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void addPlayer()
{
    int tid;
    while (1)
    {
        printf("Team ID (1-10): ");
        if (scanf("%d", &tid) == 1 && tid >= 1 && tid <= TOTAL_TEAMS)
        {
            clearBuffer();
            break;
        }
        clearBuffer();
    }

    Team *T = &teamList[tid - 1];
    if (T->countPlayers >= TEAM_MAX_PLAYERS)
    {
        return;
    }

    PlayerNode *p = malloc(sizeof(PlayerNode));
    strcpy(p->team, T->name);
    p->next = NULL;

    while (1)
    {
        printf("Player ID (1–1000): ");
        if (scanf("%d", &p->id) == 1 &&
            p->id >= PLAYER_ID_MIN && p->id <= PLAYER_ID_MAX &&
            isUniqueID(p->id))
        {
            clearBuffer();
            break;
        }
        clearBuffer();
    }

    printf("Player Name: ");
    fgets(p->name, sizeof(p->name), stdin);
    p->name[strcspn(p->name, "\n")] = '\0';

    int r = chooseRole();
    printf("Runs: ");
    scanf("%d", &p->runs);
    clearBuffer();
    printf("Batting Avg: ");
    scanf("%f", &p->avg);
    clearBuffer();
    printf("Strike Rate: ");
    scanf("%f", &p->sr);
    clearBuffer();
    printf("Wickets: ");
    scanf("%d", &p->wkts);
    clearBuffer();
    printf("Economy Rate: ");
    scanf("%f", &p->eco);
    clearBuffer();

    if (r == 1)
    {
        strcpy(p->role, "Batsman");
    }
    else if (r == 2)
    {
        strcpy(p->role, "Bowler");
    }
    else
    {
        strcpy(p->role, "All-rounder");
    }
    p->perfScore = computeScore(p);
    T->countPlayers++;

    if (r == 1)
    {
        sortedInsert(&T->batsmen, p);
        T->battingSRsum += p->sr;
        T->battingSRcount++;
    }
    else if (r == 2)
    {
        sortedInsert(&T->bowlers, p);
    }
    else
    {
        sortedInsert(&T->allRounders, p);
        T->battingSRsum += p->sr;
        T->battingSRcount++;
    }
}

void displayTeam()
{
    int tid;
    printf("Team ID (1-10): ");
    if (scanf("%d", &tid) != 1 || tid < 1 || tid > TOTAL_TEAMS)
    {
        clearBuffer();
        return;
    }
    clearBuffer();

    Team *T = &teamList[tid - 1];

    printf("ID   | Name                 | Role        | Runs  | Avg   | SR    | Wkts | ER    | Perf\n");

    PlayerNode *ptr = T->batsmen;
    while (ptr)
    {
        printf("%-4d | %-20s | %-11s | %-5d | %-5.1f | %-5.1f | %-4d | %-5.1f | %.2f\n", ptr->id, ptr->name, ptr->role, ptr->runs, ptr->avg, ptr->sr, ptr->wkts, ptr->eco, ptr->perfScore);
        ptr = ptr->next;
    }

    ptr = T->bowlers;
    while (ptr)
    {
        printf("%-4d | %-20s | %-11s | %-5d | %-5.1f | %-5.1f | %-4d | %-5.1f | %.2f\n", ptr->id, ptr->name, ptr->role, ptr->runs, ptr->avg, ptr->sr, ptr->wkts, ptr->eco, ptr->perfScore);
        ptr = ptr->next;
    }

    ptr = T->allRounders;
    while (ptr)
    {
        printf("%-4d | %-20s | %-11s | %-5d | %-5.1f | %-5.1f | %-4d | %-5.1f | %.2f\n", ptr->id, ptr->name, ptr->role, ptr->runs, ptr->avg, ptr->sr, ptr->wkts, ptr->eco, ptr->perfScore);
        ptr = ptr->next;
    }

    if (T->battingSRcount > 0)
    {
        T->avgBattingSR = T->battingSRsum / T->battingSRcount;
    }

    printf("Total Players: %d\n", T->countPlayers);
    printf("Avg Batting Strike Rate: %.2f\n", T->avgBattingSR);
}

int cmpTeam(const void *a, const void *b)
{
    const Team *A = a;
    const Team *B = b;
    if (A->avgBattingSR < B->avgBattingSR)
    {
        return 1;
    }
    if (A->avgBattingSR > B->avgBattingSR)
    {
        return -1;
    }
    return 0;
}

void sortTeams()
{
    Team temp[TOTAL_TEAMS];
    for (int i = 0; i < TOTAL_TEAMS; i++)
    {
        if (teamList[i].battingSRcount > 0)
        {
            teamList[i].avgBattingSR = teamList[i].battingSRsum / teamList[i].battingSRcount;
        }
        temp[i] = teamList[i];
    }

    qsort(temp, TOTAL_TEAMS, sizeof(Team), cmpTeam);

    for (int i = 0; i < TOTAL_TEAMS; i++)
    {
        printf("%2d | %-15s | %-9.2f | %d\n", temp[i].id, temp[i].name, temp[i].avgBattingSR, temp[i].countPlayers);
    }
}

void topKPlayers()
{
    int tid, k;
    printf("Team ID (1-10): ");
    scanf("%d", &tid);
    clearBuffer();
    if (tid < 1 || tid > TOTAL_TEAMS)
    {
        return;
    }

    int role = chooseRole();
    printf("K value: ");
    scanf("%d", &k);
    clearBuffer();

    Team *T = &teamList[tid - 1];
    PlayerNode *list = NULL;

    if (role == 1)
    {
        list = T->batsmen;
    }
    else if (role == 2)
    {
        list = T->bowlers;
    }
    else
    {
        list = T->allRounders;
    }

    PlayerNode *ptr = list;
    for (int i = 0; i < k && ptr; i++)
    {
        printf("%-4d | %-20s | %-11s | %-5d | %-5.1f | %-5.1f | %-4d | %-5.1f | %.2f\n",
               ptr->id, ptr->name, ptr->role, ptr->runs, ptr->avg, ptr->sr, ptr->wkts, ptr->eco, ptr->perfScore);
        ptr = ptr->next;
    }
}

void heapSwap(HeapNode *a, HeapNode *b)
{
    HeapNode t = *a;
    *a = *b;
    *b = t;
}

void siftDown(HeapNode arr[], int size, int index)
{
    int largest = index;
    int left = index * 2 + 1;
    int right = index * 2 + 2;

    if (left < size && arr[left].ref->perfScore > arr[largest].ref->perfScore)
    {
        largest = left;
    }
    if (right < size && arr[right].ref->perfScore > arr[largest].ref->perfScore)
    {
        largest = right;
    }

    if (largest != index)
    {
        heapSwap(&arr[index], &arr[largest]);
        siftDown(arr, size, largest);
    }
}

void pushHeap(HeapNode arr[], int *size, PlayerNode *p)
{
    arr[*size].ref = p;
    int i = *size;
    (*size)++;

    while (i != 0 && arr[(i - 1) / 2].ref->perfScore < arr[i].ref->perfScore)
    {
        heapSwap(&arr[i], &arr[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

PlayerNode *popMax(HeapNode arr[], int *size)
{
    if (*size <= 0)
    {
        return NULL;
    }
    PlayerNode *ret = arr[0].ref;
    arr[0] = arr[*size - 1];
    (*size)--;
    siftDown(arr, *size, 0);
    return ret;
}

void showAllRole()
{
    int r = chooseRole();
    HeapNode heap[TOTAL_TEAMS * 60];
    int hsize = 0;

    for (int i = 0; i < TOTAL_TEAMS; i++)
    {
        PlayerNode *list = NULL;
        if (r == 1)
        {
            list = teamList[i].batsmen;
        }
        else if (r == 2)
        {
            list = teamList[i].bowlers;
        }
        else
        {
            list = teamList[i].allRounders;
        }
        if (list)
        {
            pushHeap(heap, &hsize, list);
        }
    }

    while (hsize > 0)
    {
        PlayerNode *best = popMax(heap, &hsize);
        printf("%-4d | %-20s | %-11s | %-5d | %-5.1f | %-5.1f | %-4d | %-5.1f | %.2f\n",
               best->id, best->name, best->role, best->runs, best->avg, best->sr, best->wkts, best->eco, best->perfScore);

        if (best->next)
        {
            pushHeap(heap, &hsize, best->next);
        }
    }
}

void freeAll()
{
    for (int i = 0; i < TOTAL_TEAMS; i++)
    {
        PlayerNode *ptr;

        ptr = teamList[i].batsmen;
        while (ptr)
        {
            PlayerNode *tmp = ptr;
            ptr = ptr->next;
            free(tmp);
        }

        ptr = teamList[i].bowlers;
        while (ptr)
        {
            PlayerNode *tmp = ptr;
            ptr = ptr->next;
            free(tmp);
        }

        ptr = teamList[i].allRounders;
        while (ptr)
        {
            PlayerNode *tmp = ptr;
            ptr = ptr->next;
            free(tmp);
        }
    }
}
