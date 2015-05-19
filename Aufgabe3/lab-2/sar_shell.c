#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMD_LENGTH 50
#define DIR_LENGTH 200

char input[CMD_LENGTH], prompt[DIR_LENGTH];

char *command, *cmd_args;
char has_args;

char *work_dir, *base_dir;


void cd_routine();
void wait_routine();
void prog_routine();


int main(void)
{
	// Variablen initialisieren, Speicher holen
	strcpy(prompt,".");
	
	command = malloc(sizeof(char)*CMD_LENGTH);
	has_args = 0;

	work_dir = malloc(sizeof(char)*DIR_LENGTH);
	base_dir = malloc(sizeof(char)*DIR_LENGTH);
	getcwd(base_dir,DIR_LENGTH);

	while (1) {
		// Kommando holen
		printf("%s> ", prompt);
		fgets(input, CMD_LENGTH, stdin); // Wozu der Umweg ueber input?
		command = input;
		command[strlen(command)-1] = '\0';

		// Kommando von eventuellen Parametern trennen
		if ((cmd_args = strchr(command, ' ')) != NULL) {
			// String hat Leerzeichen
			// Kommando und Parameter durch NUL-Zeichen trennen
			*cmd_args = '\0';
			cmd_args++; // Pointer auf Param-Beginn verschieben
			has_args = 1;
		} else {
			has_args = 0;
		}

		// Kommando behandeln
		
		if (!strcmp(command, "cd")) {
			cd_routine();
		}
		else if (!strcmp(command, "wait")) {
			wait_routine();
		}
		else if (!strcmp(command, "exit")) {
			exit(0);
		}
		else {
			prog_routine();
		}		
	}
}

void cd_routine() {
	if (has_args) {
		// Pfad wurde uebergeben
		if (!chdir(cmd_args)) {
			// erstmal wechseln und dann prompt zusammenbasteln
			getcwd(work_dir, DIR_LENGTH);

			if (!strcmp(base_dir, work_dir)) {
				// work_dir == base_dir
				strcpy(prompt, ".");
			} else {
				// ist work_dir ein unterverzeichnis von base_dir?
				if (!strncmp(base_dir, work_dir, strlen(base_dir))) {
					// ja, dann ersetze base_dir durch "."
					strcpy(prompt, ".");
					strcat(prompt, work_dir+strlen(base_dir));
				} else {
					strcpy(prompt, work_dir);
				}
			}
		} else {
			printf("Pfad wurde nicht gefunden!\n");
		}
	} else {
		// kein Pfad wurde uebergeben, also ins base_dir wechseln
		chdir(base_dir);
		strcpy(prompt, ".");
	}
}

void wait_routine() {
	// ...
}

void prog_routine() {
	// TODO: ggf & aus den params entnehmen und selbst behandeln
	// TODO: pipe behandeln

	pid_t pid = fork();
	if (pid == 0) {
		// wir sind der Child-Prozess
		printf("Child: %s\n", command);

		int ret;
		if (has_args) {
			ret = execl(command, command, cmd_args, (char *) 0);
		} else {
			ret = execl(command, command, (char *) 0);
		}
		exit(ret);
	} else if (pid < 0) {
		// fork failed
		printf("Fork failed");
		exit(1);
	} else {
		// wir sind der Parent-Prozess
		int status;
		waitpid(pid, &status, 0); // TODO: nur warten wenn kein & in params
		printf("Parent: %d\n", status);
	}
}