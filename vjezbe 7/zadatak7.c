#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 64

typedef struct Dir* DirPos;
typedef struct Dir {
    char name[NAME_LEN];
    DirPos child;   
    DirPos sibling;   
    DirPos parent;    
} Dir;
DirPos createDir(const char* name, DirPos parent);
void md(DirPos current, const char* name);
DirPos cd(DirPos current, const char* name);
DirPos cdUp(DirPos current);
void dir(DirPos current);
void freeAll(DirPos current);

int main() {
    int choice;
    char name[NAME_LEN];

    Dir root = { "C:", NULL, NULL, NULL };
    DirPos current = &root;

    while(1){
        printf("\nCurrent directory: %s\n", current->name);
        printf("1 - mkdir   2 - cd   3 - cd..   4 - dir   5 - exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar(); // clear buffer

        switch (choice) {
        case 1: // md
            printf("Enter directory name: ");
            fgets(name, NAME_LEN, stdin);
            name[strcspn(name, "\n")] = 0;
            md(current, name);
            break;

        case 2: // cd
            printf("Enter directory name: ");
            fgets(name, NAME_LEN, stdin);
            name[strcspn(name, "\n")] = 0;
            current = cd(current, name);
            break;

        case 3: // cd..
            current = cdUp(current);
            break;

        case 4: // dir
            dir(current);
            break;

        case 5: // exit
            return 0;
            freeAll(root.child);

        default:
            printf("Invalid option.\n");
        }
    }

    return 0;
}

// make new node
DirPos createDir(const char* name, DirPos parent) {
    DirPos d = (DirPos)malloc(sizeof(*d));
    if (!d) return NULL;
    strcpy(d->name, name);
    d->child = d->sibling = NULL;
    d->parent = parent;
    return d;
}


// md name
void md(DirPos current, const char* name) {
    DirPos newDir = createDir(name, current);
    if (!newDir) return;

    // insert at beginning of child list
    newDir->sibling = current->child;
    current->child = newDir;
}

// cd name
DirPos cd(DirPos current, const char* name) {
    DirPos temp = current->child;
    while (temp) {
        if (strcmp(temp->name, name) == 0) return temp;
        temp = temp->sibling;
    }
    printf("Directory not found.\n");
    return current;
}

// cd ..
DirPos cdUp(DirPos current) {
    if (current->parent) return current->parent;
    printf("Already at root.\n");
    return current;
}


// dir
void dir(DirPos current) {
    DirPos temp = current->child;
    while (temp) {
        printf("%s\n", temp->name);
        temp = temp->sibling;
    }
}
void freeAll(DirPos current) {
    if (!current) return;

    freeAll(current->child);
    freeAll(current->sibling);
    free(current);
}