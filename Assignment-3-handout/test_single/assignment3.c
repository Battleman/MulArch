/*
 ============================================================================
 Filename    : assignment3.c
 Author      : Arash Pourhabibi [Replace it with your name and SCIPER ID]
 Date        : Nov. 13th, 2017
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>

typedef struct node {
    int val;
    struct node *next;
} node_t;


/*
 * This function initializes a new linked list.
 */
void init_list(node_t **head, int val) {
    (*head) = malloc(sizeof(node_t));
    
    (*head)->val = val;
    (*head)->next = NULL;
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
    node_t *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    
    current->next = malloc(sizeof(node_t));
    current->next->val = val;
    current->next->next = NULL;
}

/*
 * This function adds a new given element to the beginning of a given linked list.
 */
void add_first(node_t **head, int val) {
    node_t *new_node;
    new_node = malloc(sizeof(node_t));
    
    new_node->val = val;
    new_node->next = *head;
    *head = new_node;
}

/*
 * This function inserts a new given element at the specified position of a given linked list.
 * It returns 0 on a successful insertion, and -1 if the list is not long enough.
 */
int insert(node_t **head, int val, int index) {
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
    node_t * current = *head;
    node_t * temp_node = NULL;
    
    for (int i = 0; i < index-1; i++) {
        if (current->next == NULL) {
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
    current = (*head)->next;
    while (current) {
        if (current->val == val) {
            previous->next = current->next;
            free(current);
            return val;
        }
        
        previous = current;
        current  = current->next;
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
