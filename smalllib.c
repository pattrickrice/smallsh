#include "smalllib.h"

/* global variables used in signal handlers */
pid_t spawnpid = -5;
enum bool BACKGROUND_ALLOWED = TRUE;

/*********************************************************************
 *  Name: get_user_input
 *  Description: Prompts the user to input characters.
 *  Parameters: n/a
 *  Returns: User input.
*********************************************************************/
char *get_user_input() {
    char *buffer = malloc(2049 * sizeof(char));
    char *line = malloc(2049 * sizeof(char));
    memset(line, '\0', sizeof(strlen(line)));
    enum bool buffer_has_content = TRUE;

    /* we call fflush per spec of the design requirements. */
    fflush(stdout);
    printf(": ");
    do {
        fgets(buffer, 8 * sizeof(char), stdin);
        /* longer streams were getting clipped by fgets. Couldn't
         * figure out how to solve it other than just reading until
         * stdin was empty.*/
        strcat(line, buffer);

        /* fgets returns new line character which we
         * need to trim off */

        if (line[0] == 0) {
            buffer_has_content = FALSE;
        } else if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
            buffer_has_content = FALSE;
        }
    } while (buffer_has_content);
    free(buffer);
    return line;
}


/*********************************************************************
 *  Name: parse_args
 *  Description: Reads through user input and separates arguments by
 *  spaces.
 *  Parameters:
 *      @param input: a c string of input from the user.
 *  Returns: A list object of user arguments
*********************************************************************/
struct list *parse_args(char *input) {
    char *token = NULL;
    char *search = " ";
    struct list *args = create_list();

    /* break a string up by spaces. until there are no more*/
    token = strtok(input, search);
    while (token != NULL) {
        add_back(args, token);
        token = strtok(NULL, search);
    }

    /* Search list for instances of "$$" and replace with the current
     * PID. */
    replace_with_pid(args);
    return args;
}

/*********************************************************************
 *  Name: replace_with_pid
 *  Description: Searches through a list and replaces any instance of
 *  "$$" with the current pprocess ID.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *  Returns: n/a
*********************************************************************/
void replace_with_pid(struct list *args) {
    char *dollars = "$$";
    struct link *sentinal = args->front;
    int pid = getpid();
    char c_pid[20];

    /* fill c_pid with null characters */
    memset(c_pid, '\0', sizeof(strlen(c_pid)));
    sprintf(c_pid, "%d", pid);

    for (int i = 0; i < args->size; i++) {

        /* while you find instances of "$$ replace with PID */
        while (strstr(sentinal->value, dollars) != 0) {
            /* malloc new string size of old plus size of PID */
            char *new_arg = malloc(
                    (strlen(sentinal->value) + strlen(c_pid))
                    * sizeof(char));
            memset(new_arg, '\0', sizeof(strlen(new_arg)));

            /* pointer to the beginning of the matched
             * characters*/
            char *match = strstr(sentinal->value, dollars);
            size_t length_match = strlen(match);
            size_t length_former = strlen(sentinal->value);

            /* calculate the position up to which we want to
             * save as the prefix. */
            size_t position = length_former - length_match;

            /* save the space after the $$ instace as the
             * postfix*/
            char *post = strstr(sentinal->value, dollars) + 2;

            /* add the prefix to the new string. */
            strncpy(new_arg, sentinal->value, position);

            /* append the PID to the string. */
            strcat(new_arg, c_pid);

            /* append the post if there is one. */
            if (strlen(post) > 0) {
                strcat(new_arg, post);
            }
            sentinal->value = new_arg;
        }

        /* move on to the next argument. */
        sentinal = sentinal->next;
    }
}

/*********************************************************************
 *  Name: change_directory
 *  Description: Changes the current working directory. If no
 *  additional argument was passed, change to the HOME directory. In
 *  my implementation I also ignore if there was more than one
 *  argument.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *  Returns: n/a
*********************************************************************/
/*https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program*/
void change_directory(struct list *args) {
    if (args->size > 1) {
        chdir(args->front->next->value);
    } else {
        char *path = getenv("HOME");
        chdir(path);
    }
}

/*********************************************************************
 *  Name: show_status
 *  Description: Print out the exit code or the termination value of
 *  the last process that ran.
 *  Parameters:
 *      @param exit_code: the exit code of the process
 *      @param foreground_terminated: whether the last foreground
 *      process was terminated.
 *  Returns: n/.a
*********************************************************************/
/*https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program*/
void show_status(int *exit_code, enum bool *foreground_terminated) {
    fflush(stdout);
    if (*foreground_terminated == TRUE) {
        printf("terminated by signal %d\n", *exit_code);
    } else {
        printf("exit value %d\n", *exit_code);
    }
}

