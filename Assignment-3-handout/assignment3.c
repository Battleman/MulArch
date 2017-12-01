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

#define ERRCODE -1

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
    //new node -> init its lock
    omp_init_lock(&((*head)->lock));
}

/*
 * This function prints all the elements of a given linked list.
 */
void print_list(node_t *head) {
    /*Need to lock an element, lock its next then unlock former.
    * Ensures we don't run into strange modifications
    */
    node_t *current = head;
    omp_set_lock(&(current->lock));

    while (current != NULL) {
        printf("%d\n", current->val);
        if(current->next != NULL){
            //don't need to lock after the last element
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
    /*Similar as print : lock an element, its next and unlock former.
    * This will ensure consistency.
    */
    node_t *current = head;
    omp_set_lock(&(current->lock));
    int count = 0;

    while (current != NULL) {
        count++;
        if(current->next != NULL){
            //lock the next, but not after last element
            omp_set_lock(&((current->next)->lock));
        }
        //once next is locked, you can unlock current
        omp_unset_lock(&(current->lock));
        current = current->next;
    }

    return count;
}

/*
 * This function appends a new given element to the end of a given linked list.
 */
void append(node_t *head, int val) {
    if(NULL == head || ERRCODE == val){
        //avoids SEGFAULT and confusion between value and errcode.
        return;
    }
    node_t *current = head;
    omp_set_lock(&(current->lock)); //start off by locking head
    while (NULL != current->next) {
        //lock next element, and once it's acquired unlock current
        omp_set_lock(&((current->next)->lock));
        omp_unset_lock(&(current->lock));
        current = current->next;
    }
    //at this point, only current is locked (aka the last element)
    current->next = malloc(sizeof(node_t));
    if(NULL == current->next){
        //if malloc failed, unlock everything and return
        omp_unset_lock(&(current->lock));
        return;
    }
    //is malloc succeeded, init the new node;
    current->next->val = val;
    current->next->next = NULL;
    omp_init_lock(&((current->next)->lock)); //new node -> init its lock
    omp_unset_lock(&(current->lock)); //release former tail
}

/*
 * This function adds a new given element to the beginning of a given linked list.
 */
void add_first(node_t **head, int val){
    if(NULL == head || NULL == *head || val == ERRCODE){
        return;
    }

    //node preparation : malloc, ensuring it succeeded, setting val,...
    node_t *new_node;
    new_node = malloc(sizeof(node_t));
    if(NULL == new_node){
        return;
    }
    new_node->val = val;
    omp_init_lock(&(new_node->lock));


    //head update
    node_t* prev_head = *head;
    while(omp_test_lock(&(prev_head->lock))){
        /*Unlikely but possible case : two add_first are trying to execute
        * concurrently or head is being deleted/updated. We update head
        * until lock is clear (node was updated) or a new head is given and
        thus free of lock (head was modified), so we can lock it
        */
        prev_head = *head;
        if(prev_head == NULL){
            /*if reason lock was not free is that the list was being deleted,
            * don't proceed*/
            omp_destroy_lock(&(new_node->lock));
            free(new_node);
            return;
        }
    }
    new_node->next = prev_head;
    *head = new_node; //update head of list
    omp_unset_lock(&(prev_head->lock)); //only AFTER, you unlock previous head
}

/*
 * This function inserts a new given element (different than ERRCODE) at the specified position of a given linked list.
 * It returns 0 on a successful insertion, and ERRCODE if the list is not long enough.
 */
int insert(node_t **head, int val, int index) {
    if(NULL == head || NULL == *head || ERRCODE == val){
        /*avoid SEGFAULT if head or *head is null
        * and confusion between a value and the ERRCODE
        */
        return ERRCODE;
    }
    if (index == 0) {
        add_first(head, val);
        return 0;
    }

    node_t * current = *head;
    omp_set_lock(&(current->lock));
    for (int i = 0; i < index-1; i++) {
        if (current->next == NULL) {
            omp_unset_lock(&(current->lock));
            return ERRCODE;
        }
        omp_set_lock(&((current->next)->lock));
        omp_unset_lock(&(current->lock));
        current = current->next;
    }

    node_t *new_node;
    new_node = malloc(sizeof(node_t));
    if(NULL == new_node){
        //to avoid SEGFAULT
        return ERRCODE;
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
    int retval = ERRCODE;
    node_t* next_node = NULL;
    if (*head == NULL) {
        return ERRCODE;
    }
    node_t* prev_head = *head;
    while(omp_test_lock(&(prev_head->lock))){
        //if head is busy (maybe being updated), update
        //until head is free
        prev_head = *head;
    }
    next_node = prev_head->next;
    *head = next_node; //update head
    retval = prev_head->val;
    omp_unset_lock(&(prev_head->lock));
    omp_destroy_lock(&(prev_head->lock));
    free(prev_head);

    return retval;
}

/*
 * This function removes the specified element of a given linked list
 * The value of that element is returned if the list is long enough; otherwise it returns ERRCODE
 * Please note the index starts from 0
 */
int remove_by_index(node_t **head, int index) {
    if (index == 0) {
        return pop(head);
    }
    int retval = ERRCODE;


    if(head == NULL || *head == NULL){
        //avoids SEGFAULT if head or *head is NULL
        return retval;
    }
    node_t * current = *head;
    node_t* previous = NULL;
    omp_set_lock(&(current->lock)); //lock head

    for (int i = 0; i <= index-1; i++) {
        if (current->next == NULL) {
            //error, list is not long enough. unlock everything and return
            if(previous != NULL){
              //need to unlock previous, but only if
              //lock exists (only useful if list only has head)
              omp_unset_lock(&(previous->lock));
            }
            omp_unset_lock(&(current->lock));
            return ERRCODE;
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

    /*At this point, previous and current are locked
    * Current node has to be removed!
    */
    previous->next = current->next;
    retval = current->val;
    omp_unset_lock(&(current->lock));
    omp_destroy_lock(&(current->lock));
    free(current);
    //once current node has been deleted and freed, we can unlock previous
    omp_unset_lock(&(previous->lock));

    return retval;
}

/*
 * This function removes the specified element of a given linked list.
 * The value of that element is returned if the element is found; otherwise it returns ERRCODE.
 */
int remove_by_value(node_t **head, int val) {
    node_t *previous, *current;

    if (head == NULL || *head == NULL || val == ERRCODE) {
        return ERRCODE;
    }

    if ((*head)->val == val) {
        return pop(head);
    }

    previous = *head;
    current = (*head)->next;
    omp_set_lock(&(previous->lock));
    omp_set_lock(&(current->lock));

    while (current != NULL) {
        if (current->val == val) { //value found
            previous->next = current->next;
            //free the element and locks, now that the operation has been done
            omp_unset_lock(&(current->lock));
            omp_destroy_lock(&(current->lock));
            free(current);
            omp_unset_lock(&(previous->lock));
            return val;
        }
        if(current->next != NULL){
            //lock next (if possible) before unlocking previous
            omp_set_lock(&((current->next)->lock));
        }
        omp_unset_lock(&(previous->lock));
        previous = current;
        current  = current->next;
    }

    /*If not found, unlock everything and return ERRCODE*/
    omp_unset_lock(&(current->lock));
    return ERRCODE;
}

/*
 * This function searched for a specified element in a given linked list.
 * The index of that element is returned if the element is found; otherwise it returns ERRCODE.
 */
int search_by_value(node_t *head, int val) {
    node_t *current = head;
    int index = 0;

    if (current == NULL || val == ERRCODE) {
        return ERRCODE;
    }
    omp_set_lock(&(current->lock));
    while (current) {
        if (current->val == val) {
            //value was found, unlock and return
            omp_unset_lock(&(current->lock));
            return index;
        }
        if(current->next != NULL){
            omp_set_lock(&((current->next)->lock));
        }
        omp_unset_lock(&(current->lock));
        current  = current->next;
        index++;
    }

    return ERRCODE;
}

/*
 * This function deletes all element of a given linked list, and frees up all the allocated resources.
 */
void delete_list(node_t **head) {
    //please note the signature, that takes a node_t** and not a node_t*
    
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
        omp_destroy_lock(&(current->lock));
        free(current);
        current = next;
    }
}

int main(void) {

    node_t *test_list;
    init_list(&test_list, 2);

    add_first(&test_list, 1);

    append(test_list, 3);
    append(test_list, 4);
    append(test_list, 5);

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
