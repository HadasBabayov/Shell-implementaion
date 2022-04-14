// Hadas Bybabayov 322807629

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// Before use "cd", we save the current path.
static char lastPath[101];

/**
 * Command's information.
 */
typedef struct {
    char str[100];
    int pid;
    int isBuiltIn;
} command;

/**
 * This function gets an array of characters and splits it into strings (by spaces).
 *
 * @param str - the command.
 * @param numOfStrings - number of words in this command.
 * @return Array of strings
 */
char **stringsArray(char str[100], int *numOfStrings) {
    // Initialize array of strings.
    char **returnValue = NULL;
    // Split the input by spaces.
    char *one = strtok(str, " ");
    while (one != NULL) {
        returnValue = realloc(returnValue, sizeof(char *) * ++(*numOfStrings));
        if (returnValue == NULL) {
            // Memory allocated failed.
            exit(-1);
        }
        returnValue[*numOfStrings - 1] = one;
        one = strtok(NULL, " ");
    }
    // In the end wwe put NULL.
    returnValue = realloc(returnValue, sizeof(char *) * (*numOfStrings + 1));
    returnValue[*numOfStrings] = 0;
    return returnValue;
}

/**
 * The function gets the array and print only the commands with status "run".
 *
 * @param historyCommands - structs array.
 * @param size - number of commands in this array.
 */
void jobs(command *historyCommands, int size) {
    int i;
    for (i = 0; i < size; ++i) {
        if (!historyCommands[i].isBuiltIn) {
            // Remove the '&' from commands that aren't built in.
            int len = (int) strlen(historyCommands[i].str);
            if (historyCommands[i].str[len - 1] == '&') {
                historyCommands[i].str[len - 2] = '\0';
            }
        }
    }
    // Print only the commands with status "run".
    for (i = 0; i < size; ++i) {
        if (!historyCommands[i].pid || !waitpid(historyCommands[i].pid, NULL, WNOHANG)) {
            printf("%s\n", historyCommands[i].str);
        }
    }
}

/**
 * The function gets the array and print all the history commands with their status.
 *
 * @param historyCommands - structs array.
 * @param size - number of commands in this array.
 */
void history(command *historyCommands, int size) {
    int i;
    for (i = 0; i < size; ++i) {
        if (!historyCommands[i].isBuiltIn) {
            // Remove the '&' from commands that aren't built in.
            int len = (int) strlen(historyCommands[i].str);
            if (historyCommands[i].str[len - 1] == '&') {
                historyCommands[i].str[len - 2] = '\0';
            }
        }
    }

    // Print all the history commands with their status.
    for (i = 0; i < size; ++i) {
        // Command
        printf("%s ", historyCommands[i].str);
        // Status
        if (!historyCommands[i].pid || !waitpid(historyCommands[i].pid, NULL, WNOHANG)) {
            printf("RUNNING\n");
            continue;
        } else if (historyCommands[i].pid) {
            printf("DONE\n");
            continue;
        }
        printf("DONE\n");
    }
}

/**
 * This function implement the command "cd" according the flag.
 *
 * @param str - command
 */
void cd(char str[100]) {
    int numOfStrings = 0;
    // The command split by spaces into splitStr.
    char **splitStr = stringsArray(str, &numOfStrings);
    // More than 2 strings --> invalid.
    if (numOfStrings > 2) {
        printf("Too many arguments\n");
        return;
    } else if (numOfStrings == 1 || !strcmp(splitStr[1], "~") || !strcmp(splitStr[1],"--")) {
        // Back to home page.
        getcwd(lastPath, sizeof(lastPath));
        if (chdir(getenv("HOME")) != 0) {
            printf("chdir failed\n");
        }
    } else if (!strcmp(splitStr[1], "..")) {
        // Back one folder.
        getcwd(lastPath, sizeof(lastPath));
        if (chdir("..") != 0) {
            printf("chdir failed\n");
        }
    } else if (splitStr[1][0] == '~' && splitStr[1][1] != 0) {
        // Back to home page and then to the folder requested.
        getcwd(lastPath, sizeof(lastPath));
        // Save path without '~'.
        splitStr[1] = splitStr[1] + 1;
        int size = (int) strlen(splitStr[1]);
        char path[100];
        strcpy(path, strncat(getenv("HOME"), splitStr[1], size));
        if (chdir(path) != 0) {
            printf("chdir failed\n");
        }
    } else if (!strcmp(splitStr[1], "-")) {
        // Back to last path.
        char temp[101];
        getcwd(temp, sizeof(temp));
        if (chdir(lastPath) != 0) {
            printf("chdir failed\n");
        }
        strcpy(lastPath, temp);
    } else {
        // After '-' can't be another character.
        if (splitStr[1][0] == '-' && splitStr[1][1] != 0) {
            printf("An error occurred\n");
            return;
        }
        // Go to folder that we get after "cd".
        getcwd(lastPath, sizeof(lastPath));
        if (chdir(splitStr[1]) != 0) {
            printf("chdir failed\n");
        }
    }
}

