#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1024

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

int Runner(linkedList states);
int ListPrint(linkedList);
int ListInsertSorted(position, position);
int TreePrintCitiesLargerThan(treePointer, int);
int FileReadStates(char[], linkedList);
int FileReadCities(position, char[]);
int TreePrintInorder(treePointer);

position ListFindState(linkedList, char[]);
treePointer TreeCreateEl(char[], int);
position ListCreateHead();
position ListCreateEl(char[]);
treePointer TreeInsert(treePointer, char[], int);
treePointer TreeCreateRoot();

int main()
{

    linkedList states;
    states.next = ListCreateHead();
    states.nextTree = TreeCreateRoot();

    FileReadStates("drzave.txt", states);
    ListPrint(states); // za provjeru ucitanih podataka iz txt

    Runner(states);

    return 0;
}

treePointer TreeCreateRoot()
{

    treePointer root = NULL;

    root = malloc(sizeof(struct treeNode));

    if (!root)
    {
        printf("greska u alokaciji memorije\n");
        return NULL;
    }

    strcpy(root->cityName, "");
    root->left = NULL;
    root->right = NULL;
    root->population = 0;

    return root;
}

int ListInsertSorted(position newElement, position head)
{

    if (head->next == NULL)
    {
        head->next = newElement;
        newElement->next = NULL;
    }

    else
    {
        while (head->next != NULL && (strcmp(head->next->stateName, newElement->stateName) < 0))
            head = head->next;
        newElement->next = head->next;
        head->next = newElement;
    }

    return 0;
}

position ListCreateEl(char stateName[])
{

    position newElement = NULL;

    newElement = (position)malloc(sizeof(struct statelist));

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

int FileReadStates(char stateFile[], linkedList states)
{

    FILE *fp = NULL;
    char stateName[MAX] = {0};
    char cityfile[MAX] = {0};
    position newElement = NULL;

    newElement = (position)malloc(sizeof(linkedList));

    if (!newElement)
    {
        printf("greska u alokaciji memorije\n");
        return 0;
    }

    fp = fopen(stateFile, "r");

    if (!fp)
    {
        printf("datoteka se ne moze otvoriti\n");
        return EXIT_FAILURE;
    }

    while (!feof(fp))
    {
        fscanf(fp, "%[^,],%s\n", stateName, cityfile);
        newElement = ListCreateEl(stateName);
        ListInsertSorted(newElement, states.next);
        FileReadCities(newElement, cityfile);
    }

    fclose(fp);

    return 0;
}

int FileReadCities(position head, char cityfile[])
{

    FILE *fp = NULL;
    char cityName[MAX] = {0};
    int population = 0;
    treePointer newElement = NULL;
    treePointer root = head->nextTree;

    fp = fopen(cityfile, "r");

    if (!fp)
    {
        printf("datoteka se ne moze otvoriti\n");
        return EXIT_FAILURE;
    }

    while (!feof(fp))
    {
        fscanf(fp, "%[^,],%d\n", cityName, &population);
        newElement = TreeCreateRoot();
        newElement = TreeInsert(root, cityName, population);
    }

    fclose(fp);

    return 0;
}

int TreePrintInorder(treePointer root)
{

    if (root == NULL)
        return 0;

    TreePrintInorder(root->left);
    if (root->population != 0)
        printf("%s, %d ljudi\n", root->cityName, root->population);
    TreePrintInorder(root->right);
    return 0;
}

treePointer TreeCreateEl(char cityName[], int population)
{

    treePointer newElement = NULL;

    newElement = (treePointer)malloc(sizeof(struct treeNode));

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

position ListFindState(linkedList states, char stateName[])
{

    position current = NULL;

    current = (position)malloc(sizeof(linkedList));

    if (!current)
    {
        printf("greska u alokaciji memorije\n");
        return NULL;
    }

    current = states.next;

    while (current->next != NULL && (strcmp(current->next->stateName, stateName)))
        current = current->next;

    if ((current->next == NULL || strcmp(stateName, current->next->stateName)))
    {
        return NULL;
    }

    current = current->next;

    return current;
}

int Runner(linkedList states)
{

    int choice = 0;
    int population = 0;
    char stateName[MAX] = {0};
    char cityName[MAX] = {0};
    position currentList = NULL;
    treePointer currentTree = NULL;
    treePointer temp = NULL;

    currentList = (position)malloc(sizeof(linkedList));

    if (!currentList)
    {
        printf("greska u alokaciji memorije\n");
        return EXIT_FAILURE;
    }

    currentTree = (treePointer)malloc(sizeof(struct treeNode));

    if (!currentTree)
    {
        printf("greska u alokaciji memorije\n");
        return EXIT_FAILURE;
    }

    temp = (treePointer)malloc(sizeof(struct treeNode));

    if (!temp)
    {
        printf("greska u alokaciji memorije\n");
        return EXIT_FAILURE;
    }

    while (1)
    {

        printf("1 - izaberi drzavu\n");
        printf("2 - exit\n");
        scanf(" %d", &choice);
        if (choice == 1)
        {
            printf("ime drzave: ");
            scanf(" %s", stateName);
            currentList = ListFindState(states, stateName);

            if (!currentList)
            {
                printf("unesena drzava ne postoji\n");
            }

            else
            {
                printf("drzavu %s je pronadena\n", currentList->stateName);
                printf("gradovi drzave su:\n");
                TreePrintInorder(currentList->nextTree);

                temp = currentList->nextTree;
                printf("\nunesi broj stanovnika u gradu: ");
                scanf("%d", &population);
                printf("gradovi sa vise od %d stanovnika u gradu %s su:\n", population, stateName);
                TreePrintCitiesLargerThan(temp, population);
            }
            printf("\n");
        }
        else if (choice == 2)
        {
            break;
        }
        else
        {
            printf("greska u unosu\n");
        }
    }

    return 0;
}

treePointer TreeInsert(treePointer root, char cityName[], int population)
{

    if (root == NULL)
    {

        root = (treePointer)malloc(sizeof(struct treeNode));
        strcpy(root->cityName, cityName);
        root->population = population;
        root->left = NULL;
        root->right = NULL;
    }

    if (root->population > population)
        root->left = TreeInsert(root->left, cityName, population);

    else if (root->population < population)
        root->right = TreeInsert(root->right, cityName, population);

    else if (strcmp(root->cityName, cityName) > 0)
        root->left = TreeInsert(root->left, cityName, population);

    else if (strcmp(root->cityName, cityName) < 0)
        root->right = TreeInsert(root->right, cityName, population);

    return root;
}

position ListCreateHead()
{

    position head = NULL;
    head = (position)malloc(sizeof(linkedList));

    if (!head)
    {
        printf("greska u alokaciji memorije\n");
        return NULL;
    }

    strcpy(head->stateName, "");
    head->next = NULL;
    head->nextTree = NULL;
    head->next = NULL;

    return head;
}

int ListPrint(linkedList states)
{

    position current = states.next;

    while (current->next != NULL)
    {
        current = current->next;
        printf("drzava: %s \ngradovi:\n", current->stateName);
        TreePrintInorder(current->nextTree);
        printf("\n");
    }

    return 0;
}

int TreePrintCitiesLargerThan(treePointer current, int population)
{

    if (current == NULL)
    {
        return 0;
    }

    TreePrintCitiesLargerThan(current->left, population);

    if (current->population >= population)
        printf("%s, %d ljudi\n", current->cityName, current->population);

    TreePrintCitiesLargerThan(current->right, population);
    return 0;
}