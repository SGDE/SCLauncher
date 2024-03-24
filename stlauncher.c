#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <dirent.h>

#define MAX_OPTIONS 100


typedef struct {
	char name[200];
	char command[300];
} Option;


Option options[MAX_OPTIONS];
char search[200];
unsigned short current_option = 0, total_options = 0;
int quit = 0;


void load_options();
void show_options();
void show_entry();
void clear_screen();
void get_keys(int key);


// load entries
void load_options() {
	DIR *directory;
	struct dirent *entry;
	char filepath[300], line[200], type[50], value[300];

	directory = opendir("/usr/share/applications");
	if (directory != NULL) {
		while ((entry = readdir(directory)) != NULL && total_options < MAX_OPTIONS) {
			sprintf(filepath, "/usr/share/applications/%s", entry->d_name);
			FILE *desktop = fopen(filepath, "r");
			if (desktop != NULL) {
				while (fgets(line, sizeof(line), desktop) && total_options < MAX_OPTIONS) {
					if (line[0] == '#') continue;
					if (sscanf(line, "%[^=]=%s", type, value) == 2) {
						if (strcmp(type, "Name") == 0)
							strcpy(options[total_options].name, value);
						else if (strcmp(type, "Exec") == 0)
							strcpy(options[total_options].command, value);
					}
				}
				fclose(desktop);
				total_options++;
			}
		}
		closedir(directory);
	}
}

// show options
void show_options() {
	for (unsigned short i = 0; i < total_options; ++i) {
		if (i == current_option)
			attron(A_REVERSE);
		printw("%s\n", options[i].name);
		attroff(A_REVERSE);
	}
}

// Show search bar
void show_entry() {
	mvprintw(0, 0, "Search: %s", search);
}

// clean screen?
void clear_screen() {
	clear();
}

// Key input
void get_keys(int key) {
	switch (key) {
		case 27: // Escape key
			quit = 1;
			break;
		case KEY_UP:
			if (current_option > 0)
				current_option--;
			break;
		case KEY_DOWN:
			if (current_option < total_options - 1)
				current_option++;
			break;
		default:
			// Search Entry
			if (key >= 32 && key <= 126 && strlen(search) < sizeof(search) - 1) {
				search[strlen(search)] = key;
				search[strlen(search) + 1] = '\0';
			} else if (key == 8 && strlen(search) > 0) {
				search[strlen(search) - 1] = '\0';
			}
			break;
	}
}



// Main
int main() {
	// Init
	initscr();
	curs_set(FALSE);
	noecho();
	start_color();
	keypad(stdscr, TRUE); // fix keyboard

	load_options();

	int key;
	while (!quit) {
		clear_screen();
		show_entry();
		show_options();
		refresh();
		key = getch();
		get_keys(key);
	}

	// end ncurses
	endwin();
	return 0;
}