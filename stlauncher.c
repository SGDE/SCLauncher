#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

//	Variables
#define MAX_OPTIONS 100

typedef struct {
	char name[200];
	char command[300];
} Option;

Option options[MAX_OPTIONS];
Option reorg[MAX_OPTIONS];
char search[200];
unsigned short current_option = 0, total_options = 0, total_reorg = 0, nos = 1, quit = 0, x, y;

//	Colors
unsigned short get_color(char *value){
	if (strcmp(value, "BLACK") == 0) return 0;
	else if (strcmp(value, "RED") == 0) return 1;
	else if (strcmp(value, "GREEN") == 0) return 2;
	else if (strcmp(value, "YELLOW") == 0) return 3;
	else if (strcmp(value, "BLUE") == 0) return 4;
	else if (strcmp(value, "MAGENTA") == 0) return 5;
	else if (strcmp(value, "CYAN") == 0) return 6;
	else if (strcmp(value, "WHITE") == 0) return 7;
	else return 8;
}

void create_color(unsigned short pair, char *value1, char *value2) {
	unsigned short colors[2] = {get_color(value1), get_color(value2)};
	init_pair(pair, colors[0], colors[1]);
}

void on(unsigned short pair) {attron(COLOR_PAIR(pair));}
void off(unsigned short pair) {attroff(COLOR_PAIR(pair));}

// Configuration
void create_config(char *path){
	FILE *file = fopen(path, "w");
	fprintf(file, "SELECTED=BLUE,CYAN\n");
	fprintf(file, "UNSELECTED=WHITE,BLUE\n");
	fprintf(file, "SEARCH=BLUE,WHITE\n");
	fclose(file);
}

void read_config(char *home){
	char path[256];
	sprintf(path, "%s/.config/stlauncher.conf", home);
	FILE *file = fopen(path, "r");
	if (file != NULL) {
		char line[200], type[200], value1[200], value2[200];
		while (fgets(line, sizeof(line), file)){
			if (line[0] == '#') continue;
			if (sscanf(line, "%[^=]=%[^,],%s", type, value1, value2) == 3){
				//	Create colors
				if (strcmp(type, "SELECTED") == 0) create_color(1, value1, value2);
				else if (strcmp(type, "UNSELECTED") == 0) create_color(2, value1, value2);
				else if (strcmp(type, "SEARCH") == 0) create_color(3, value1, value2);
			}
		}
	}
	else {create_config(path); read_config(home);}
}

//	GUI
unsigned short verify_desktop(char *filename) {
	unsigned short filen_len = strlen(filename);

	if (strcmp(filename + filen_len - 8, ".desktop") == 0) return 1;
	else return 0;
}

void load_options() {
	DIR *directory;
	struct dirent *entry;
	char filepath[300], line[200], type[50], value[300], verify[100];

	directory = opendir("/usr/share/applications");
	if (directory != NULL) {
		while ((entry = readdir(directory)) != NULL && total_options < MAX_OPTIONS) {
			sprintf(verify, entry->d_name);
			if (strcmp(verify, "..") != 0 && strcmp(verify, ".") != 0 && verify_desktop(verify)) { 
				sprintf(filepath, "/usr/share/applications/%s", entry->d_name);
				FILE *desktop = fopen(filepath, "r");
				if (desktop != NULL) {
					while (fgets(line, sizeof(line), desktop) && total_options < MAX_OPTIONS) {
						if (line[0] == '#') continue;
						if (sscanf(line, "%[^=]=%[^\n]", type, value) == 2) {
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
		}
		closedir(directory);
	}
}

// Show options
void show_options() {
	if (strlen(search) == 0){
		nos = 0;
		for (unsigned short i = 0; (i < y - 3) && (i < total_options); ++i) {
			if (i == current_option){
				on(1);
				for (unsigned short xlimit = 1; xlimit != x - 1; xlimit++) mvaddch(i + 2, xlimit, ' ');
				mvprintw(i + 2, 3, "%s", options[i].name);
				off(1);
			}
			else {
				on(2);
				for (unsigned short xlimit = 1; xlimit != x - 1; xlimit++) mvaddch(i + 2, xlimit, ' ');
				mvprintw(i + 2, 3, "%s", options[i].name);
				off(2);
			}
		}
	}
	else {
		nos = 1;
		total_reorg = 0;
		char lower[200], lower_name[200];
		for (unsigned short i = 0; i != sizeof(lower); i++) {
			if (search[i] != '\0') lower[i] = search[i];
			else break;
		}
		
		for (unsigned short i = 0; i != total_options; ++i){
			for (unsigned short c = 0; c != 200; c++) lower_name[c] = options[i].name[c];
			
			if (strstr(lower_name, search) != NULL || strstr(options[i].command, search) != NULL) {
				sprintf(reorg[total_reorg].name, options[i].name);
				sprintf(reorg[total_reorg].command, options[i].command);
				total_reorg++;
			}
		}
		for (unsigned short i = 0; i != total_reorg; i++){
			if (i == current_option){
				on(1);
				for (unsigned short xlimit = 1; xlimit != x - 1; xlimit++) mvaddch(i + 2, xlimit, ' ');
				mvprintw(i + 2, 3, "%s", reorg[i].name);
				off(1);
			}
			else {
				on(2);
				for (unsigned short xlimit = 1; xlimit != x - 1; xlimit++) mvaddch(i + 2, xlimit, ' ');
				mvprintw(i + 2, 3, "%s", reorg[i].name);
				off(2);
			}
		}
	}
}

//	Show search bar
void show_entry() {
	on(3);
	attron(A_BOLD);
	for (unsigned short xlimit = 1; xlimit != x - 1; xlimit++) mvaddch(1, xlimit, ' ');
	mvprintw(1, 1, "Search: %s", search);
	off(3);
	attroff(A_BOLD);
}

//	Clear Screen
void clear_screen() {
	for (unsigned short posy = 0; posy != y; posy++)
		for (unsigned short posx = 0; posx != x; posx++) mvaddch(posy, posx, ' ');
	box(stdscr, 0, 0);
}

//	Key input
void get_keys(unsigned short key) {
	switch (key) {
		case 27: // Escape key
			quit = 1;
			break;
		case KEY_UP:
			if (current_option > 0)
				current_option--;
			break;
		case KEY_LEFT:
			if (current_option > 0)
				current_option--;
			break;
		case KEY_DOWN:
			if (current_option < total_options - 1 && current_option < y - 4 && !nos)
				current_option++;
			else if (current_option < total_reorg - 1 && current_option < y)
				current_option++;
			break;
		case KEY_RIGHT:
			if (current_option < total_options - 1 && current_option < y - 4 && !nos)
				current_option++;
			else if (current_option < total_reorg - 1 && current_option < y)
				current_option++;
			break;
		case 10:
			if (strlen(search) == 0) {
				execl("/bin/sh", "sh", "-c", options[current_option].command, (char *)0);
			}
			else {
				execl("/bin/sh", "sh", "-c", reorg[current_option].command, (char *)0);
			}
			quit = 1;
			break;
		default:
			// Search Entry
			if (key >= 32 && key <= 126 && strlen(search) < sizeof(search) - 1) {
				search[strlen(search)] = key;
				search[strlen(search) + 1] = '\0';
			}
			else if (key == 263 && strlen(search) > 0) {
				search[strlen(search) - 1] = '\0';
			}
			current_option = 0;
			break;
	}
}

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
