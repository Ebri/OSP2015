#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define DIR_LENGTH 150

char input[50], prompt[150];
char *command;
char *work_dir, *base_dir;

char* command_b;

int main(void)
{
	int tmp;
	strcpy(prompt,".");
	command = malloc(sizeof(char)*50);
	work_dir = malloc(sizeof(char)*DIR_LENGTH);
	base_dir = malloc(sizeof(char)*DIR_LENGTH);
	getcwd(base_dir,DIR_LENGTH);
	while (1) {
		printf("%s> ", prompt);
		fgets(input, 50,stdin);
		command = input;
		command[strlen(command)-1] = '\0';
		printf("Command: %s\n", command);
		if ((command_b = strchr(command, ' ')) != NULL) {
			//String mit Leerzeichen
			printf("String mit Leerzeichen\n");
			*command_b = '\0';
			printf("Command: %s\n", command);

			/*
			 * cd Routine
			 */
			if (!strcmp(command, "cd")) {
				command = command_b+1;
				if (!chdir(command)) {
					getcwd(work_dir, DIR_LENGTH);
					tmp = strcmp(base_dir, work_dir);
					if (tmp == 0)
						strcpy(prompt, ".");
					else if (tmp < 0){
						strcat(prompt, work_dir+strlen(base_dir));
					}
					else {
						strcpy(prompt, work_dir);
					}
				}
				else
					printf("Pfad nicht gefunden\n");
			}

			/*
			 * Wait Rountine
			 */
		}
		// Kein Leerzeichen
		if (!strcmp(command, "exit")) {
			exit(0);
		}

	}
}
