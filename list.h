#include <malloc.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>

#ifndef ASSIGNMENT_3_LIST_H
#define ASSIGNMENT_3_LIST_H

enum bool {FALSE, TRUE};

struct link{
    char *value;
    struct link *next;
};

struct list{
    int size;
    struct link *front;
    struct link *back;
};

struct link *create_link();

struct list *create_list();

void add_back(struct list* curr_list, char* value);

void print_list(struct list* curr_list);

enum bool is_empty(struct list* curr_list);

enum bool is_comment(struct list* args);

enum bool is_background(struct list*args);

char **get_array(struct list* curr_list);

void remove_link(struct list* curr_list, int position);

void destroy_array(char **array);

void destroy_list(struct list* curr_list);

#endif
