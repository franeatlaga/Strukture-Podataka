#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Person* PersonPtr;

typedef struct Person{
    char name[50];
    char surname[50];
    int birthYear;
    PersonPtr next;
} Person;


int AddPersonToBeggining(PersonPtr head);
int AddPersonToEnd(PersonPtr head);
int PrintList(PersonPtr head);
int DeleteFromList(PersonPtr head);
int FindAPerson(PersonPtr head, char *surname);


int main (){
    
    Person head={ .next = NULL, .name = "", .surname = "", .birthYear = 0};
    printf("pick a choice:\n1.Add person to beggining\n2.Add person to end\n3.Print list\n4.Delete from list\n5.Find a person by surname\n0.Exit\n");
    int choice;
    do{
        printf("Choice: ");
        scanf("%d", &choice);
        
        switch(choice){
            case 1: { if (AddPersonToBeggining(&head)==0)
            break;
        else
    printf("something went wrong\n");
            break;
}
            case 2: { if (AddPersonToEnd(&head)==0)
            break;
        else
    printf("something went wrong\n");
            break;
}
            case 3: { if (PrintList(&head)==0)
            break;
        else
    printf("something went wrong\n");
            break;
}
            case 4: { if (DeleteFromList(&head)==0)
            break;
        else
    printf("something went wrong\n");
            break;
}
            case 5: {
                char surname[50];
            
                printf("Enter surname to find: ");
                scanf("%s", surname);
               FindAPerson(&head, surname);  
                break;
            }
            case 0: break;
            default: printf("Invalid choice. Try again.\n"); break;
        }
        
    }while(choice != 0);


   while (head.next != NULL){
       PersonPtr temp = head.next;
       head.next = head.next->next;
       free (temp);
    }
    return 0;
}

int AddPersonToBeggining(PersonPtr head){
    PersonPtr newPerson = malloc(sizeof(Person));
    if (newPerson == NULL){
        return -1;
    }
    printf("Enter name: ");
    scanf("%s", newPerson->name);
    printf("Enter surname: ");
    scanf("%s", newPerson->surname);
    printf("Enter birth year: ");
    scanf("%d", &newPerson->birthYear);
    newPerson->next = NULL;
    newPerson->next = head->next;
    head->next = newPerson; 
    printf("Person added successfully\n");

    return 0;
}

int PrintList(PersonPtr head){
    PersonPtr current = head->next;
    while (current != NULL){
        printf("Name: %s, Surname: %s, Birth Year: %d\n", current->name, current->surname, current->birthYear);
        current = current->next;
    }
    return 0;
}

int DeleteFromList(PersonPtr head){
    printf("which person on the list do you want to delete (surname): ");
    char surname_temp[50];
    scanf("%s", surname_temp);
    PersonPtr current = head;
    PersonPtr previous = NULL;
    while (current != NULL){
        if (strcmp(current->surname, surname_temp) == 0){
            if (previous == NULL){
                head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            printf("Person deleted successfully\n");
            return 0;
        }
        previous = current;
        current = current->next;
    }
    printf("Person not found\n");
    return -1;

}

int FindAPerson(PersonPtr head, char surname[]){
    PersonPtr current = head;
    while (current != NULL){
        if (strcmp(current->surname, surname) == 0){
            printf("Person found: %s %s, Birth Year: %d\n", current->name, current->surname, current->birthYear);
            return 0;
        }
        current = current->next;
    }
    printf("Person not found\n");
    return -1;
}

int AddPersonToEnd(PersonPtr head){
    PersonPtr newPerson = malloc(sizeof(Person));
    if (newPerson == NULL){
        return -1;
    }
    printf("Enter name: ");
    scanf("%s", newPerson->name);
    printf("Enter surname: ");
    scanf("%s", newPerson->surname);
    
    printf("Enter birth year: ");
    scanf("%d", &newPerson->birthYear);
    newPerson->next = NULL;
    PersonPtr current = head;
    while (current->next != NULL){
        current = current->next;    
    }
    current->next = newPerson;
    return 0;
}
