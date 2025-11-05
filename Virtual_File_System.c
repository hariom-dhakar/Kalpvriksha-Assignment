#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_FILENAME_LEN 50
#define MAX_BLOCK_PER_FILE 100

char **virtualDisk;

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct File
{
    char name[MAX_FILENAME_LEN];
    int isDirectory;
    struct File *parent;
    struct File *next;
    struct File *prev;

    int blockCount;
    int blocks[MAX_BLOCK_PER_FILE];

    struct File *children;
} File;

File *root = NULL;
File *cwd = NULL;
FreeBlock *freeBlockHead = NULL;
FreeBlock *freeBlockTail = NULL;
int usedBlocks = 0;

void initDisk();
void initFreeBlocks();
void initFileSystem();
void handleInput(char *dataInput);
void printPath(File *dir);
void mkdir(char *name);
void rmdirCmd(char *name);
void lsCmd();
void cdCmd(char *name);
void createCmd(char *name);
void writeCmd(char *name, char *data);
void readCmd(char *name);
void deleteCmd(char *name);
void dfCmd();
void freeAll(File *dir);
void cleanup();

int main()
{
    initFileSystem();
    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char cmd[32];
    char fileNameInput[64];
    char dataInput[1024];

    while (1)
    {
        printPath(cwd);
        printf("> ");

        if (scanf("%s", cmd) != 1)
            continue;

        if (strcmp(cmd, "exit") == 0)
        {
            cleanup();
            printf("Memory released. Exiting program...\n");
            break;
        }
        else if (strcmp(cmd, "mkdir") == 0)
        {
            scanf("%s", fileNameInput);
            mkdir(fileNameInput);
        }
        else if (strcmp(cmd, "rmdir") == 0)
        {
            scanf("%s", fileNameInput);
            rmdirCmd(fileNameInput);
        }

        else if (strcmp(cmd, "ls") == 0)
        {
            lsCmd();
        }
        else if (strcmp(cmd, "cd") == 0)
        {
            scanf("%s", fileNameInput);
            cdCmd(fileNameInput);
        }
        else if (strcmp(cmd, "create") == 0)
        {
            scanf("%s", fileNameInput);
            createCmd(fileNameInput);
        }
        else if (strcmp(cmd, "write") == 0)
        {
            scanf("%s", fileNameInput);
            handleInput(dataInput);
            writeCmd(fileNameInput, dataInput);
        }
        else if (strcmp(cmd, "read") == 0)
        {
            scanf("%s", fileNameInput);
            readCmd(fileNameInput);
        }
        else if (strcmp(cmd, "delete") == 0)
        {
            scanf("%s", fileNameInput);
            deleteCmd(fileNameInput);
        }
        else if (strcmp(cmd, "df") == 0)
        {
            dfCmd();
        }
        else
        {
            printf("Invalid command.\n");
        }
    }

    return 0;
}

void initDisk()
{
    virtualDisk = malloc(NUM_BLOCKS * sizeof(char *));
    for (int i = 0; i < NUM_BLOCKS; i++){
        virtualDisk[i] = calloc(BLOCK_SIZE, sizeof(char));
    }
}

void initFreeBlocks()
{
    FreeBlock *prev = NULL;
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        FreeBlock *block = malloc(sizeof(FreeBlock));
        block->index = i;
        block->next = NULL;
        block->prev = prev;

        if (prev)
        {
            prev->next = block;
        }
        else
        {
            freeBlockHead = block;
        }
        prev = block;
    }
    freeBlockTail = prev;
}

void initFileSystem()
{
    initDisk();
    initFreeBlocks();
    root = malloc(sizeof(File));
    strcpy(root->name, "/");
    root->isDirectory = 1;
    root->parent = NULL;
    root->children = NULL;
    root->next = root;
    root->prev = root;
    root->blockCount = 0;
    memset(root->blocks, 0, sizeof(root->blocks));

    cwd = root;
    printf("Virtual File System initialized. Root directory created.\n");
}

void handleInput(char *dataInput)
{
    char *ptr = dataInput;
    int ch = getchar();

    while (ch == ' ' || ch == '\t')
    {
        ch = getchar();
    }

    if (ch == '\n' || ch == EOF)
    {
        *dataInput = '\0';
        return;
    }

    if (ch == '"')
    {
        while ((ch = getchar()) != '"' && ch != EOF && (ptr - dataInput) < 1023)
        {
            *ptr++ = (char)ch;
        }
    }
    else
    {
        *ptr++ = (char)ch;
        while ((ch = getchar()) != '\n' && ch != EOF && (ptr - dataInput) < 1023)
        {
            *ptr++ = (char)ch;
        }
    }

    *ptr = '\0';
}

