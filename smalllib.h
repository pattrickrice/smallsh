/*********************************************************************
 * Author: Patrick Rice
 * Date: 8/06/2018
 * Description: The header for the main commands used in smallsh.
*********************************************************************/
#ifndef ASSIGNMENT_3_SMALLSH_H
#define ASSIGNMENT_3_SMALLSH_H

/* all the necessary imports */
#include <malloc.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <dirent.h>
#include "list.h"
#include <memory.h>
#include <stdlib.h>
#include <assert.h>
#include <wait.h>
#include "signal.h"
#include <sys/fcntl.h>

char *get_user_input();

void print_background_processes();

struct list *parse_args(char *);

void replace_with_pid(struct list *);

void change_directory(struct list *);

void show_status(int *, enum bool *);

void use_built_in_cmd(struct list *, int *, enum bool *);

enum bool is_built_in_cmd(char *);

void catchSIGTSTP(int);

void execute_decision(struct list *,
                      char *,
                      int *,
                      enum bool *);

void foreground_execute(struct list *,
                        char *,
                        int *,
                        enum bool *);

void background_execute(struct list *,
                        char *);

#endif