/*
============================================================================
Filename    : assignment3.c
Author      : Arnaud Boissaye 214939
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

//TODO create here 2 function aquire and free a lock.


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
* TODO Should be done by only 1 thread.
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
* TODO could it be parrallelised ? sounds like a 1 threaded function as for print.
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
  node_t *previous = current;
  //lock the head
  omp_set_lock(&current->lock);
  while (current->next != NULL) {
    //go to next
    current = current->next;
    //lock it
    omp_set_lock(&current->lock);
    //unlock previous only if you are able to lock the current
    omp_unset_lock(&previous->lock);
    //update previous
    previous = current;
  }

  current->next = malloc(sizeof(node_t));
  current->next->val = val;
  current->next->next = NULL;

  //free the lock when you finish to append
  omp_unset_lock(&current->lock);
}

/*
* This function adds a new given element to the beginning of a given linked list.
* here we just need to block the head
*/
void add_first(node_t **head, int val) {
  node_t *new_node;
  new_node = malloc(sizeof(node_t));
  new_node->val = val;
  new_node->next = *head;

  //cette ligne compile pas
  //omp_set_lock(&head->lock);
  *head = new_node;
  //as head become new node head become new_node->next
  // omp_unset_lock(&new_node->next->lock);
}

/*
* This function inserts a new given element at the specified position of a given linked list.
* It returns 0 on a successful insertion, and -1 if the list is not long enough.
* TODO Same as add_first : should be multithreaded !!!!
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
* TODO think about two thread that just want to pop the first element but ask for it ar the same time.
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
 * Please note the index starts from 0.
 * TODO multithreaded !!!!
 */
int remove_by_index(node_t **head, int index) {

    if (index == 0) {
        return pop(head);
    }

    int retval = -1;
    node_t * current = *head;
    node_t * previous = NULL;

    for (int i = 0; i <= index-1; i++) {
        if (current->next == NULL) {
            return -1;
        }
        previous = current;
        current = current->next;
    }

    // Current node has to be removed!
    previous->next = current->next;
    retval = current->val;
    free(current);

    return retval;
}

/*
* This function removes the specified element of a given linked list.
* The value of that element is returned if the element is found; otherwise it returns -1.
* TODO Same as remove_by_index : multithreaded !!!!
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
  printf("bonjour\n");
  node_t *test_list;
  printf("je fonctionne\n");
  init_list(&test_list, 2);
  printf("2\n");
  add_first(&test_list, 1);
  printf("3\n");
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
