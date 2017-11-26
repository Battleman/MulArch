/*
 ============================================================================
 Filename    : assignment3.c
 Author      : Olivier Cloux 236079
 Date        : Nov. 13th, 2017
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct node {
    int val;
    struct node *next;
    omp_lock_t lock;
} node_t;


/*
 * This function initializes a new linked list.
 */

void init_list(node_t **head, int val) {
    (*head) = malloc(sizeof(node_t));
    if(NULL == *head){
      return;
    }
    (*head)->val = val;
    (*head)->next = NULL;
    omp_init_lock(&((*head)->lock));
}

/*
 * This function prints all the elements of a given linked list.
 */
void print_list(node_t *head) {
    node_t *current = head;
    omp_set_lock(&(current->lock));
    while (current != NULL) {
        printf("%d\n", current->val);
        if(current->next != NULL){
            omp_set_lock(&((current->next)->lock));
        }
        omp_unset_lock(&(current->lock));
        current = current->next;
    }
}

/*
 * This function counts the elements of a given linked list and returns the counted number.
 */
int count(node_t *head)
{
    node_t *current = head;
    int count = 0;

    while (current != NULL) {
        count++;
        current = current->next;
    }

    return count;
}

/*
 * This function appends a new given element to the end of a given linked list.
 */
void append(node_t *head, int val) {
    if(NULL == head || -1 == val){
        //avoids SEGFAULT and confusion between value and errcode.
        return;
    }
    node_t *current = head;

    while (NULL != current->next) {
        current = current->next;
    }
    omp_set_lock(&(current->lock));
    current->next = malloc(sizeof(node_t));
    if(NULL == current->next){
        return;
    }
    current->next->val = val;
    current->next->next = NULL;
    omp_init_lock(&((current->next)->lock));
    omp_unset_lock(&(current->lock));
}

/*
 * This function adds a new given element to the beginning of a given linked list.
 */
void add_first(node_t **head, int val){
    if(NULL == head || NULL == *head || val == -1){
        return;
    }
    node_t *new_node;
    new_node = malloc(sizeof(node_t));
    if(NULL == new_node){
        return;
    }
    new_node->val = val;
    omp_init_lock(&(new_node->lock));
    node_t* prev_head = *head;
    while(omp_test_lock(&(prev_head->lock))){
      prev_head = *head;
    }
    new_node->next = prev_head;
    *head = new_node;
    omp_unset_lock(&(prev_head->lock));
}

/*
 * This function inserts a new given element (different than -1) at the specified position of a given linked list.
 * It returns 0 on a successful insertion, and -1 if the list is not long enough.
 */
int insert(node_t **head, int val, int index) {
    if(NULL == head || NULL == *head || -1 == val){
        //avoid SEGFAULT and confusion between a value and the errcode -1
        return -1;
    }
    if (index == 0) {
        add_first(head, val);
        return 0;
    }

    node_t * current = *head;
    omp_set_lock(&(current->lock));
    for (int i = 0; i < index-1; i++) {
        if (current->next == NULL) {
            return -1;
        }
        omp_set_lock(&((current->next)->lock));
        omp_unset_lock(&(current->lock));
        current = current->next;
    }

    node_t *new_node;
    new_node = malloc(sizeof(node_t));
    if(NULL == new_node){
        //to avoid SEGFAULT
        return -1;
    }
    new_node->val = val;
    new_node->next = current->next;
    omp_init_lock(&(new_node->lock));
    current->next = new_node;
    omp_unset_lock(&(current->lock));
    return 0;
}

/*
 * This function pops the first element of a given linked list.
 * The value of that element is returned (if list is not empty), and the element is removed.
 */
int pop(node_t **head) {
    int retval = -1;
    node_t* next_node = NULL;
    if (*head == NULL) {
        return -1;
    }

    next_node = (*head)->next;
    retval = (*head)->val;
    free(*head);
    *head = next_node;

    return retval;
}


/*
 * This function removes the specified element of a given linked list
 * The value of that element is returned if the list is long enough; otherwise it returns -1
 * Please note the index starts from 0
 */

