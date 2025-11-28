#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY 1000
#define MAX_VALUE_LEN 100

typedef struct Node
{
    int key;
    char value[MAX_VALUE_LEN];
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct LRUCache
{
    int capacity;
    int size;
    Node *head;
    Node *tail;
    Node *map[MAX_KEY];
} LRUCache;

Node *createNode(int key, const char *value);
LRUCache *createCache(int capacity);
void detachNode(Node *node);
void insertAtHead(LRUCache *cache, Node *node);
char *get(LRUCache *cache, int key);
void put(LRUCache *cache, int key, const char *value);
void freeCache(LRUCache *cache);

int main()
{
    LRUCache *cache = NULL;
    char command[32];
    while (scanf("%s", command) != EOF)
    {
        if (strcmp(command, "createCache") == 0)
        {
            int cap;
            scanf("%d", &cap);
            if (cache){
                freeCache(cache);
            }
            cache = createCache(cap);
        }
        else if (strcmp(command, "put") == 0)
        {
            int key;
            char value[MAX_VALUE_LEN];
            scanf("%d %s", &key, value);
            if (cache){
                put(cache, key, value);
            }
        }
        else if (strcmp(command, "get") == 0)
        {
            int key;
            scanf("%d", &key);
            if (cache){
                printf("%s\n", get(cache, key));
            }
            else{
                printf("NULL\n");
            }
        }
        else if (strcmp(command, "exit") == 0)
        {
            break;
        }
    }
    if (cache){
        freeCache(cache);
    }
    return 0;
}

Node *createNode(int key, const char *value)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->key = key;
    strncpy(node->value, value, MAX_VALUE_LEN);
    node->prev = NULL;
    node->next = NULL;
    return node;
}

LRUCache *createCache(int capacity)
{
    if (capacity < 1 || capacity > MAX_KEY){
        return NULL;
    }
    LRUCache *cache = (LRUCache *)malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = createNode(-1, "");
    cache->tail = createNode(-1, "");
    cache->head->next = cache->tail;
    cache->tail->prev = cache->head;
    for (int i = 0; i < MAX_KEY; i++){
        cache->map[i] = NULL;
    }
    return cache;
}

void detachNode(Node *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
}

void insertAtHead(LRUCache *cache, Node *node)
{
    node->next = cache->head->next;
    node->prev = cache->head;
    cache->head->next->prev = node;
    cache->head->next = node;
}

char *get(LRUCache *cache, int key)
{
    if (!cache || key < 0 || key >= MAX_KEY){
        return "NULL";
    }
    Node *node = cache->map[key];
    if (!node){
        return "NULL";
    }
    detachNode(node);
    insertAtHead(cache, node);
    return node->value;
}

void put(LRUCache *cache, int key, const char *value)
{
    if (!cache || key < 0 || key >= MAX_KEY){
        return;
    }
    Node *node = cache->map[key];
    if (node)
    {
        strncpy(node->value, value, MAX_VALUE_LEN);
        detachNode(node);
        insertAtHead(cache, node);
        return;
    }
    if (cache->size == cache->capacity)
    {
        Node *lru = cache->tail->prev;
        detachNode(lru);
        cache->map[lru->key] = NULL;
        free(lru);
        cache->size--;
    }
    Node *newNode = createNode(key, value);
    insertAtHead(cache, newNode);
    cache->map[key] = newNode;
    cache->size++;
}

void freeCache(LRUCache *cache)
{
    if (!cache){
        return;
    }
    Node *curr = cache->head;
    while (curr)
    {
        Node *next = curr->next;
        free(curr);
        curr = next;
    }
    free(cache);
}