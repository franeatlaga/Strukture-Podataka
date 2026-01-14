#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1024
#define N 11

typedef struct treeNode *treePointer;
struct treeNode
{
    char cityName[MAX];
    int population;
    treePointer left;
    treePointer right;
};

struct statelist;
typedef struct statelist *position;
typedef struct statelist
{
    char stateName[MAX];
    treePointer nextTree;
    position next;
} linkedList;

int Hash(char stateName[]);
int HashTablePrint(linkedList hashTable[]);

int Runner(linkedList hashTable[]);
int ListPrint(linkedList states);
int ListInsertSorted(position newElement, position head);
int TreePrintCitiesLargerThan(treePointer current, int population);
int FileReadStates(char stateFile[], linkedList hashTable[]);
int FileReadCities(position head, char cityfile[]);
int TreePrintInorder(treePointer root);

position ListFindState(linkedList hashTable[], char stateName[]);
treePointer TreeCreateEl(char cityName[], int population);
position ListCreateHead();
position ListCreateEl(char stateName[]);
treePointer TreeInsert(treePointer root, char cityName[], int population);
treePointer TreeCreateRoot();

FILE *OpenFileSmart(const char *filename, const char *mode);

int main(void)
{
    int i;
    linkedList hashTable[N];

    for (i = 0; i < N; i++)
    {
        hashTable[i].next = ListCreateHead();
        hashTable[i].nextTree = TreeCreateRoot();
    }

    if (FileReadStates("drzave.txt", hashTable) != 0)
        return 1;

    HashTablePrint(hashTable);
    Runner(hashTable);

    return 0;
}

FILE *OpenFileSmart(const char *filename, const char *mode)
{
    FILE *fp = fopen(filename, mode);
    if (fp)
        return fp;

    char path[MAX];
    snprintf(path, MAX, "../%s", filename);
    fp = fopen(path, mode);
    return fp;
}

int Hash(char stateName[])
{
    int sum = 0;
    int i;

    for (i = 0; i < 5 && stateName[i] != '\0'; i++)
        sum += (unsigned char)stateName[i];

    return sum % N;
}

int HashTablePrint(linkedList hashTable[])
{
    int i;

    for (i = 0; i < N; i++)
    {
        printf("----\nBucket %d\n", i);
        ListPrint(hashTable[i]);
    }

    return 0;
}

position ListCreateHead()
{
    position head = (position)malloc(sizeof(linkedList));
    if (!head)
    {
        printf("greska u alokaciji memorije\n");
        return NULL;
    }

    strcpy(head->stateName, "");
    head->nextTree = NULL;
    head->next = NULL;

    return head;
}

position ListCreateEl(char stateName[])
{
    position newElement = (position)malloc(sizeof(linkedList));
    if (!newElement)
    {
        printf("greska u alokaciji memorije\n");
        return NULL;
    }

    strcpy(newElement->stateName, stateName);
    newElement->next = NULL;
    newElement->nextTree = TreeCreateRoot();

    return newElement;
}

int ListInsertSorted(position newElement, position head)
{
    while (head->next != NULL && strcmp(head->next->stateName, newElement->stateName) < 0)
        head = head->next;

    newElement->next = head->next;
    head->next = newElement;

    return 0;
}

int ListPrint(linkedList states)
{
    position current = states.next;

    while (current->next != NULL)
    {
        current = current->next;
        printf("  drzava: %s\n  gradovi:\n", current->stateName);
        TreePrintInorder(current->nextTree);
        printf("\n");
    }

    return 0;
}

position ListFindState(linkedList hashTable[], char stateName[])
{
    int hashIdx = Hash(stateName);
    position current = hashTable[hashIdx].next;

    while (current != NULL && strcmp(current->stateName, stateName) != 0)
        current = current->next;

    return current;
}

