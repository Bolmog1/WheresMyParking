#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#define PARKING_CSV "parking-metropole.csv"

#define PWD "FUck"

#define WIN_WIDTH_CREDIT 40
#define WIN_HEIGHT_CREDIT 10
#define WIN_WIDTH_MENU 80
#define WIN_HEIGHT_MENU 25
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
    attron(COLOR_PAIR(2));
    for (i = 1; i <= height; i++) {
        mvprintw(starty + i, startx + width, " ");
    }
    for (i = 1; i <= width; i++) {
        mvprintw(starty + height, startx + i, " ");
    }
    attroff(COLOR_PAIR(2));
    refresh();
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
    draw_shadow(starty, startx, WIN_WIDTH_CREDIT, WIN_HEIGHT_CREDIT);

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
    int win_height_menu = NB_MENU + 2;
    // Def de la fenetre
    WINDOW *win_menu = newwin(win_height_menu, WIN_WIDTH_MENU, starty, startx);
    wbkgd(win_menu, COLOR_PAIR(3));
    box(win_menu, ACS_VLINE, ACS_HLINE);
    cprint(win_menu, 0, " Menu ");
    cprint(win_menu, win_height_menu - 1, " (Q) Quitter | (^v) Déplacer | (Enter) Choissir ");
    draw_shadow(starty, startx, WIN_WIDTH_MENU, win_height_menu);


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
    cprint(win_menu, WIN_HEIGHT_MENU - 1, " (Q) Quitter | (^v) Déplacer | (Enter) Choissir ");
    draw_shadow(starty, startx, WIN_WIDTH_MENU, WIN_HEIGHT_MENU);

    int selected = 0;
    int offset = 0;  // Pour faire défiler la liste si elle est plus grande que la fenêtre
    int max_visible = WIN_HEIGHT_MENU - 2;  // Nombre maximum d'éléments visibles
    char text_buffer[80];
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

void afficheparking(int selected) {
    selected = selected >= 0 ? selected : afficheparkings();
    if (selected < 0) {return;}

    bkgd(COLOR_PAIR(1));
    clear();

    struct parking choose_one = parkings[selected];
    // Coordonnées de la fenêtre
    int starty = (LINES - WIN_HEIGHT_MENU) / 2;
    int startx = (COLS - WIN_WIDTH_MENU) / 2;

    // Def de la fenetre
    WINDOW *win_menu = newwin(WIN_HEIGHT_MENU, WIN_WIDTH_MENU, starty, startx);
    wbkgd(win_menu, COLOR_PAIR(3));
    box(win_menu, ACS_VLINE, ACS_HLINE);
    cprint(win_menu, 0, choose_one.nom);
    draw_shadow(starty, startx, WIN_WIDTH_MENU, WIN_HEIGHT_MENU);

    char buffer[80];

    sprintf(buffer, "ID : \t%s", choose_one.id);
    mvwprintw(win_menu, 1, 3, buffer);
    sprintf(buffer, "Nom : \t%s", choose_one.nom);
    mvwprintw(win_menu, 2, 3, buffer);
    sprintf(buffer, "Adresse : \t%s", choose_one.adresse);
    mvwprintw(win_menu, 3, 3, buffer);
    sprintf(buffer, "Ville : \t%s", choose_one.ville);
    mvwprintw(win_menu, 4, 3, buffer);
    sprintf(buffer, "Etat : \t%s", choose_one.etat);
    mvwprintw(win_menu, 5, 3, buffer);
    sprintf(buffer, "Place Disponible : \t%d / %d", choose_one.place_dispo, choose_one.capacite_max);
    mvwprintw(win_menu, 6, 3, buffer);
    sprintf(buffer, "Date derniere MàJ :  %s", choose_one.date_maj);
    mvwprintw(win_menu, 7, 3, buffer);

    wrefresh(win_menu);
    wgetch(win_menu);

    cprint(win_menu, 2, choose_one.nom);
}

void scanfGB(WINDOW *win_menu, int y, int x) {
    wattron(win_menu, COLOR_PAIR(2));
    for (int i = x; i < getmaxx(win_menu) - x; ++i) {
        mvwprintw(win_menu, y, i, " ");
    }
}

void choixparking() {
    // Définition de l'arrière-plan bleu
    bkgd(COLOR_PAIR(1));
    clear();
    refresh();

    // Coordonnées de la fenêtre
    int starty = (LINES - WIN_HEIGHT_MENU) / 2;
    int startx = (COLS - WIN_WIDTH_MENU) / 2;

    // Def de la fenetre
    WINDOW *win_menu = newwin(3, WIN_WIDTH_MENU, starty, startx);
    wbkgd(win_menu, COLOR_PAIR(3));
    box(win_menu, ACS_VLINE, ACS_HLINE);
    cprint(win_menu, 0, " Entrer un ID de parking ");
    draw_shadow(starty, startx, WIN_WIDTH_MENU, 3);

    keypad(win_menu, TRUE);  // Enable keyboard input for the window

    scanfGB(win_menu, 1, 2);

    wrefresh(win_menu);
    char buffer[8];
    echo();
    curs_set(1);
    mvwscanw(win_menu,1,2,"%s", buffer);
    noecho();
    curs_set(0);
    wattroff(win_menu, COLOR_PAIR(2));
    mvwprintw(win_menu, 0, 0, buffer);
    wrefresh(win_menu);
    for (int i = 0; i < NB_PARKINGS; i++) {
      if (!strcmp(parkings[i].id, buffer)) {
          afficheparking(i);
          break;
      }
    }
}

