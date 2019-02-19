/*********************************************************************
 * Author: Patrick Rice
 * Date: 8/06/2018
 * Description: smallsh is a shell that runs on the command line. It
 * can handle creating background processes, term signals, and has
 * several built in commands.
*********************************************************************/


#include "smalllib.h"

/*********************************************************************
 *  Name: main
 *  Description: Creates a shell program that prompts the user for
 *  commands. The shell runs commands in the foreground or background
 *  depending on if the user specified and if the user is allowed.
 *  The shell has signal handlers for SIGINT and SIGTSTP which are
 *  specified in the requirements.
 *  Parameters:
 *      @param the array we want to free.
 *  Returns: n/a
*********************************************************************/
int main()
{
    /* handle our signals*/
    struct sigaction SIGTSTP_action = {0};

    /* just ignore SIGINT */
    signal(SIGINT, SIG_IGN);

    /* special handler for SIGTSTP */
    SIGTSTP_action.sa_handler = catchSIGTSTP;
    SIGTSTP_action.sa_flags = SA_RESTART;
    sigfillset(&SIGTSTP_action.sa_mask);
    sigaction(SIGTSTP, &SIGTSTP_action, NULL);

    /* begin the meat of the program */
    char *command_exit = "exit";
    char *command_line = get_user_input();
    struct list *list_parsed_args;

    enum bool foreground_terminated;
    int exit_code = 0;

    /* did the user request to exit? */
    while (strcmp(command_exit, command_line) != 0)
    {
        list_parsed_args = parse_args(command_line);
        /* decide if we have the command built in or
         * if we need to execute the command. */
        if (!is_empty(list_parsed_args) &&
            is_built_in_cmd(list_parsed_args->front->value) &&
            !is_comment(list_parsed_args))
        {

            use_built_in_cmd(
                    list_parsed_args,
                    &exit_code,
                    &foreground_terminated);
        } else if (!is_empty(list_parsed_args) &&
                   !is_comment(list_parsed_args))
        {

            execute_decision(
                    list_parsed_args,
                    command_line,
                    &exit_code,
                    &foreground_terminated);

        }

        /* clean up before looping*/
        free(command_line);
        destroy_list(list_parsed_args);

        /* print finished background processes. */
        print_background_processes();

        /* reprompt the user. */
        command_line = get_user_input();
    }

    /* If pid is negative, but not -1, sig shall be sent to all
     * processes (excluding an unspecified set of system
     * processes) whose process group ID is equal to the
     * absolute value of pid, and for which the process has
     * permission to send a signal.
     * https://linux.die.net/man/3/kill */
    signal(SIGQUIT, SIG_IGN);
    kill(-1*getpid(), SIGQUIT);

    /* at this point, it's just the cmd left*/
    free(command_line);
    return 0;
}
