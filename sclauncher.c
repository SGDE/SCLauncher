#include "sclauncher.h"

// Main
int main() {
	// Init
	initscr();
	curs_set(FALSE);
	noecho();
	keypad(stdscr, TRUE);

	start_color();
	read_config(getenv("HOME"));
	load_options();

	unsigned short key;

	while (!quit) {
		getmaxyx(stdscr, y, x);
		clear_screen();
		show_entry();
		show_options();
		refresh();
		key = getch();
		get_keys(key);
	}

	endwin();
	return 0;
}