void printPath(File *dir)
{
    if (!dir)
    {
        return;
    }
    if (dir->parent)
    {
        printPath(dir->parent);
        if (strcmp(dir->name, "/") != 0){
            printf("%s/", dir->name);
        }
    }
    else
    {
        printf("/");
    }
}
void mkdir(char *name)
{
    if (!cwd->isDirectory)
    {
        printf("Err: Cannot create directory inside a file\n");
        return;
    }

    File *check = cwd->children;
    if (check)
    {
        do
        {
            if (strcmp(check->name, name) == 0)
            {
                printf("Err: '%s' already exists in this directory\n", name);
                return;
            }
            check = check->next;
        } while (check != cwd->children);
    }

    File *newDir = malloc(sizeof(File));

    strcpy(newDir->name, name);
    newDir->isDirectory = 1;
    newDir->parent = cwd;
    newDir->children = NULL;
    newDir->blockCount = 0;
    memset(newDir->blocks, 0, sizeof(newDir->blocks));

    if (!cwd->children)
    {
        cwd->children = newDir;
        newDir->next = newDir;
        newDir->prev = newDir;
    }
    else
    {
        File *first = cwd->children;
        File *last = first->prev;

        newDir->next = first;
        newDir->prev = last;
        last->next = newDir;
        first->prev = newDir;
    }

    printf("Directory '%s' created successfully\n", name);
}

void rmdirCmd(char *name)
{
    if (!cwd || !cwd->children)
    {
        printf("This directory has no subdirectories.\n");
        return;
    }

    File *temp = cwd->children;

    do
    {
        if (strcmp(temp->name, name) == 0)
        {
            if (!temp->isDirectory)
            {
                printf("Err: '%s' is a file, not a directory.\n", name);
                return;
            }

            if (temp->children != NULL)
            {
                printf("Err: Directory '%s' is not empty.\n", name);
                return;
            }

            if (temp == cwd->children && temp->next == temp)
            {
                cwd->children = NULL; 
            }
            else
            {
                if (temp == cwd->children){
                    cwd->children = temp->next;
                }

                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }

            free(temp);
            printf("Directory '%s' removed successfully.\n", name);
            return;
        }

        temp = temp->next;
    } while (temp != cwd->children);

    printf("Err: Directory '%s' not found.\n", name);
}


void lsCmd()
{
    if (!cwd)
    {
        printf("Err: Current working directory not set\n");
        return;
    }

    if (cwd->children == NULL)
    {
        printf("(empty)\n");
        return;
    }

    File *temp = cwd->children;
    do
    {
        printf("%s%s  ", temp->name, temp->isDirectory ? "/" : "");
        temp = temp->next;
    } while (temp != cwd->children);

    printf("\n");
}

void cdCmd(char *name)
{
    if (!cwd)
    {
        printf("Err: Current working directory not set.\n");
        return;
    }

    if (strcmp(name, "..") == 0)
    {
        if (cwd->parent)
        {
            cwd = cwd->parent;
            printf("Moved to %s\n", cwd->name);
        }
        else
        {
            printf("Already at root directory.\n");
        }
        return;
    }

    if (!cwd->children)
    {
        printf("This directory has no subdirectories.\n");
        return;
    }

    File *temp = cwd->children;
    do
    {
        if (strcmp(temp->name, name) == 0)
        {
            if (temp->isDirectory)
            {
                cwd = temp;
                printf("Moved to /%s\n", cwd->name);
            }
            else
            {
                printf("Err: '%s' is not a directory.\n", name);
            }
            return;
        }
        temp = temp->next;
    } while (temp != cwd->children);

    printf("Directory not found.\n");
}

void createCmd(char *name)
{
    if (!cwd)
    {
        printf("Err: Current directory not set.\n");
        return;
    }

    if (!cwd->isDirectory)
    {
        printf("Err: Cannot create file inside another file.\n");
        return;
    }

    File *check = cwd->children;
    if (check)
    {
        do
        {
            if (strcmp(check->name, name) == 0)
            {
                printf("Err: '%s' already exists in this directory.\n", name);
                return;
            }
            check = check->next;
        } while (check != cwd->children);
    }

    File *newFile = malloc(sizeof(File));

    strcpy(newFile->name, name);
    newFile->isDirectory = 0;
    newFile->blockCount = 0;
    memset(newFile->blocks, 0, sizeof(newFile->blocks));
    newFile->parent = cwd;
    newFile->children = NULL;

    if (!cwd->children)
    {
        cwd->children = newFile;
        newFile->next = newFile;
        newFile->prev = newFile;
    }
    else
    {
        File *first = cwd->children;
        File *last = first->prev;
        newFile->next = first;
        newFile->prev = last;
        last->next = newFile;
        first->prev = newFile;
    }

    printf("File '%s' created successfully.\n", name);
}