int modeAdministrateur() {
    // Définition de l'arrière-plan bleu
    bkgd(COLOR_PAIR(1));
    clear();
    refresh();

    // Coordonnées de la fenêtre
    int starty = (LINES - WIN_HEIGHT_MENU) / 2;
    int startx = (COLS - WIN_WIDTH_MENU) / 2;

    // Def de la fenetre
    WINDOW *win_menu = newwin(3, WIN_WIDTH_MENU, starty, startx);
    wbkgd(win_menu, COLOR_PAIR(3));
    box(win_menu, ACS_VLINE, ACS_HLINE);
    cprint(win_menu, 0, " Entrer le mdp admin ");
    draw_shadow(starty, startx, WIN_WIDTH_MENU, 3);

    keypad(win_menu, TRUE);  // Enable keyboard input for the window

    scanfGB(win_menu, 1, 2);
    echo();
    curs_set(1);
    char buffer_pwd[20];

    wrefresh(win_menu);

    mvwscanw(win_menu,1,2,"%20s", buffer_pwd);
    curs_set(0);
    noecho();
    if (strcmp(buffer_pwd, PWD)) {
        printf("\a"); 
        return 0;
    }

    int selected;
    char buffer[80];
    while (1) {
        selected = afficheparkings();
        if (selected == -1) {return 1;}

        bkgd(COLOR_PAIR(1));
        clear();

        struct parking choose_one = parkings[selected];
        // Coordonnées de la fenêtre
        starty = (LINES - WIN_HEIGHT_MENU) / 2;
        startx = (COLS - WIN_WIDTH_MENU) / 2;

        // Def de la fenetre
        win_menu = newwin(WIN_HEIGHT_MENU, WIN_WIDTH_MENU, starty, startx);
        wbkgd(win_menu, COLOR_PAIR(3));
        box(win_menu, ACS_VLINE, ACS_HLINE);
        cprint(win_menu, 0, choose_one.nom);
        draw_shadow(starty, startx, WIN_WIDTH_MENU, WIN_HEIGHT_MENU);

        sprintf(buffer, "ID : \t%s", choose_one.id);
        mvwprintw(win_menu, 1, 3, buffer);
        sprintf(buffer, "Nom : \t%s", choose_one.nom);
        mvwprintw(win_menu, 3, 3, buffer);
        sprintf(buffer, "Adresse : \t%s", choose_one.adresse);
        mvwprintw(win_menu, 5, 3, buffer);
        sprintf(buffer, "Ville : \t%s", choose_one.ville);
        mvwprintw(win_menu, 7, 3, buffer);
        sprintf(buffer, "Etat : \t%s", choose_one.etat);
        mvwprintw(win_menu, 9, 3, buffer);

        wrefresh(win_menu);

        wattron(win_menu, COLOR_PAIR(3));
        for (int i = 2; i < 12; i+=2) {
            scanfGB(win_menu, i, 3);
        }

        // %8[^\n] à la place de %s pour accepter les espaces
        mvwscanw(win_menu,2,3,"%8[^\n]", choose_one.id);
        if (choose_one.id[0] != '\0') {strcpy(parkings[selected].id, choose_one.id);}
        
        mvwscanw(win_menu,4,3,"%50[^\n]", choose_one.nom);
        if (choose_one.nom[0] != '\0') {strcpy(parkings[selected].nom, choose_one.nom);}

        mvwscanw(win_menu,6,3,"%50[^\n]", choose_one.adresse);
        if (choose_one.adresse[0] != '\0') {strcpy(parkings[selected].adresse, choose_one.adresse);}

        mvwscanw(win_menu,8,3,"%25[^\n]", choose_one.ville);
        if (choose_one.ville[0] != '\0') {strcpy(parkings[selected].ville, choose_one.ville);}

        mvwscanw(win_menu,10,3,"%10[^\n]", choose_one.etat);
        if (choose_one.etat[0] != '\0') {strcpy(parkings[selected].etat, choose_one.etat);}
        
        noecho();
        curs_set(0);
    }
    return 1;
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
                clear();
                endwin();
                return 0;
            case 0:
                afficheparking(-1);
                break;
            case 1:
                choixparking();
                break;
            case 2:
                // Entrer dans un parking
                break;
            case 3:
                modeAdministrateur();
                break;
        }
	}
	
	clear();
	endwin();
	return 0;
}