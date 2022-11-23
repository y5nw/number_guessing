#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <readline/readline.h>

struct leveldef {
	int min;
	int max;
	size_t nattempts;
};

typedef void (*entrycb)();
struct mentrydef {
	char *str;
	char *desc;
	entrycb cb;
};

static struct leveldef levels[] = {
	{1, 10, 8},
	{1, 12, 8},
	{1, 30, 15},
	{1, 30, 12},
	{1, 10, 3},
	{1, 50, 30},
	{0, 0, 0},
};

static void done() {
	exit(0);
}

static struct mentrydef mentries[] = {
	{"q", "Quit", done},
	{NULL, NULL, NULL},
};

int main(int argc, char **argv) {
	srand(time(NULL));
	size_t nlevels;
	for (nlevels = 0; levels[nlevels].nattempts; nlevels++);
	while(1) {
		size_t i;
		printf("There are %zd level(s). Enter the number of the level to start.\n", nlevels);
		for (i = 0; i < nlevels; i++) {
			struct leveldef l = levels[i];
			printf("[%zd] Range: (%d, %d); %zd attempt(s)\n", i+1, l.min, l.max, l.nattempts);
		}
		printf("Additionally, the following command(s) are available:\n");
		for (i = 0; mentries[i].cb; i++)
			printf("[%s] %s\n", mentries[i].str, mentries[i].desc);
		char *s = readline("Enter level or command: ");
		if (!s)
			break;
		char *p;
		size_t id = strtoul(s, &p, 10);
		if (*p) {
			struct mentrydef *q = mentries;
			while (q->str) {
				if (!strcmp(s, q->str)) {
					free(s);
					q->cb();
					break;
				}
				q++;
			}
			if (!q->str) {
				printf("Invalid command: %s\n", s);
				free(s);
			}
		} else if (id < 1 || id > nlevels) {
			free(s);
			printf("Level number out of range.\n");
		} else {
			free(s);
			id--;
			struct leveldef level = levels[id];
			int min = level.min, max = level.max;
			int val = rand()%(max-min)+min;
			size_t nattempts = level.nattempts;
			printf("You need to guess a number in the interval (%d, %d).\n", min, max);
			while (nattempts) {
				printf("You have %zd attempt(s) left.\n", nattempts);
				s = readline("Guess the number: ");
				if (!s) {
					printf("You gave up.\n");
					break;
				}
				int input = strtol(s, &p, 10);
				if (*p) {
					printf("Input is not a number.\n");
				} else if (input < min || input > max) {
					printf("Input out of range.\n");
				} else if (input == val) {
					printf("Your guess is correct.\n");
					nattempts = 0;
				} else {
					char *desc = input>val?"higher":"lower";
					printf("Your guess is %s than the actual value.\n", desc);
					nattempts--;
					if (!nattempts)  {
						printf("You lost.\n");
					}
				}
				free(s);
			}
		}
	}
	return 0;
}