int remove_by_index(node_t **head, int index) {
    if (index == 0) {
        return pop(head);
    }
    int retval = -1;


    if(head == NULL || *head == NULL){
        //avoids SEGFAULT if head or *head is NULL
        return retval;
    }
    node_t * current = *head;
    node_t* previous = NULL;
    omp_set_lock(&(current->lock)); //lock head

    for (int i = 0; i <= index-1; i++) {
        if (current->next == NULL) {
            //error, we list is not long enough. unlock everything and return
            if(previous != NULL){
              //need to unlock previous, but only if
              //lock exists (will after 1st iteration)
              omp_unset_lock(&(previous->lock));
            }
            omp_unset_lock(&(current->lock));
            return -1;
        }
        /*Lock next and unlock previous, before shifting*/
        omp_set_lock(&((current->next)->lock));
        if(previous != NULL){
          //will exist after 1st iteration
          omp_unset_lock(&(previous->lock));
        }
        previous = current;
        current = current->next;
    }
    /*At this point, previous and current are locked*/
    // Current node has to be removed!
    previous->next = current->next;
    retval = current->val;
    omp_unset_lock(&(current->lock));
    free(current);
    omp_unset_lock(&(previous->lock));

    return retval;
}

/*
 * This function removes the specified element of a given linked list.
 * The value of that element is returned if the element is found; otherwise it returns -1.
 */
int remove_by_value(node_t **head, int val) {
    node_t *previous, *current;

    if (*head == NULL) {
        return -1;
    }

    if ((*head)->val == val) {
        return pop(head);
    }
    previous = *head;
    omp_set_lock(&(previous->lock));
    current = (*head)->next;
    omp_set_lock(&(current->lock));
    while (current != NULL) {
        if (current->val == val) {
            previous->next = current->next;
            //free the element and locks, now that the operation has been done
            omp_unset_lock(&(current->lock));
            free(current);
            omp_unset_lock(&(previous->lock));
            return val;
        }
        omp_unset_lock(&(previous->lock));
        previous = current;
        current  = current->next;
        omp_set_lock(&(current->lock));
    }
    return -1;
}

/*
 * This function searched for a specified element in a given linked list.
 * The index of that element is returned if the element is found; otherwise it returns -1.
 */
int search_by_value(node_t *head, int val) {
    node_t *current = head;
    int index = 0;

    if (current == NULL) {
        return -1;
    }

    while (current) {
        if (current->val == val) {
            return index;
        }

        current  = current->next;
        index++;
    }

    return -1;
}

/*
 * This function deletes all element of a given linked list, and frees up all the allocated resources.
 */
void delete_list(node_t **head) {
    if(head == NULL || *head == NULL){
      //probable cause : the list has already been deleted; don't proceed.
      return;
    }
    node_t *current = *head;
    omp_set_lock(&(current->lock));
    *head = NULL; //avoids any program to execute after this is called
    node_t *next;

    while (current) {
        next = current->next;
        if(next != NULL){
            omp_set_lock(&(next->lock));
        }
        omp_unset_lock(&(current->lock));
        free(current);
        current = next;
    }
}

int main(void) {
    // omp_lock_t head_lock, tail_lock, tail_lock;
    // omp_init_lock(head_lock);
    // omp_init_lock(middle_lock);
    // omp_init_lock(tail_lock);

    omp_lock_t head_lock;
    omp_init_lock(&head_lock);
    node_t *test_list;
    init_list(&test_list, 2);
    printf("Done init\n");

    add_first(&test_list, 1);
    printf("Done add_first\n");

    append(test_list, 3);
    append(test_list, 4);
    append(test_list, 5);
    printf("Done 3 add\n");
    printf("Count = %d\n", count(test_list));
    remove_by_index(&test_list, 2);
    printf("Count = %d\n", count(test_list));

    printf("Search for 5 -> index = %d\n", search_by_value(test_list, 5));
    remove_by_value(&test_list, 5);
    printf("Search for 5 -> index = %d\n", search_by_value(test_list, 5));

    print_list(test_list);
    delete_list(&test_list);
    return 0;
}


/*###
* FUNCTIONALITY QUESTIONS
* Counting should wait until previously started operation is over
*
*
*/