/*********************************************************************
 *  Name: use_built_in_cmd
 *  Description: Uses 1 of the 2 programs that are built into the
 *  shell depending on the arguments passed in.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *      @param exit_code: the exit code of the process
 *      @param foreground_terminated: whether the last foreground
 *      process was terminated.
 *  Returns:
*********************************************************************/
void use_built_in_cmd(
        struct list *args,
        int *exit_code,
        enum bool *foreground_terminated) {

    /* was the command "cd? if so, do that */
    if (strcmp(args->front->value, "cd") == 0) {
        change_directory(args);

        /* was the command status? if so, you know what to do. */
    } else if (strcmp(args->front->value, "status") == 0) {
        if (args->size > 1 &&
            strcmp(args->front->next->value, "&") == 0) {

            /* adjust the foreground terminated status as well as
             * the exit code. */
            *foreground_terminated = TRUE;
            show_status(exit_code, foreground_terminated);
            *foreground_terminated = FALSE;
        } else {
            show_status(exit_code, foreground_terminated);
        }
    }
}

/*********************************************************************
 *  Name: is_built_in_cmd
 *  Description: checks if the command entered by the user is a built
 *      in command.
 *  Parameters:
 *      @param cmd: the command entered by the user
 *  Returns:
*********************************************************************/
enum bool is_built_in_cmd(char *cmd) {
    if (strcmp(cmd, "cd") == 0 ||
        strcmp(cmd, "status") == 0) {
        return TRUE;
    }
    return FALSE;
}

/*********************************************************************
 *  Name: is_comment
 *  Description: Check if the argument list is a comment.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *  Returns: TRUE if comment, FALSE if not.
*********************************************************************/
enum bool is_comment(struct list *args) {
    if (args->front->value[0] == '#') {
        return TRUE;
    }
    return FALSE;
}

/*********************************************************************
 *  Name: is_background
 *  Description: checks if the command was a background command.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *  Returns: TRUE if background, FALSE if not.
*********************************************************************/
enum bool is_background(struct list *args) {

    if (strcmp(args->back->value, "&") == 0) {
        remove_link(args, args->size - 1);
        return TRUE;
    }
    return FALSE;
}

/*********************************************************************
 *  Name: catchSIGTSTP
 *  Description: Handler for the SIGTSTP signal. Changes the parameter
 *  for if background processes are allowed, and prints out the
 *  informative message after foreground process has finished.
 *  Parameters:
 *      @param signo: I don't use this param but examples for signal
 *      handlers had it so it stays.
 *  Returns: n/a
*********************************************************************/
void catchSIGTSTP(int signo) {
    enum bool during_process = FALSE;
    char message[100];
    char *foreground = "\nEntering foreground-only mode (& is now ignored)\n";
    char *background = "\nExiting foreground-only mode\n";

    memset(message, '\0', sizeof(strlen(message)));

    /* keeps the signal handler waiting until child process dies,
     * creates a flag that signals that we need to fake a prompt */
    while (waitpid(spawnpid, NULL, WNOHANG) == 0) {
        during_process = TRUE;
    };

    if (BACKGROUND_ALLOWED && during_process) {
        strcpy(message, foreground);
        BACKGROUND_ALLOWED = FALSE;
    } else if (BACKGROUND_ALLOWED && !during_process) {

        /* if not during a process, we are adding a new line
         * so add the look of a prompt after */
        strcpy(message, foreground);
        strcat(message, ": ");
        BACKGROUND_ALLOWED = FALSE;
    } else if (!BACKGROUND_ALLOWED && during_process) {
        strcpy(message, background);
        BACKGROUND_ALLOWED = TRUE;
    } else {
        strcpy(message, background);
        strcat(message, ": ");
        BACKGROUND_ALLOWED = TRUE;
    }

    /* print our message to the screen */
    fflush(stdout);
    write(STDOUT_FILENO, message, strlen(message));
}

/*********************************************************************
 *  Name: gave_redirect_in
 *  Description: checks if the user gave a redirect command.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *  Returns: TRUE if redirect in was given, FALSE if not
*********************************************************************/
enum bool gave_redirect_in(struct list *args) {
    struct link *sentinal = args->front;
    for (int i = 0; i < args->size; i++) {
        if (strcmp(sentinal->value, "<") == 0) {
            return TRUE;
        }
        sentinal = sentinal->next;
    }
    return FALSE;
}

