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

    (*head)->val = val;
    (*head)->next = NULL;
    omp_init_lock(&((*head)->lock));
}

/*
 * This function prints all the elements of a given linked list.
 */
void print_list(node_t *head) {
    node_t *current = head;

    while (current != NULL) {
        printf("%d\n", current->val);
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
    omp_unset_lock(&(current->lock));
}

/*
 * This function adds a new given element to the beginning of a given linked list.
 */
void add_first(node_t **head, int val) {
    node_t *new_node;
    new_node = malloc(sizeof(node_t));
    if(NULL == new_node){
        return;
    }
    new_node->val = val;
    new_node->next = *head;
    *head = new_node;
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

    for (int i = 0; i < index-1; i++) {
        if (current->next == NULL) {
            return -1;
        }
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
    current->next = new_node;

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
 * This function removes the specified element of a given linked list.
 * The value of that element is returned if the list is long enough; otherwise it returns -1.
 */
int remove_by_index(node_t **head, int index) {

    if (index == 0) {
        return pop(head);
    }

    int retval = -1;

    if(head == NULL || *head == NULL){
        //avoids horrible SEGFAULT if head or *head is NULL
        return retval;
    }
    node_t * current = *head;
    node_t * temp_node = NULL;

    for (int i = 0; i < index-1; i++) {
        if (NULL == current->next) {
            return -1;
        }
        current = current->next;
    }

    temp_node = current->next;
    retval = temp_node->val;
    current->next = temp_node->next;
    free(temp_node);

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
void delete_list(node_t *head) {
    node_t *current = head;
    node_t *next;

    while (current) {
        next = current->next;
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
    delete_list(test_list);

    return 0;
}
