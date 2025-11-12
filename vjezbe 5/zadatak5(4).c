#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_PATH "Exercise5/expr.txt"
#define BUFFER_SIZE 100

typedef struct PolyNode {
    int coef;
    int exp;
    struct PolyNode* next;
} PolyNode;

PolyNode* sort(PolyNode*);                    // i dalje deklariran, iako se ne koristi
PolyNode* add_poly(PolyNode*, PolyNode*, PolyNode**);
PolyNode* mult_poly(PolyNode*, PolyNode*, PolyNode**);
PolyNode* create_node(int, int);
PolyNode* insert_term(PolyNode*, int, int);
char* read_file();
int parse_buffer(PolyNode**, PolyNode**, char*);
void print_poly(PolyNode*);
void free_poly(PolyNode*);

int main(void) {
    PolyNode* sum_head = NULL;
    PolyNode* prod_head = NULL;
    PolyNode* second = NULL;
    PolyNode* first = NULL;
    int sum_done = 0, prod_done = 0;

    char* buffer = read_file();
    if (buffer == NULL) {
        return EXIT_FAILURE;
    }

    int choice;
    if ((parse_buffer(&first, &second, buffer)) == 1) {
        printf("Error while adding to current polynomial!\n");
        free(buffer);
        return EXIT_FAILURE;
    }
    free(buffer);

    do {
        printf("1 - Addition\n");
        printf("2 - Multiplication\n");
        printf("0 - Exit\n");
        printf("Choose option: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            if (sum_done) {
                printf("You already used this option!\n");
                break;
            }
            if (!add_poly(first, second, &sum_head)) {
                return EXIT_FAILURE;
            }
            sum_done = 1;
            print_poly(sum_head);
            printf("\n");
            break;

        case 2:
            if (prod_done) {
                printf("You already used this option!\n");
                break;
            }
            if (!mult_poly(first, second, &prod_head)) {
                return EXIT_FAILURE;
            }
            prod_done = 1;
            print_poly(prod_head);
            printf("\n");
            break;

        case 0:
            break;

        default:
            printf("Invalid option!\n");
            break;
        }
    } while (choice != 0);

    free_poly(sum_head);
    free_poly(prod_head);
    free_poly(first);
    free_poly(second);

    return 0;
}

char* read_file() {
    int j = 0;
    FILE* f1 = fopen(FILE_PATH, "r");
    if (!f1) {
        printf("Error opening file!\n");
        return NULL;
    }

    char* buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));
    char* cleaned_buffer = (char*)malloc(BUFFER_SIZE * sizeof(char));
    if (!buffer || !cleaned_buffer) {
        printf("Allocation error! (read_file)\n");
        if (buffer) free(buffer);
        if (cleaned_buffer) free(cleaned_buffer);
        fclose(f1);
        return NULL;
    }

    while (fgets(buffer, BUFFER_SIZE, f1)) {
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] < '0' || buffer[i] > '9') {
                continue;
            }
            cleaned_buffer[j++] = buffer[i];
        }
        cleaned_buffer[j++] = ' ';
    }
    cleaned_buffer[j] = '\0';

    free(buffer);
    fclose(f1);
    return cleaned_buffer;
}

PolyNode* create_node(int coef, int exp) {
    PolyNode* new_node = (PolyNode*)malloc(sizeof(PolyNode));
    if (!new_node) {
        printf("Allocation error! (create_node)\n");
        return NULL;
    }

    new_node->next = NULL;
    new_node->coef = coef;
    new_node->exp = exp;

    return new_node;
}

PolyNode* insert_term(PolyNode* head, int coef, int exp) {
    PolyNode* new_node = create_node(coef, exp);
    if (!new_node) {
        printf("Allocation error (insert_term)!\n");
        return NULL;
    }

    if (head == NULL || new_node->exp > head->exp) {
        new_node->next = head;
        return new_node;
    }

    if (new_node->exp == head->exp) {
        head->coef += new_node->coef;
        free(new_node);
        return head;
    }

    PolyNode* current = head;
    PolyNode* prev = NULL;

    while (current && current->exp > exp) {
        prev = current;
        current = current->next;
    }

    if (current && current->exp == exp) {
        current->coef += coef;
        free(new_node);
        return head;
    }

    prev->next = new_node;
    new_node->next = current;
    return head;
}

void print_poly(PolyNode* head) {
    PolyNode* temp = head;

    while (temp != NULL) {
        printf("%dx^%d ", temp->coef, temp->exp);
        if (temp->next != NULL) {
            printf("+ ");
        }
        temp = temp->next;
    }
}

void free_poly(PolyNode* head) {
    PolyNode* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

int parse_buffer(PolyNode** first, PolyNode** second, char* buffer) {
    int i = 0;
    int coef, exp;
    PolyNode** current = first;

    while (buffer[i] != ' ' && buffer[i] != '\0') {
        coef = buffer[i++] - '0';
        if (buffer[i] == ' ' || buffer[i] == '\0') break;
        exp = buffer[i++] - '0';
        *current = insert_term(*current, coef, exp);
        if (!*current) {
            return EXIT_FAILURE;
        }
    }

    if (buffer[i] == ' ') i++;
    current = second;

    while (buffer[i] != '\0') {
        coef = buffer[i++] - '0';
        if (buffer[i] == '\0') break;
        exp = buffer[i++] - '0';
        *current = insert_term(*current, coef, exp);
        if (!*current) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

PolyNode* add_poly(PolyNode* first, PolyNode* second, PolyNode** head) {
    PolyNode* temp1 = first;
    PolyNode* temp2 = second;
    int coef, exp;

    while (temp1 != NULL && temp2 != NULL) {
        if (temp1->exp == temp2->exp) {
            coef = temp1->coef + temp2->coef;
            exp = temp1->exp;
            *head = insert_term(*head, coef, exp);
            temp1 = temp1->next;
            temp2 = temp2->next;
        }
        else if (temp1->exp > temp2->exp) {
            *head = insert_term(*head, temp1->coef, temp1->exp);
            temp1 = temp1->next;
        }
        else {
            *head = insert_term(*head, temp2->coef, temp2->exp);
            temp2 = temp2->next;
        }
    }

    while (temp1 != NULL) {
        *head = insert_term(*head, temp1->coef, temp1->exp);
        temp1 = temp1->next;
    }
    while (temp2 != NULL) {
        *head = insert_term(*head, temp2->coef, temp2->exp);
        temp2 = temp2->next;
    }

    return *head;
}

PolyNode* mult_poly(PolyNode* first, PolyNode* second, PolyNode** head) {
    PolyNode* temp1 = first;
    PolyNode* temp2 = second;
    int coef, exp;

    while (temp1 != NULL) {
        temp2 = second;
        while (temp2 != NULL) {
            coef = temp1->coef * temp2->coef;
            exp = temp1->exp + temp2->exp;
            *head = insert_term(*head, coef, exp);
            if (!*head) {
                return NULL;
            }
            temp2 = temp2->next;
        }
        temp1 = temp1->next;
    }

    return *head;
}