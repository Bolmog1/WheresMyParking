#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PARKING_CSV "parking-metropole.csv"

struct parking {
	char id[8];
	char nom[50];
	char adresse[50];
	char ville[25];
	char etat[10];
	int place_dispo;
	int capacite_max;
	char date_maj[26];
	char affichage[10];
};

FILE *load_parking_csv() {
	return fopen(PARKING_CSV, "r");
}

int lenght_of_file(FILE *fptr) {
	int size = 0;
	while ( ! feof( fptr ) ) {
        if (fgetc( fptr ) == '\n') {
        	size++;
    	}
    }
    return size;
}

void load_parking(FILE *fptr, struct parking *parkings, int nb_of_rows) {
	fseek(fptr, 0, SEEK_SET);
	while (fgetc(fptr) != '\n');
	for (int i = 0; i < nb_of_rows; ++i) {
		char ligne[250];
		fgets(ligne, 250, fptr);
		struct parking parking_temp;
		struct parking parking_traite;
		strcpy(parking_traite.id, strtok(ligne, ";"));
		strcpy(parking_traite.nom, strtok(NULL, ";"));
		strcpy(parking_traite.adresse, strtok(NULL, ";"));
		strcpy(parking_traite.ville, strtok(NULL, ";"));
		strcpy(parking_traite.etat, strtok(NULL, ";"));
		parking_traite.place_dispo = atoi(strtok(NULL, ";"));
		parking_traite.capacite_max = atoi(strtok(NULL, ";"));
		strcpy(parking_traite.date_maj, strtok(NULL, ";"));
		strcpy(parking_traite.affichage, strtok(NULL, ";"));
		parkings[i] = parking_traite;
	}
}

int main(int argc, char const *argv[]) {
	FILE *fptr = load_parking_csv();
	int nb_of_rows = lenght_of_file(fptr) - 1; // supprimer la premiere ligne d'en tete
	printf("%d\n", nb_of_rows);
	struct parking *parkings = malloc(sizeof(struct parking) * nb_of_rows);
	if (parkings == NULL) {
		perror("malloc");
		exit(1);
	}
	load_parking(fptr, parkings, nb_of_rows);
	printf("%s\n", parkings[0].nom);
	return 0;
}