/*********************************************************************
 *  Name: gave_redirect_out
 *  Description: checks if the user gave a redirect command.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *  Returns: TRUE if redirect out was given, FALSE if not
*********************************************************************/
enum bool gave_redirect_out(struct list *args) {
    struct link *sentinal = args->front;
    for (int i = 0; i < args->size; i++) {
        if (strcmp(sentinal->value, ">") == 0) {
            return TRUE;
        }
        sentinal = sentinal->next;
    }
    return FALSE;
}

/*********************************************************************
 *  Name: get_redirect
 *  Description: finds the value specified by the redirect character
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *      @param character: the character before our command.
 *  Returns: the value specified by the redirect character
*********************************************************************/
char *get_redirect(struct list *args, char *character) {
    struct link *sentinal = args->front;

    for (int i = 0; i < args->size; i++) {
        /* if the value contains the character we are looking for
         * and the list has a link after ... */
        if (strcmp(sentinal->value, character) == 0 &&
            i + 1 < args->size) {

            /* the extra 1 here solved problems when I was testing
             * */
            char *out = malloc(strlen(sentinal->next->value) + 1);
            strcpy(out, sentinal->next->value);

            /* remove the two links from our arguments because we
             * are handling them in this program. */
            remove_link(args, i);
            remove_link(args, i);
            return out;
        }

        /* continue on */
        sentinal = sentinal->next;
    }
    return NULL;
}

/*********************************************************************
 *  Name: print_background_processes
 *  Description: prints the exit message of a background process. Only
 *  prints when a process had died.
 *  Parameters:
 *      @param exit_code: we will assign the exit code of the dead
 *      process.
 *  Returns: n/a
*********************************************************************/
void print_background_processes() {

    int childExitMethod = -1;

    /* check if a process has returned */
    int child_pid = waitpid(-1, &childExitMethod, WNOHANG);

    /* the result will be the id of the child that died. */
    while (child_pid > 0) {

        /* how did the process exit? */
        if (WIFEXITED(childExitMethod)) {
            int exit_code = WEXITSTATUS(childExitMethod);
            fflush(stdout);
            printf("background pid %d is done: exit value %d\n",
                   child_pid,
                   exit_code);

        } else if (WIFSIGNALED(childExitMethod)) {
            int termsig = WTERMSIG(childExitMethod);
            fflush(stdout);
            printf("background pid %d is done: terminated by signal %d\n",
                   child_pid,
                   termsig);
        }

        /* calling this returns additional children that have died
         * so lets loop until this returns no more. */
        child_pid = waitpid(-1, &childExitMethod, WNOHANG);
    }
}

/*********************************************************************
 *  Name: execute_decision
 *  Description: Decides what process to execute a command considering
 *  if the user is currently allow to run processes in the background.
 *  It then calls the appropriate function.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *  Returns: n/a
*********************************************************************/
void execute_decision(struct list *args,
                      char *command_line,
                      int *exit_code,
                      enum bool *foreground_terminated) {

    /* did the user ask for background and is background allowed? */
    if (is_background(args) && BACKGROUND_ALLOWED) {

        /* start a background process. this function returns the
         * number of the process. */
        pid_t background_pid = fork();
        switch (background_pid) {
            case -1:
                perror("[ERROR] failed to spawn process!\n");
                exit(1);
            case 0:
                /* execute the command. */
                background_execute(args, command_line);
                break;
            default:
                /* print out the background PID*/
                fflush(stdout);
                printf("background pid is %d\n", background_pid);
                break;
        }
    } else {
        /*if we're here, its going to run in the foreground. */
        foreground_execute(args, command_line, exit_code, foreground_terminated);
    }
}

