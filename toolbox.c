#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

void initncurses() {
    /*
    Initialise les paramètres de ncurses
    */
    initscr();              // initialiser ncurses
    start_color();          // gestion des couleurs
    use_default_colors();
    noecho();               // Désactiver l'affichage des entrées utilisateur
    curs_set(0);            // Cacher le curseur

    // Définition des couleurs
    init_pair(1, COLOR_WHITE, COLOR_BLUE);   // Fond bleu (BG)
    init_pair(2, COLOR_WHITE, COLOR_BLACK);  // Ombre noire (BG)
    init_pair(3, COLOR_BLACK, COLOR_WHITE);  // Fenetre Blanche (FG)
    init_pair(4, COLOR_RED, COLOR_WHITE);    // Texte rouge (FG)
    init_pair(5, COLOR_RED, COLOR_RED);      // Fond rouge (BG)

    // Définition de l'arrière-plan bleu
    bkgd(COLOR_PAIR(1));
    clear();
    refresh();  // refresh pour afficher les modifications
}

void cprint(WINDOW *win, int line, char text[]) {
    /*
    Affiche le texte 'text' centré dans sur la fenetre 'win' sur la ligne 'line'
    */
    mvwprintw(win, line, (getmaxx(win) - strlen(text)) / 2, text);
}

void lprint(WINDOW *win, int line, char *text, int selected) {
    /*
    Affiche un ligne 'line' du systeme de séléction sur la fenetre 'win' et avec le fond bleu si 'selected' est true
    */
    wattron(win, COLOR_PAIR(selected ? 1 : 3));  // Use window attributes with wattron
    // Clear the line with the right color
    for (int i = 2; i < getmaxx(win) - 2; ++i) {
        mvwprintw(win, line, i, " ");
    }

    mvwprintw(win, line, 3, "%s", text);
    wattroff(win, COLOR_PAIR(selected ? 1 : 3));
}

void scanfGB(WINDOW *win_menu, int y, int x) {
    /*
    Met un arriere plan noir pour les scanf
    */
    wattron(win_menu, COLOR_PAIR(2));
    for (int i = x; i < getmaxx(win_menu) - x; ++i) {
        mvwprintw(win_menu, y, i, " ");
    }
}