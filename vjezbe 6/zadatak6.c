#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_DATE 12
#define MAX_LINE 256

typedef struct Item *ItemPos;
typedef struct Bill *BillPos;

typedef struct Item
{
    char name[MAX_NAME];
    int quantity;
    double price;
    ItemPos next;
} Item;

typedef struct Bill
{
    char date[MAX_DATE]; // YYYY-MM-DD
    ItemPos items;       // linked list of items for this bill
    BillPos next;        // next bill
} Bill;

// loading
int loadAllBills(const char *listFileName, BillPos head);
int loadSingleBill(const char *fileName, BillPos head);

// create nodes
BillPos createBill(const char *date);
ItemPos createItem(const char *name, int quantity, double price);

// insert sorted
int insertBillSorted(BillPos head, BillPos newBill); // sorted by date
int insertItemSorted(BillPos bill, ItemPos newItem); // sorted by item name

// user query
void query(BillPos head);

// debug print
void printAll(BillPos head);

// free memory
void freeAllBills(BillPos head);
void freeAllItems(ItemPos first);

// helper
void trimNewline(char *s);

int main(void)
{
    Bill head;
    head.next = NULL;
    head.items = NULL;
    strcpy(head.date, ""); // dummy head, not used

    if (loadAllBills("racuni.txt", &head) != 0)
    {
        printf("Error while reading bills.\n");
        return EXIT_FAILURE;
    }

    // debug:
    printAll(&head);

    query(&head);

    freeAllBills(&head);

    return 0;
}

void trimNewline(char *s)
{
    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
    {
        s[len - 1] = '\0';
        len--;
    }
}

BillPos createBill(const char *date)
{
    BillPos newBill = (BillPos)malloc(sizeof(Bill));
    if (!newBill)
    {
        printf("Memory allocation error for bill.\n");
        return NULL;
    }

    strncpy(newBill->date, date, MAX_DATE);
    newBill->date[MAX_DATE - 1] = '\0';

    newBill->items = NULL;
    newBill->next = NULL;

    return newBill;
}

ItemPos createItem(const char *name, int quantity, double price)
{
    ItemPos newItem = (ItemPos)malloc(sizeof(Item));
    if (!newItem)
    {
        printf("Memory allocation error for item.\n");
        return NULL;
    }

    strncpy(newItem->name, name, MAX_NAME);
    newItem->name[MAX_NAME - 1] = '\0';

    newItem->quantity = quantity;
    newItem->price = price;
    newItem->next = NULL;

    return newItem;
}

int insertBillSorted(BillPos head, BillPos newBill)
{
    BillPos prev = head;
    BillPos curr = head->next;

    // ascending order by date (oldest first)
    while (curr != NULL && strcmp(curr->date, newBill->date) < 0)
    {
        prev = curr;
        curr = curr->next;
    }

    newBill->next = curr;
    prev->next = newBill;

    return 0;
}

int insertItemSorted(BillPos bill, ItemPos newItem)
{
    ItemPos prev = NULL;
    ItemPos curr = bill->items;

    // ascending order by name
    while (curr != NULL && strcmp(curr->name, newItem->name) < 0)
    {
        prev = curr;
        curr = curr->next;
    }

    if (prev == NULL)
    {
        // insert at beginning
        newItem->next = bill->items;
        bill->items = newItem;
    }
    else
    {
        newItem->next = curr;
        prev->next = newItem;
    }

    return 0;
}

int loadAllBills(const char *listFileName, BillPos head)
{
    FILE *f = fopen(listFileName, "r");
    if (!f)
    {
        perror("Error opening list file");
        return -1;
    }

    char fileName[256];

    while (fgets(fileName, sizeof(fileName), f))
    {
        trimNewline(fileName);

        if (fileName[0] == '\0')
            continue; // skip empty lines

        if (loadSingleBill(fileName, head) != 0)
        {
            printf("Error loading bill from file: %s\n", fileName);
            // could return -1; but let's continue
        }
    }

    fclose(f);
    return 0;
}

