#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct{ // Definicija strukture student
    char ime[20];
    char prezime[20];
    float broj_bodova;
}student;

int Koliko_studenata() {
    int brojac_studenata = 0;
    char znak;
    FILE *ulaz = fopen("studenti.txt", "r");
    if (ulaz == NULL) {
        printf("Greška prilikom otvaranja datoteke.\n");
        return -1;
    }   // otvaranje datoteke i provjera jeli uspješno otvorena
    while ((znak = fgetc(ulaz)) != EOF) {  
        if (znak == '\n') {
            brojac_studenata++;
        }
    }   // čitamo datoteku znak po znak
    fclose(ulaz);
    return brojac_studenata; // Vraćamo broj studenata (broj redova)
}

student* Upisivanje_studenta(student *s, int broj_studenata, FILE *ulaz) {
    int i;
    for (i = 0; i < broj_studenata; i++) {
        fscanf(ulaz, "%s %s %f", s[i].ime, s[i].prezime, &s[i].broj_bodova);
    }
    fclose(ulaz);
    return s;

}

int ispisivanje_bodova(student *s, int broj_studenata) {
    int i;
    float max_broj_bodova = 80; // maksimalan broj bodova
    printf("\nIspis bodova studenata:\n");
    for (i = 0; i < broj_studenata; i++) {
        printf("Student %d: %s %s %.2f, %.2f\n", i + 1, s[i].ime, s[i].prezime, s[i].broj_bodova, (s[i].broj_bodova / max_broj_bodova) * 100 );
    }   //ispisvanje svakog studenta i njegovih aps i rel bodova, primjetit cemo da ima studenata sa 106% npr jer to ovisi o koliki nam je max broj bodova
    return 0;
}

int main() {
    int brojac_studenata = 0;
    
    brojac_studenata = Koliko_studenata();
    student *s = (student*)malloc(brojac_studenata * sizeof(student));  // dinamička alokacija memorije za strukturu student
    printf("Broj studenata: %d\n", brojac_studenata);    //ovom funkcijom smo prebrojali studente
    s = Upisivanje_studenta(s, brojac_studenata, fopen("studenti.txt", "r")); //pozivamo funkciju za upisivanje studenata
    ispisivanje_bodova(s, brojac_studenata); //ispisujemo bodove studenata


    free(s); // oslobađanje memorije studenta
    return 0;
}