int FileReadStates(char stateFile[], linkedList hashTable[])
{
    FILE *fp = OpenFileSmart(stateFile, "r");
    char stateName[MAX] = {0};
    char cityfile[MAX] = {0};

    if (!fp)
    {
        printf("datoteka %s se ne moze otvoriti\n", stateFile);
        return 1;
    }

    while (fscanf(fp, " %[^,],%s", stateName, cityfile) == 2)
    {
        int hashIdx = Hash(stateName);
        position newElement = ListCreateEl(stateName);

        if (!newElement)
        {
            fclose(fp);
            return 1;
        }

        ListInsertSorted(newElement, hashTable[hashIdx].next);

        if (FileReadCities(newElement, cityfile) != 0)
        {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

int FileReadCities(position head, char cityfile[])
{
    FILE *fp = OpenFileSmart(cityfile, "r");
    char cityName[MAX] = {0};
    int population = 0;

    if (!fp)
    {
        printf("datoteka %s se ne moze otvoriti\n", cityfile);
        return 1;
    }

    while (fscanf(fp, " %[^,],%d", cityName, &population) == 2)
        head->nextTree = TreeInsert(head->nextTree, cityName, population);

    fclose(fp);
    return 0;
}

treePointer TreeCreateRoot()
{
    treePointer root = (treePointer)malloc(sizeof(struct treeNode));
    if (!root)
    {
        printf("greska u alokaciji memorije\n");
        return NULL;
    }

    strcpy(root->cityName, "");
    root->population = 0;
    root->left = NULL;
    root->right = NULL;

    return root;
}

treePointer TreeCreateEl(char cityName[], int population)
{
    treePointer newElement = (treePointer)malloc(sizeof(struct treeNode));
    if (!newElement)
    {
        printf("greska u alokaciji memorije\n");
        return NULL;
    }

    strcpy(newElement->cityName, cityName);
    newElement->population = population;
    newElement->left = NULL;
    newElement->right = NULL;

    return newElement;
}

treePointer TreeInsert(treePointer root, char cityName[], int population)
{
    if (root == NULL)
        return TreeCreateEl(cityName, population);

    if (root->population == 0 && root->cityName[0] == '\0')
    {
        strcpy(root->cityName, cityName);
        root->population = population;
        return root;
    }

    if (population < root->population)
        root->left = TreeInsert(root->left, cityName, population);
    else if (population > root->population)
        root->right = TreeInsert(root->right, cityName, population);
    else
    {
        if (strcmp(cityName, root->cityName) < 0)
            root->left = TreeInsert(root->left, cityName, population);
        else if (strcmp(cityName, root->cityName) > 0)
            root->right = TreeInsert(root->right, cityName, population);
    }

    return root;
}

int TreePrintInorder(treePointer root)
{
    if (root == NULL)
        return 0;

    TreePrintInorder(root->left);

    if (root->population != 0)
        printf("    %s, %d ljudi\n", root->cityName, root->population);

    TreePrintInorder(root->right);

    return 0;
}

int TreePrintCitiesLargerThan(treePointer current, int population)
{
    if (current == NULL)
        return 0;

    TreePrintCitiesLargerThan(current->left, population);

    if (current->population >= population && current->population != 0)
        printf("    %s, %d ljudi\n", current->cityName, current->population);

    TreePrintCitiesLargerThan(current->right, population);

    return 0;
}

int Runner(linkedList hashTable[])
{
    int choice = 0;
    int population = 0;
    char stateName[MAX] = {0};
    position currentList = NULL;

    while (1)
    {
        printf("1 - izaberi drzavu\n");
        printf("2 - exit\n");
        printf("odabir: ");
        scanf(" %d", &choice);

        if (choice == 1)
        {
            printf("ime drzave: ");
            scanf(" %s", stateName);

            currentList = ListFindState(hashTable, stateName);

            if (!currentList)
            {
                printf("unesena drzava ne postoji\n\n");
                continue;
            }

            printf("drzava %s je pronadena\n", currentList->stateName);
            printf("gradovi drzave su:\n");
            TreePrintInorder(currentList->nextTree);

            printf("\nunesi broj stanovnika: ");
            scanf(" %d", &population);

            printf("gradovi sa vise od %d stanovnika u drzavi %s su:\n", population, stateName);
            TreePrintCitiesLargerThan(currentList->nextTree, population);
            printf("\n");
        }
        else if (choice == 2)
        {
            break;
        }
        else
        {
            printf("greska u unosu\n\n");
        }
    }

    return 0;
}