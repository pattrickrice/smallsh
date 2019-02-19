/*********************************************************************
 * Author: Patrick Rice
 * Date: 8/06/2018
 * Description: List implementation. A typical data structure
 * implementation.
*********************************************************************/
#include "list.h"

/*********************************************************************
 *  Name: create_link
 *  Description: allocates memory for a new link object.
 *  Parameters: n/a
 *  Returns: the new link
*********************************************************************/
struct link *create_link(){
    struct link *new_link = malloc(sizeof(struct link));
    return new_link;
}

/*********************************************************************
 *  Name: free_link
 *  Description: frees memory for the link
 *  Parameters:
 *  Returns:
*********************************************************************/
void free_link(struct link *link){
    free(link);
}

/*********************************************************************
 *  Name: create_list
 *  Description: Creates a new list object, sets the size to 0
 *  Parameters: n/a
 *  Returns: A newly allocated list object.
*********************************************************************/
struct list *create_list(){
    struct list *new_list = malloc(sizeof(struct list));
    new_list->size = 0;
    return new_list;
}

/*********************************************************************
 *  Name: add_back
 *  Description: Adds a value into the list by creating a new link.
 *  Parameters:
 *      @param curr_list: the list we are working with.
 *      @param value: the value we are adding to the list.
 *  Returns: n/a
*********************************************************************/
void add_back(struct list* curr_list, char* value){
    if (curr_list->size == 0) {
        curr_list->back = create_link();
        curr_list->front = curr_list->back;
    } else {
        curr_list->back->next = create_link();
        curr_list->back = curr_list->back->next;
    }
    curr_list->back->value = value;
    curr_list->size++;
}

/*********************************************************************
 *  Name: remove_link
 *  Description: Removes a link from the structure and repairs the
 *  whole in the list the link left behind.
 *  Parameters:
 *      @param curr_list: the list we are working with.
 *      @param position: the position on the list we want to remove.
 *  Returns: n/a
*********************************************************************/
void remove_link(struct list* curr_list, int position){
    struct link *sentinal = curr_list->front;
    struct link *garbage = curr_list->front;

    struct link *prev = curr_list->front;

    /* search through the list until we are at the position. */
    for (int i = 0; i <curr_list->size; i++){
        if (i == position){
            prev->next = sentinal->next;
            garbage = sentinal;
            free_link(garbage);
            curr_list->size--;
            break;
        }
        prev = sentinal;
        sentinal = sentinal->next;
    }
}

/*********************************************************************
 *  Name: get_array
 *  Description: creates an array structure out of the list object.
 *  Parameters:
 *      @param curr_list: the list we are working with.
 *  Returns: an array of c strings.
*********************************************************************/
char **get_array(struct list* curr_list){
    int size = curr_list->size + 2;
    char **array = malloc(size * sizeof(char*));

    struct link *sentinal = curr_list->front;
    for (int i = 0; i <curr_list->size; i++){

        /* as long as the list continues to exist, the method won't
         * be a hug deal. */
        array[i] = sentinal->value;
        sentinal = sentinal->next;
    }
    array[curr_list->size] = NULL;
    return array;
}

/*********************************************************************
 *  Name: destroy_array
 *  Description: frees the memory of the array.
 *  Parameters:
 *      @param the array we want to free.
 *  Returns: n/a
*********************************************************************/
void destroy_array(char **array){
    free(array);
}

/*********************************************************************
 *  Name: print_list
 *  Description: Prints the contents of the list with indices.
 *  Parameters:
 *      @param curr_list: the list we are working with.
 *  Returns: n/a
*********************************************************************/
void print_list(struct list* curr_list){
    struct link *sentinal = curr_list->front;
    for (int i = 0; i <curr_list->size; i++){
        fflush(stdout);
        printf("%s\n", sentinal->value);
        sentinal = sentinal->next;
    }
}

/*********************************************************************
 *  Name: is_empty
 *  Description: returns if the list is empty.
 *  Parameters:
 *      @param curr_list: the current working list.
 *  Returns: TRUE if empty, FALSE if not.
*********************************************************************/
enum bool is_empty(struct list* curr_list){
    if (curr_list->size == 0){
        return TRUE;
    }
    return FALSE;
}

/*********************************************************************
 *  Name: destroy_list
 *  Description: frees up memory for the list object.
 *  Parameters:
 *      @param curr_list: the current working list.
 *  Returns: n/a
*********************************************************************/
void destroy_list(struct list* curr_list){
    struct link *sentinal = curr_list->front;
    struct link *garbage = curr_list->front;

    for (int i =0; i < curr_list->size; i++){
        assert(sentinal != 0);
        sentinal = sentinal->next;
        free_link(garbage);
        garbage = sentinal;
    }
    free(curr_list);
}
