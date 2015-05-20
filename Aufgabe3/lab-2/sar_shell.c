#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CMD_LENGTH 50
#define DIR_LENGTH 200

char prompt[DIR_LENGTH];
char *work_dir, *base_dir;

char *command, **cmd_args;

char has_args;

pid_t known_pids[9];

void cd_routine();
void wait_routine();
void prog_routine();
char* tokenize(char *inputString, char** args);
pid_t search_pid(pid_t pid);

int main(void)
{
	// Variablen initialisieren, Speicher holen
	strcpy(prompt,".");
	
	command = malloc(sizeof(char)*CMD_LENGTH);
	cmd_args = malloc(sizeof(char*)*10);
	has_args = 0;

	work_dir = malloc(sizeof(char)*DIR_LENGTH);
	base_dir = malloc(sizeof(char)*DIR_LENGTH);
	getcwd(base_dir,DIR_LENGTH);

	while (1) {
		// Kommando holen
		printf("%s> ", prompt);
		fgets(command, CMD_LENGTH, stdin);
		command[strlen(command)-1] = '\0';

		command = tokenize(command,cmd_args);
		printf("[DEBUG_Joe] Command: %s\n", command);
		char** tmp = cmd_args;
		while (*cmd_args != NULL) {
			printf("[DEBUG_Joe] Argument: %s\n", *cmd_args++);
		}
		cmd_args = tmp;
/*
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
*/
		// Kommando behandeln
		if (!strcmp(command, "")) {
			continue;
		}
		else if (!strcmp(command, "cd")) {
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

/**
 * Sucht aus der Liste der known_pids die angefragte.
 *
 * return pid_t - wenn gefunden, 0 sonst
 */
pid_t search_pid(pid_t pid) {
	int i;
	for (i = 0; i < 8; i++) {
		if (pid == known_pids[i])
			return pid;
	}
	return 0;
}

/**
 * Zerlegt einen Leerzeichen getrennten Eingabestring in das Kommando und die Argumente.
 *
 * Parameter:
 * command - Eingabestring
 * args* - Pointer auf ein String-Array
 *
 * Output:
 * return - Pointer zum Command-String
 * args* - NULL-Terminierte Liste von Argumenten
 */
char* tokenize(char *inputString, char** args) {
	char* cmd;
	char* tokenp;
	if  (inputString == NULL || args == NULL) {
		return NULL;
	}
	if ((tokenp = strchr(inputString,' ')) != NULL) {	//Prüfen ob Leerzeichen im Eingabestring
		*tokenp = '\0'; 								//Wenn ja, Leerzeichen durch Stringterminierung ersetzen
		cmd = malloc(sizeof(char)*strlen(inputString));	//Speicher allozieren und
		strcpy(cmd, inputString);						//String reinschreiben
		inputString = tokenp+1;							//Im Inputstring an die nächste Stelle springen
		//Solange Leerzeichen vorhanden sind, an diesen Trennen und getrennt in das Array speichern
		while ((tokenp = strchr(inputString,' ')) != NULL) {
			*tokenp = '\0';
			*args = malloc(sizeof(char)*strlen(inputString));
			strcpy(*args, inputString);
			//Im Args Array eine Position weiter springen
			args++;
			inputString = tokenp+1;
		}
		//Argument nach dem Letzten Leerzeichen verarbeiten, wenn nicht leer.
		if (strlen(inputString) > 0) {
			*args = malloc(sizeof(char) * strlen(inputString));
			strcpy(*args, inputString);
			args++;
		}
	}
	// Wenn kein Leerzeichen vorhanden, dann einfach Commando Speichern
	else {
		cmd = malloc(sizeof(char)*strlen(inputString));
		strcpy(cmd, inputString);
	}
	// Args Array mit NULL terminieren
	*args = NULL;
	return cmd;
}

void cd_routine() {
	if (*cmd_args != NULL) {
		// Pfad wurde uebergeben
		if (!chdir(*cmd_args)) {
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
	// TODO: wait auf pids aus Argumenten
	// TODO: wait auf erste pid wenn kein Argument übergeben???
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