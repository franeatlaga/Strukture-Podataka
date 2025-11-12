#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILEPATH "Vjezba5/izraz.txt"
#define MAX 100

typedef struct Poly {
    int coeff;
    int pow;
    struct Poly* next;
} Poly;

Poly* add_poly(Poly*, Poly*, Poly**);
Poly* mult_poly(Poly*, Poly*, Poly**);
Poly* create(int, int);
Poly* add(Poly*, int, int);
char* read_file();
int read_buffer(Poly**, Poly**, char*);
void printpoly(Poly*);
void freepoly(Poly*);

int main(void) {
    Poly* heada = NULL;
    Poly* headm = NULL;
    Poly* second = NULL;
    Poly* first = NULL;
    int statea = 0, statem = 0;

    char* buffer = read_file();
    if (buffer == NULL) {
        return EXIT_FAILURE;
    }

    if ((read_buffer(&first, &second, buffer)) == EXIT_FAILURE) {
        printf("Greska u parsiranju polinoma!\n");
        free(buffer);
        return EXIT_FAILURE;
    }
    free(buffer);

    int choice;
    do {
        printf("1 - Zbrajanje\n");
        printf("2 - Mnozenje\n");
        printf("0 - Izlaz\n");
        printf("Odabir: ");
        scanf("%d", &choice);

        switch (choice) {
        case 1:
            if (statea) {
                printf("Vec ste iskoristili ovu opciju!\n");
                break;
            }
            if (!add_poly(first, second, &heada)) {
                return EXIT_FAILURE;
            }
            statea = 1;
            printpoly(heada);
            printf("\n");
            break;
        case 2:
            if (statem) {
                printf("Vec ste iskoristili ovu opciju!\n");
                break;
            }
            if (!mult_poly(first, second, &headm)) {
                return EXIT_FAILURE;
            }
            statem = 1;
            printpoly(headm);
            printf("\n");
            break;
        case 0:
            break;
        default:
            printf("Greska u unosu opcije!\n");
            break;
        }
    } while (choice != 0);

    freepoly(heada);
    freepoly(headm);
    freepoly(first);
    freepoly(second);

    return 0;
}

char* read_file() {
    FILE* f1 = fopen(FILEPATH, "r");
    if (!f1) {
        printf("Greska u otvaranju filea!\n");
        return NULL;
    }

    // Pomaknemo se na kraj da doznamo velicinu
    if (fseek(f1, 0, SEEK_END) != 0) {
        printf("Greska pri fseek!\n");
        fclose(f1);
        return NULL;
    }

    long length = ftell(f1);
    if (length < 0) {
        printf("Greska pri ftell!\n");
        fclose(f1);
        return NULL;
    }

    rewind(f1);

    char* buffer = (char*)malloc((size_t)length + 1);
    if (!buffer) {
        printf("Greska u alokaciji! (read_file)\n");
        fclose(f1);
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, (size_t)length, f1);
    buffer[read_bytes] = '\0';

    fclose(f1);
    return buffer;
}

Poly* create(int coeff, int pow) {
    Poly* new_poly = (Poly*)malloc(sizeof(Poly));
    if (!new_poly) {
        printf("Greska u alokaciji! (create)\n");
        return NULL;
    }
    new_poly->next = NULL;
    new_poly->coeff = coeff;
    new_poly->pow = pow;

    return new_poly;
}

// Umetanje u sortiranu listu po opadajucoj potenciji, spajanje jednakih
Poly* add(Poly* head, int coeff, int pow) {
    Poly* new_poly = create(coeff, pow);
    if (!new_poly) {
        printf("Greska u alokaciji (add)!\n");
        return NULL;
    }

    if (head == NULL || new_poly->pow > head->pow) {
        new_poly->next = head;
        return new_poly;
    }

    if (new_poly->pow == head->pow) {
        head->coeff += new_poly->coeff;
        free(new_poly);
        return head;
    }

    Poly* current = head;
    Poly* prev = NULL;

    while (current && current->pow > pow) {
        prev = current;
        current = current->next;
    }

    if (current && current->pow == pow) {
        current->coeff += coeff;
        free(new_poly);
        return head;
    }

    prev->next = new_poly;
    new_poly->next = current;
    return head;
}

void printpoly(Poly* head) {
    Poly* temp = head;

    if (!temp) {
        printf("Prazan polinom.");
        return;
    }

    while (temp != NULL) {
        printf("%dx^%d", temp->coeff, temp->pow);
        if (temp->next != NULL) {
            printf(" + ");
        }
        temp = temp->next;
    }
}

void freepoly(Poly* head) {
    Poly* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

/*
    read_buffer:
    - buffer sadrzi SVE iz datoteke (oba polinoma, razdvojena '\n')
    - prvi red -> prvi polinom
    - drugi red -> drugi polinom
    - koristi strtol za citanje brojeva, preskace znakove tipa 'x', '^', '+', itd.
*/
int read_buffer(Poly** first, Poly** second, char* buffer) {
    char* p = buffer;
    int coeff = 0, pow = 0;
    int line = 0;          // 0 -> prvi polinom, 1 -> drugi polinom
    Poly** current = first;

    while (*p != '\0') {
        // novi red znaci novi polinom
        if (*p == '\n') {
            line++;
            if (line == 1) {
                current = second;
            }
            p++;
            continue;
        }

        // preskoci sve sto nije cifra ni znak +/-
        if ((*p < '0' || *p > '9') && *p != '-' && *p != '+') {
            p++;
            continue;
        }

        // citamo koeficijent
        coeff = (int)strtol(p, &p, 10);

        // preskoci sve sto nije cifra ni znak +/-
        while (*p != '\0' &&
               (*p < '0' || *p > '9') &&
               *p != '-' && *p != '+') {
            if (*p == '\n') {
                // ako bi tu naletjeli na novi red, to bi znacilo da
                // je polinom cudno formatiran, ali svejedno
                // cemo krenuti u drugi polinom
                line++;
                if (line == 1) {
                    current = second;
                }
            }
            p++;
        }

        if (*p == '\0') break;

        // citamo potenciju
        pow = (int)strtol(p, &p, 10);

        *current = add(*current, coeff, pow);
        if (!*current) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

/*
    add_poly:
    - umjesto merge algoritma:
      1) kopira sve clanove iz first u rezultat
      2) doda sve clanove iz second u rezultat
*/
Poly* add_poly(Poly* first, Poly* second, Poly** head) {
    Poly* result = NULL;
    Poly* temp = first;

    while (temp != NULL) {
        result = add(result, temp->coeff, temp->pow);
        if (!result) return NULL;
        temp = temp->next;
    }

    temp = second;
    while (temp != NULL) {
        result = add(result, temp->coeff, temp->pow);
        if (!result) return NULL;
        temp = temp->next;
    }

    *head = result;
    return *head;
}

/*
    mult_poly:
    - klasicno dvostruko mnozenje svakog clana s svakim
    - sve se ubacuje preko add() u rezultat
*/
Poly* mult_poly(Poly* first, Poly* second, Poly** head) {
    Poly* result = NULL;
    Poly* t1 = first;
    Poly* t2 = NULL;
    int coeff, pow;

    while (t1 != NULL) {
        t2 = second;
        while (t2 != NULL) {
            coeff = t1->coeff * t2->coeff;
            pow = t1->pow + t2->pow;

            result = add(result, coeff, pow);
            if (!result) {
                return NULL;
            }

            t2 = t2->next;
        }
        t1 = t1->next;
    }

    *head = result;
    return *head;
}