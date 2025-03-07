#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define PARKING_CSV "parking-metropole.csv"

#define WIN_WIDTH 40
#define WIN_HEIGHT 10

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

struct parking *lesparkings() {
	FILE *fptr = load_parking_csv();

	int nb_of_rows = lenght_of_file(fptr) - 1;
	
	struct parking *parkings = malloc(sizeof(struct parking) * nb_of_rows);

	if (parkings == NULL) {
		perror("malloc");
		exit(1);
	}

	load_parking(fptr, parkings, nb_of_rows);
	return parkings;
}

void draw_shadow(WINDOW *win, int starty, int startx) {
    int i;
    for (i = 1; i <= WIN_HEIGHT; i++) {
        mvprintw(starty + i, startx + WIN_WIDTH, " ");
    }
    for (i = 1; i <= WIN_WIDTH; i++) {
        mvprintw(starty + WIN_HEIGHT, startx + i, " ");
    }
}

void initncurses() {
    initscr();              // Initialiser ncurses
    start_color();          // Activer la gestion des couleurs
    use_default_colors();
    noecho();               // Désactiver l'affichage des entrées utilisateur
    curs_set(0);            // Cacher le curseur

    // Définition des couleurs
    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // Fond bleu
    init_pair(2, COLOR_WHITE, COLOR_BLACK);  // Ombre noire
    init_pair(3, COLOR_BLACK, COLOR_WHITE);  // Fenêtre grise
    init_pair(4, COLOR_RED, COLOR_WHITE);    // Fenêtre grise

    // Définition de l'arrière-plan bleu
    bkgd(COLOR_PAIR(1));
    clear();
    refresh();  // Ajout de refresh pour afficher les modifications
}

void affichecredit(int state) {
    // Définition de l'arrière-plan bleu
    bkgd(COLOR_PAIR(1));
    clear();

    // Coordonnées de la fenêtre
    int starty = (LINES - WIN_HEIGHT) / 2;
    int startx = (COLS - WIN_WIDTH) / 2;

    // Dessiner l'ombre
    attron(COLOR_PAIR(2));
    draw_shadow(NULL, starty, startx);
    attroff(COLOR_PAIR(2));

    // Créer la fenêtre principale (grise)
    WINDOW *win = newwin(WIN_HEIGHT, WIN_WIDTH, starty, startx);
    wbkgd(win, COLOR_PAIR(3));
    box(win, ACS_VLINE, ACS_HLINE);
    mvwprintw(win, 0, (WIN_WIDTH - 21) / 2, "Wheres My Parking");
    wattron(win, COLOR_PAIR(4));
    mvwprintw(win, 2, (WIN_WIDTH - 14) / 2, "Developper par");
    wattroff(win, COLOR_PAIR(4));
    wattron(win, A_BOLD);
    mvwprintw(win, 3, (WIN_WIDTH - 34) / 2, "Eliot MIDAVAINE & Simon DEFONTAINE");
    wattroff(win, A_BOLD);  // Ajout de attroff pour désactiver le mode gras
    if (state) {
    	mvwprintw(win, 5, (WIN_WIDTH - 16) / 2, "Fichier charge !");
    	wattron(win, A_BLINK);
		mvwprintw(win, 7, (WIN_WIDTH - 34) / 2, "Appuyez sur entrer pour continuer");
    	wattroff(win, A_BLINK);
    } else {
    	mvwprintw(win, 5, (WIN_WIDTH - 24) / 2, "Chargement du fichier...");
    }
    refresh();  // Ajout de refresh pour afficher les modifications
    wrefresh(win);
    delwin(win);
}


int main(int argc, char const *argv[]) {
	initncurses();
	affichecredit(0);
	
	struct parking *parkings = lesparkings();
	
	printf("test");
	printf("%s\n", parkings[0].nom);

	affichecredit(1);
	getch();
	clear();
	endwin();
	return 0;
}