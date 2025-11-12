#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define FILEPATH "Vjezba5/izraz.txt"
#define MAX 100

typedef struct Poly {
	int coeff, pow;
	struct Poly* next;
}Poly;

Poly* sort(Poly*);
Poly* add_poly(Poly*, Poly*, Poly**);
Poly* mult_poly(Poly*, Poly*, Poly**);
Poly* create(int, int);
Poly* add(Poly*, int, int);
char* read_file();
int read_buffer(Poly**, Poly**, char*);
void printpoly(Poly*);
void freepoly(Poly*);

int main() {
	Poly* heada = NULL;
	Poly* headm = NULL;
	Poly* second = NULL;
	Poly* first = NULL;
	int statea = 0, statem = 0;
	char* buffer = read_file();
	if (buffer == NULL) {
		return EXIT_FAILURE;
	}
	int choice;
	if ((read_buffer(&first, &second, buffer)) == 1) {
		printf("Greska u dodavanju u current!\n");
		return EXIT_FAILURE;
	}
	free(buffer);
	do {
		printf("1 - Zbrajanje\n"); printf("2 - Mnozenje\n"); printf("0 - Izlaz\n"); scanf("%d", &choice);
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
	int j = 0;
	FILE* f1 = fopen(FILEPATH, "r");
	if (!f1) {
		printf("Greska u otvaranju filea!\n");
		return NULL;
	}
	char* buffer = (char*)malloc(MAX * sizeof(char));
	char* real_buffer = (char*)malloc(MAX * sizeof(char));
	if (!buffer || !real_buffer) {
		printf("Greska u alokaciji! (read_file)\n");
		return NULL;
	}

	while (fgets(buffer, MAX, f1)) {
		for (int i = 0; buffer[i] != '\0'; i++) {
			if ((buffer[i] < '0' || buffer[i] > '9')) {
				continue;
			}
			real_buffer[j++] = buffer[i];
		}
		real_buffer[j++] = ' ';
	}
	real_buffer[j] = '\0';
	free(buffer);
	fclose(f1);
	return real_buffer;
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

	while (temp != NULL) {
		printf("%dx^%d ", temp->coeff, temp->pow);
		if (temp->next != NULL) {
			printf("+ ");
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

int read_buffer(Poly** first, Poly** second, char* buffer) {
	int i = 0;
	int coeff, pow;
	Poly** current = first;

	while (buffer[i] != ' ' && buffer[i] != '\0') {
		coeff = buffer[i++] - '0';
		if (buffer[i] == ' ' || buffer[i] == '\0') break;
		pow = buffer[i++] - '0';
		*current = add(*current, coeff, pow);
		if (!*current) {
			return EXIT_FAILURE;
		}
	}

	if (buffer[i] == ' ') i++;
	current = second;

	while (buffer[i] != '\0') {
		coeff = buffer[i++] - '0';
		if (buffer[i] == '\0') break;
		pow = buffer[i++] - '0';
		*current = add(*current, coeff, pow);
		if (!*current) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

Poly* add_poly(Poly* first, Poly* second, Poly** head) {
	Poly* temp1 = first;
	Poly* temp2 = second;
	int coeff, pow;

	while (temp1 != NULL && temp2 != NULL) {
		if (temp1->pow == temp2->pow) {
			coeff = temp1->coeff + temp2->coeff;
			pow = temp1->pow;
			*head = add(*head, coeff, pow);
			temp1 = temp1->next;
			temp2 = temp2->next;
		}
		else if (temp1->pow > temp2->pow) {
			*head = add(*head, temp1->coeff, temp1->pow);
			temp1 = temp1->next;
		}
		else {
			*head = add(*head, temp2->coeff, temp2->pow);
			temp2 = temp2->next;
		}
	}

	while (temp1 != NULL) {
		*head = add(*head, temp1->coeff, temp1->pow);
		temp1 = temp1->next;
	}
	while (temp2 != NULL) {
		*head = add(*head, temp2->coeff, temp2->pow);
		temp2 = temp2->next;
	}

	return *head;
}

Poly* mult_poly(Poly* first, Poly* second, Poly** head) {
	Poly* temp1 = first; Poly* temp2 = second;
	int coeff, pow, found = 0;

	while (temp1 != NULL) {
		temp2 = second;
		while (temp2 != NULL) {
			coeff = temp1->coeff * temp2->coeff;
			pow = temp1->pow + temp2->pow;
			*head = add(*head, coeff, pow);
			if (!*head) {
				return NULL;
			}
			temp2 = temp2->next;
		}
		temp1 = temp1->next;
	}

	return *head;
}