/*********************************************************************
 *  Name: foreground_execute
 *  Description: Executes a command in the foreground, reporting the
 *  success of the execution.
 *  Parameters:
 *      @param args: A list object with arguments entered by the user.
 *  Returns: n/a
*********************************************************************/
void foreground_execute(struct list *args,
                        char *command_line,
                        int *exit_code,
                        enum bool *foreground_terminated) {

    spawnpid = -5;
    int childExitMethod = -1;
    char **arg_list;
    FILE *file_out;
    FILE *file_in;
    char *out;
    char *in;

    /* spawnpid named after the lectures :shrug: */
    spawnpid = fork();

    switch (spawnpid) {
        case -1:
            perror("[ERROR] failed to spawn process!\n");
            exit(1);
        case 0:

            /* I follow the process laid out by this stackoverflow
             * question. basically  open the file and then replace
             * with a dup2 call. I add error checking though during
             * the opening process.
             * https://stackoverflow.com/questions/13801175/classic-c-using-pipes-in-execvp-function-stdin-and-stdout-redirection*/
            if (gave_redirect_out(args)) {
                out = get_redirect(args, ">");

                file_out = fopen(out, "w");
                if (file_out == NULL) {

                    /* execessive fflushes, I don't have the time
                     * to figure out if any of them are
                     * necessary. */
                    fflush(stdout);
                    printf("cannot open %s for output\n", out);
                    fflush(stdout);
                    exit(1);
                }
                dup2(fileno(file_out), STDOUT_FILENO);
            }

            /* pretty much repeat the same thing with stdin */
            if (gave_redirect_in(args)) {
                in = get_redirect(args, "<");

                file_in = fopen(in, "r");
                if (file_in == NULL) {
                    fflush(stdout);
                    printf("cannot open %s for input\n", in);
                    fflush(stdout);
                    exit(1);
                }
                dup2(fileno(file_in), STDIN_FILENO);
            }

            /* this grabs a list object and converts to an
             * array with the final item as NULL. */
            arg_list = get_array(args);

            /* handle signals differently than the parent
             * SIGINT should quit, SIGTSTP should do nothing. */
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_IGN);

            /* if exit code returns, something went wrong. */
            *exit_code = execvp(arg_list[0], arg_list);
            if (*exit_code == -1) {
                fflush(stdout);
                printf("%s: no such file or directory\n", arg_list[0]);
            }

            /* set the exit code to our own. This might not
             * matter though because I'm pretty sure the
             * instance is destroyed. */
            *exit_code = 1;

            /* free up remaining dynamic memory. */
            destroy_array(arg_list);
            free(command_line);
            destroy_list(args);
            exit(1);
        default:
            /* parent drops to here */
            break;
    }

    /* have the parent wait for the child. */
    waitpid(spawnpid, &childExitMethod, 0);

    /* This block is pretty much the exact same as the
     * lectures. Just check how the thing exited and then
     * report on it. */
    if (WIFEXITED(childExitMethod)) {
        *foreground_terminated = FALSE;
        *exit_code = WEXITSTATUS(childExitMethod);

    } else if (WIFSIGNALED(childExitMethod)) {
        int termsig = WTERMSIG(childExitMethod);
        fflush(stdout);
        printf("terminated by signal %d\n", termsig);

        *foreground_terminated = TRUE;
        *exit_code = termsig;
    }
}

/*********************************************************************
 *  Name: background_execute
 *  Description:
 *      @param args: A list object with arguments entered by the user.
 *      @param command_line:
 *  Parameters:
 *  Returns:
*********************************************************************/
void background_execute(struct list *args,
                        char *command_line) {

    /* just ignore the SIGINT, SIGTSTP signal. */
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    int *pid_background = malloc(sizeof(int));
    *pid_background = getpid();

    char **arg_list;
    FILE *file_out;
    FILE *file_in;
    char *out;
    char *in;

    char *exit_message = malloc(256 * sizeof(char));
    memset(exit_message, '\0', sizeof(strlen(exit_message)));

    /* check to see if the user specified a redirect, if they did
     * do the redirect. */
    if (gave_redirect_out(args)) {
        out = get_redirect(args, ">");
    } else {
        out = "/dev/null";
    }

    /* I follow the process laid out by this stackoverflow
     * question. basically  open the file and then replace
     * with a dup2 call. I add error checking though during
     * the opening process.
     * https://stackoverflow.com/questions/13801175/classic-c-using-pipes-in-execvp-function-stdin-and-stdout-redirection*/
    file_out = fopen(out, "w");
    if (file_out == NULL) {

        /* execessive fflushes, I don't have the time
         * to figure out if any of them are
         * necessary. */
        fflush(stdout);
        printf("cannot open %s for output\n", out);
        fflush(stdout);
        exit(1);
    }
    dup2(fileno(file_out), STDOUT_FILENO);

    /* check to see if the user specified a redirect, if they did
     * do the redirect. */
    if (gave_redirect_in(args)) {
        in = get_redirect(args, "<");
    } else {
        in = "/dev/null";
    }

    /* pretty much repeat the same thing with stdin */
    file_in = fopen(in, "r");
    if (file_in == NULL) {
        fflush(stdout);
        printf("cannot open %s for input\n", in);
        fflush(stdout);
        exit(1);
    }

    dup2(fileno(file_in), STDIN_FILENO);

    /* turn list object into array with last item as NULL. */
    arg_list = get_array(args);

    /* execute. */
    execvp(arg_list[0], arg_list);

    /* free up memory.*/
    destroy_array(arg_list);
    free(command_line);
    destroy_list(args);
    exit(1);
}