void writeCmd(char *name, char *data)
{
    if (data == NULL || strlen(data) == 0)
    {
        printf("Err: No data provided to write.\n");
        return;
    }
    if (!cwd || !cwd->children)
    {
        printf("No files found in current directory.\n");
        return;
    }

    File *temp = cwd->children;
    do
    {
        if (!temp->isDirectory && strcmp(temp->name, name) == 0)
        {
            if (temp->blockCount > 0)
            {
                for (int i = 0; i < temp->blockCount; i++)
                {
                    FreeBlock *node = malloc(sizeof(FreeBlock));
                    node->index = temp->blocks[i];
                    node->next = freeBlockHead;
                    node->prev = NULL;
                    if (freeBlockHead)
                    {
                        freeBlockHead->prev = node;
                    }
                    freeBlockHead = node;
                    usedBlocks--;
                }
                temp->blockCount = 0;
            }

            int bytes = strlen(data);
            int blocksNeeded = (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;

            if (blocksNeeded > MAX_BLOCK_PER_FILE)
            {
                printf("Err: File too large (max %d blocks).\n", MAX_BLOCK_PER_FILE);
                return;
            }

            for (int i = 0; i < blocksNeeded; i++)
            {
                if (!freeBlockHead)
                {
                    printf("Disk full! Write incomplete.\n");
                    return;
                }

                FreeBlock *tempBlock = freeBlockHead;
                freeBlockHead = tempBlock->next;

                if (freeBlockHead){
                    freeBlockHead->prev = NULL;
                }

                temp->blocks[temp->blockCount++] = tempBlock->index;

                int remaining = bytes - i * BLOCK_SIZE;
                int toCopy = (remaining > BLOCK_SIZE) ? BLOCK_SIZE : remaining;
                memcpy(virtualDisk[tempBlock->index], data + i * BLOCK_SIZE, toCopy);

                usedBlocks++;
            }

            printf("Data written successfully (size=%d bytes).\n", bytes);
            return;
        }
        temp = temp->next;
    } while (temp != cwd->children);

    printf("File not found.\n");
}

void readCmd(char *name)
{
    if (!cwd || !cwd->children)
    {
        printf("No files found in current directory.\n");
        return;
    }

    File *temp = cwd->children;
    do
    {
        if (strcmp(temp->name, name) == 0)
        {
            if (temp->isDirectory)
            {
                printf("Err: '%s' is a directory, not a file.\n", name);
                return;
            }

            if (temp->blockCount == 0)
            {
                printf("File '%s' is empty.\n", name);
                return;
            }

            for (int i = 0; i < temp->blockCount; i++)
            {
                fwrite(virtualDisk[temp->blocks[i]], 1, BLOCK_SIZE, stdout);
            }
            return;
        }

        temp = temp->next;
    } while (temp != cwd->children);

    printf("File not found.\n");
}

void deleteCmd(char *name)
{
    if (!cwd || !cwd->children)
    {
        printf("This directory has no files.\n");
        return;
    }

    File *temp = cwd->children;
    do
    {
        if (!temp->isDirectory && strcmp(temp->name, name) == 0)
        {
            for (int i = 0; i < temp->blockCount; i++)
            {
                FreeBlock *node = malloc(sizeof(FreeBlock));
                node->index = temp->blocks[i];
                node->next = freeBlockHead;
                node->prev = NULL;
                if (freeBlockHead){
                    freeBlockHead->prev = node;
                }
                freeBlockHead = node;
                usedBlocks--;
            }

            if (temp == cwd->children && temp->next == temp)
            {
                cwd->children = NULL;
            }
            else
            {
                if (temp == cwd->children)
                {
                    cwd->children = temp->next;
                }
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            temp->next = temp->prev = NULL;
            free(temp);
            printf("File '%s' deleted successfully.\n", name);
            return;
        }

        if (temp->isDirectory && strcmp(temp->name, name) == 0)
        {
            printf("Err: '%s' is a directory. Use rmdir instead.\n", name);
            return;
        }

        temp = temp->next;
    } while (temp != cwd->children);

    printf("File not found.\n");
}

void dfCmd()
{
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", NUM_BLOCKS - usedBlocks);
    printf("Disk Usage: %.2f%%\n", (usedBlocks * 100.0) / NUM_BLOCKS);
}

void freeAll(File *dir)
{
    if (dir->isDirectory && dir->children)
    {
        File *first = dir->children;
        File *child = first;
        first->prev->next = NULL;
        while (child)
        {
            File *next = child->next;
            freeAll(child);
            child = next;
        }
    }
}

void cleanup()
{
    if (!root)
    {
        return;
    }
    freeAll(root);
    FreeBlock *temp = freeBlockHead;
    while (temp)
    {
        FreeBlock *next = temp->next;
        free(temp);
        temp = next;
    }
    for (int i = 0; i < NUM_BLOCKS; i++)
    {
        free(virtualDisk[i]);
    }
    free(virtualDisk);
}