int loadSingleBill(const char *fileName, BillPos head)
{
    FILE *f = fopen(fileName, "r");
    if (!f)
    {
        perror("Error opening bill file");
        return -1;
    }

    char line[MAX_LINE];

    // first line = date
    if (!fgets(line, sizeof(line), f))
    {
        printf("Empty or invalid bill file: %s\n", fileName);
        fclose(f);
        return -1;
    }
    trimNewline(line);

    BillPos newBill = createBill(line);
    if (!newBill)
    {
        fclose(f);
        return -1;
    }

    insertBillSorted(head, newBill);

    // read items
    while (fgets(line, sizeof(line), f))
    {
        trimNewline(line);
        if (line[0] == '\0')
            continue;

        char name[MAX_NAME];
        int quantity;
        double price;

        // example line: "Bread, 2, 1.20"
        if (sscanf(line, " %[^,], %d, %lf", name, &quantity, &price) == 3)
        {
            ItemPos newItem = createItem(name, quantity, price);
            if (!newItem)
            {
                fclose(f);
                return -1;
            }
            insertItemSorted(newBill, newItem);
        }
        else
        {
            printf("Invalid line in file %s: %s\n", fileName, line);
        }
    }

    fclose(f);
    return 0;
}

void query(BillPos head)
{
    char targetName[MAX_NAME];
    char startDate[MAX_DATE];
    char endDate[MAX_DATE];

    printf("Enter item name: ");
    fgets(targetName, sizeof(targetName), stdin);
    trimNewline(targetName);

    printf("Enter start date (YYYY-MM-DD): ");
    fgets(startDate, sizeof(startDate), stdin);
    trimNewline(startDate);

    printf("Enter end date (YYYY-MM-DD): ");
    fgets(endDate, sizeof(endDate), stdin);
    trimNewline(endDate);

    double totalSpent = 0.0;
    int totalQuantity = 0;

    BillPos currentBill = head->next;

    while (currentBill != NULL)
    {
        // skip bills before start date
        if (strcmp(currentBill->date, startDate) < 0)
        {
            currentBill = currentBill->next;
            continue;
        }

        // since list is sorted, we can stop after endDate
        if (strcmp(currentBill->date, endDate) > 0)
        {
            break;
        }

        ItemPos item = currentBill->items;
        while (item != NULL)
        {
            if (strcmp(item->name, targetName) == 0)
            {
                totalQuantity += item->quantity;
                totalSpent += item->quantity * item->price;
            }
            item = item->next;
        }

        currentBill = currentBill->next;
    }

    printf("\nRESULTS:\n");
    printf("Item: %s\n", targetName);
    printf("Date range: %s to %s\n", startDate, endDate);
    printf("Total quantity purchased: %d\n", totalQuantity);
    printf("Total money spent: %.2lf\n", totalSpent);
}

void printAll(BillPos head)
{
    BillPos bill = head->next;

    if (!bill)
    {
        printf("No bills loaded.\n");
        return;
    }

    printf("\n=== ALL BILLS ===\n");

    while (bill != NULL)
    {
        printf("Bill date: %s\n", bill->date);

        ItemPos item = bill->items;
        if (!item)
        {
            printf("  (no items)\n");
        }
        else
        {
            while (item != NULL)
            {
                printf("  %-20s  qty: %3d  price: %7.2lf\n",
                       item->name, item->quantity, item->price);
                item = item->next;
            }
        }

        printf("\n");
        bill = bill->next;
    }
}

void freeAllItems(ItemPos first)
{
    ItemPos curr = first;
    while (curr != NULL)
    {
        ItemPos temp = curr;
        curr = curr->next;
        free(temp);
    }
}

void freeAllBills(BillPos head)
{
    BillPos currBill = head->next;
    head->next = NULL;

    while (currBill != NULL)
    {
        BillPos tempBill = currBill;
        currBill = currBill->next;

        freeAllItems(tempBill->items);
        free(tempBill);
    }
}