/**
 * This function gets a command and count the quotes.
 *
 * @param str - command
 * @return How many quotes are in this command.
 */
int numOfQuotationMarks(char str[100]) {
    int counter = 0, i;
    for (i = 0; i < strlen(str); ++i) {
        if (str[i] == '"') {
            ++counter;
        }
    }
    return counter;
}

/**
 * This function gets a command and print the input without quotes.
 * Only if the number of quotes is even.
 *
 * @param str - command
 */
void echo(char str[100]) {
    // If the number of quotes is odd - return
    int counter = numOfQuotationMarks(str);
    if (counter % 2) {
        return;
    }

    // Remove "echo " from the command.
    int i;
    for (i = 5; i < strlen(str); ++i) {
        str[i - 5] = str[i];
    }
    str[i - 5] = '\0';


    char echoToPrint[100];
    i = 0;
    int index = 0;
    while (i < strlen(str)) {
        // Copy only characters that are not equal to "
        if (str[i] != '"') {
            echoToPrint[index] = str[i];
            index++;
        }
        i++;
    }
    echoToPrint[index] = '\0';
    // Print the result.
    printf("%s\n", echoToPrint);
}

/**
 * This function gets a command and do fork() and wait() (if we don't hava a '&').
 *
 * @param str - command.
 * @return pid of this command.
 */
int notBuiltIn(char str[100]) {
    int numOfStrings = 0;
    char **splitStr = stringsArray(str, &numOfStrings);

    // Initialize flag and remove the '&' if there is.
    int isBackground = 0;
    if (!strcmp(splitStr[numOfStrings - 1], "&")) {
        splitStr[numOfStrings - 1] = NULL;
        isBackground = 1;
    }

    int pid = fork();
    if (pid < 0) {
        printf("fork failed\n");
    }
    // Call exec
    if (pid == 0) {
        int check = execvp(splitStr[0], splitStr);
        if (check < 0) {
            printf("exec failed\n");
            exit(-1);
        }
    }
    // Call wait
    if (pid > 0) {
        if (!isBackground) {
            waitpid(pid, NULL, 0);
            
        }
    }
    free(splitStr);
    return pid;
}

/**
 * This program implement a Linux shell, that get a commands from thr user and responds accordingly.
 *
 * @return 0 if we gets exit.
 */
int main() {
    // We save here all the user commands.
    command historyCommands[100];
    // Count the commands.
    int counter = 0;
    while (1) {
        printf("$ ");
        fflush(stdout);
        char str[100];
        // Get a command.
        scanf("%100[^\n]%*c", str);

        // Change the pid of "history" if there isn't last command.
        if (counter != 0 && historyCommands[counter - 1].pid == 0) {
            if (!strcmp(historyCommands[counter - 1].str, "history")) {
                // If this history command isn't the last one --> DONE (pid = 0 means- still running).
                historyCommands[counter - 1].pid = 1;
            }
        }
        // Save the data in the commands array.
        strcpy(historyCommands[counter].str, str);
        historyCommands[counter].isBuiltIn = 1;

        // Built-in
        if (str[0] == 'e' && str[1] == 'x' && str[2] == 'i' && str[3] == 't' && str[4] == 0) {
            return 0;
        } else if (str[0] == 'j' && str[1] == 'o' && str[2] == 'b' && str[3] == 's' && str[4] == 0) {
            historyCommands[counter].pid = 1;
            jobs(historyCommands, counter + 1);
        } else if (str[0] == 'h' && str[1] == 'i' && str[2] == 's' && str[3] == 't' &&
                   str[4] == 'o' && str[5] == 'r' && str[6] == 'y' && str[7] == 0) {
            historyCommands[counter].pid = 0;
            history(historyCommands, counter + 1);
        } else if (str[0] == 'c' && str[1] == 'd') {
            historyCommands[counter].pid = 1;
            cd(str);
        } else if (str[0] == 'e' && str[1] == 'c' && str[2] == 'h' && str[3] == 'o' && str[4] == ' ') {
            historyCommands[counter].pid = 1;
            echo(str);
        } else {
            // ~Built-in
            historyCommands[counter].isBuiltIn = 0;
            historyCommands[counter].pid = notBuiltIn(str);
        }
        // Increase the counter --> get the next command.
        counter++;
    }
}


