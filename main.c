#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define PARKING_CSV "parking-metropole.csv"

#define WIN_WIDTH_CREDIT 40
#define WIN_HEIGHT_CREDIT 10
#define WIN_WIDTH_MENU 60
#define WIN_HEIGHT_MENU 15
#define NB_ELEMENT_MENU 10

char *MENU[] = {"Afficher tous les parkings", "Afficher un parking", "Entrer dans un parking", "Mode Administrateur", "Quitter (Q)"};
#define NB_MENU 5

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

struct parking *parkings;
int NB_PARKINGS;

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
    NB_PARKINGS = nb_of_rows;
	
	struct parking *parkings = malloc(sizeof(struct parking) * nb_of_rows);

	if (parkings == NULL) {
		perror("malloc");
		exit(1);
	}

	load_parking(fptr, parkings, nb_of_rows);
	return parkings;
}

void draw_shadow(int starty, int startx, int width, int height) {
    int i;
    for (i = 1; i <= height; i++) {
        mvprintw(starty + i, startx + width, " ");
    }
    for (i = 1; i <= width; i++) {
        mvprintw(starty + height, startx + i, " ");
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

void cprint(WINDOW *win, int line, char text[]) {
	mvwprintw(win, line, (getmaxx(win) - strlen(text)) / 2, text);
}

void affichecredit(int state) {
    // Définition de l'arrière-plan bleu
    bkgd(COLOR_PAIR(1));
    clear();

    // Coordonnées de la fenêtre
    int starty = (LINES - WIN_HEIGHT_CREDIT) / 2;
    int startx = (COLS - WIN_WIDTH_CREDIT) / 2;

    // Dessiner l'ombre
    attron(COLOR_PAIR(2));
    draw_shadow(starty, startx, WIN_WIDTH_CREDIT, WIN_HEIGHT_CREDIT);
    attroff(COLOR_PAIR(2));

    // Créer la fenêtre principale (grise)
    WINDOW *win = newwin(WIN_HEIGHT_CREDIT, WIN_WIDTH_CREDIT, starty, startx);
    wbkgd(win, COLOR_PAIR(3));
    box(win, ACS_VLINE, ACS_HLINE);
    cprint(win, 0, " Wheres My Parking ");
    wattron(win, COLOR_PAIR(4));
    cprint(win, 2, "Developper par");
    wattroff(win, COLOR_PAIR(4));
    wattron(win, A_BOLD);
    cprint(win, 3, "Eliot MIDAVAINE & Simon DEFONTAINE");
    wattroff(win, A_BOLD);  // Ajout de attroff pour désactiver le mode gras
    if (state) {
    	cprint(win, 5, "Fichier charge !");
    	wattron(win, A_BLINK);
		cprint(win, 7, "Appuyez sur entrer pour continuer");
    	wattroff(win, A_BLINK);
    } else {
    	cprint(win, 5, "Chargement du fichier...");
    }
    refresh();  // Ajout de refresh pour afficher les modifications
    wrefresh(win);
    delwin(win);
}

void lprint(WINDOW *win, int line, char *text, int selected) {
    wattron(win, COLOR_PAIR(selected ? 1 : 3));  // Use window attributes with wattron
    // Clear the line with the right color
    for (int i = 2; i < getmaxx(win) - 2; ++i) {
        mvwprintw(win, line, i, " ");
    }

    mvwprintw(win, line, 3, "%s", text);
    wattroff(win, COLOR_PAIR(selected ? 1 : 3));
}

int affichemenu() {
    // Définition de l'arrière-plan bleu
    bkgd(COLOR_PAIR(1));
    clear();

    // Coordonnées de la fenêtre
    int starty = (LINES - WIN_HEIGHT_MENU) / 2;
    int startx = (COLS - WIN_WIDTH_MENU) / 2;

    // Def de la fenetre
    WINDOW *win_menu = newwin(WIN_HEIGHT_MENU, WIN_WIDTH_MENU, starty, startx);
    wbkgd(win_menu, COLOR_PAIR(3));
    box(win_menu, ACS_VLINE, ACS_HLINE);
    cprint(win_menu, 0, " Menu ");

    int selected = 0;
    int ch;
    keypad(win_menu, TRUE);  // Enable keyboard input for the window

    // Menu loop
    while (1) {
        for (int i = 0; i < NB_MENU; ++i) {
            lprint(win_menu, i + 1, MENU[i], selected == i);
        }
        
        wrefresh(win_menu);
        ch = wgetch(win_menu);
        
        // Handle keyboard input
        switch(ch) {
            case KEY_UP:
                if (selected > 0)
                    selected--;
                break;
            case KEY_DOWN:
                if (selected < NB_MENU - 1)
                    selected++;
                break;
            case 10:  // Enter key
                delwin(win_menu);
                return selected;
            case 'q':  // Quit
                delwin(win_menu);
                return -1;
        }
    }
}

int afficheparkings() {
    // Définition de l'arrière-plan bleu
    bkgd(COLOR_PAIR(1));
    clear();

    // Coordonnées de la fenêtre
    int starty = (LINES - WIN_HEIGHT_MENU) / 2;
    int startx = (COLS - WIN_WIDTH_MENU) / 2;

    // Def de la fenetre
    WINDOW *win_menu = newwin(WIN_HEIGHT_MENU, WIN_WIDTH_MENU, starty, startx);
    wbkgd(win_menu, COLOR_PAIR(3));
    box(win_menu, ACS_VLINE, ACS_HLINE);
    cprint(win_menu, 0, " Tous les parkings ");

    int selected = 0;
    int offset = 0;  // Pour faire défiler la liste si elle est plus grande que la fenêtre
    int max_visible = WIN_HEIGHT_MENU - 2;  // Nombre maximum d'éléments visibles
    char text_buffer[WIN_WIDTH_MENU];
    int ch;
    keypad(win_menu, TRUE);  // Enable keyboard input for the window

    // Menu loop
    while (1) {
        // Effacer tout l'affichage d'abord
        for (int i = 0; i < max_visible; ++i) {
            lprint(win_menu, i + 1, "", 0);
        }
        
        // Afficher les éléments visibles avec offset
        for (int i = 0; i < max_visible && i + offset < NB_PARKINGS; ++i) {
            sprintf(text_buffer, "%s (%s)", 
                    parkings[i + offset].nom, 
                    parkings[i + offset].ville);
            lprint(win_menu, i + 1, text_buffer, selected == i);
        }
        
        wrefresh(win_menu);
        ch = wgetch(win_menu);
        
        // Handle keyboard input
        switch(ch) {
            case KEY_UP:
                if (selected > 0) {
                    selected--;
                } else if (offset > 0) {
                    offset--;
                }
                break;
            case KEY_DOWN:
                if (selected < max_visible - 1 && selected + offset < NB_PARKINGS - 1) {
                    selected++;
                } else if (offset + max_visible < NB_PARKINGS) {
                    offset++;
                }
                break;
            case 10:  // Enter key
                delwin(win_menu);
                return selected + offset;
            case 'q':  // Quit
                delwin(win_menu);
                return -1;
        }
    }
}

int main(int argc, char const *argv[]) {
	initncurses();
	affichecredit(0);
	
	parkings = lesparkings();

	affichecredit(1);
	getch();
	int user_input;
	while (1) {
		user_input = affichemenu();
        switch (user_input) {
            case -1:
            case 4:
                return 0;
            case 0:
                afficheparkings();
                break;
            case 1:
                // Afficher un parking
                break;
            case 2:
                // Entrer dans un parking
                break;
            case 3:
                // Mode admin
                break;
        }
	}
	
	clear();
	endwin();
	return 0;
}