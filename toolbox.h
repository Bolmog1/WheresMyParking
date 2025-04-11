void initncurses();

void cprint(WINDOW *win, int line, char text[]);

void lprint(WINDOW *win, int line, char *text, int selected);

void scanfGB(WINDOW *win_menu, int y, int x);