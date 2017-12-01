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
here we need to be sure that the current node is not
deleted when we print it

in order to do so we lock the current print lock the next one
then unlock the current to move forward
*/
void print_list(node_t *head) {
  node_t *current = head;
  omp_set_lock(&current->lock);
  while (current != NULL) {
    printf("%d\n", current->val);
    omp_set_lock(&current->next->lock);
    omp_unset_lock(&current->lock);
    current = current->next;

  }
}

/*
* This function counts the elements of a given linked list and returns the counted number.
* here we need to be sure that the count is correct so no add node or delete on
the current one we are counting.

we go through the list the same way as print list.

*/
int count(node_t *head)
{
  node_t *current = head;
  int count = 0;
  //lock the head
  omp_set_lock(&current->lock);
  while (current != NULL) {
    count++;
    //lock the next one
    omp_set_lock(&current->next->lock);
    //unlock the current and move forward
    omp_unset_lock(&current->lock);
    current = current->next;
  }

  return count;
}

/*
* This function appends a new given element to the end of a given linked list.

Here we need to be sure that the last element of the list is
the last element of the list.

in order to do so we go through the list locking ...
and when we found the last one we block on it until

*/
void append(node_t *head, int val) {
  node_t *current = head;
  node_t *previous = current;
  //lock the head
  omp_set_lock(&current->lock);
  while (current->next != NULL) {
    //lock the next one
    omp_set_lock(&current->next->lock);
    //go to next
    current = current->next;
    //unlock previous only if you are able to lock the current
    omp_unset_lock(&previous->lock);
    //update previous
    previous = current;
  }
  printf("I DID IT\n");
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

  //init and lock it !
  omp_init_lock(&new_node->lock);
  omp_set_lock(&new_node->lock);

  //lock the head
  omp_set_lock(&new_node->next->lock);
  *head = new_node;

  //unlock after set
  omp_unset_lock(&new_node->next->lock);
  omp_unset_lock(&new_node->lock);
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
  //lock the head
  omp_set_lock(&current->lock);

  for (int i = 0; i < index-1; i++) {
    if (current->next == NULL) {
      return -1;
    }
    //lock the next node
    omp_set_lock(&current->next->lock);
    //unlock the current if you locked the next
    omp_unset_lock(&current->lock);
    current = current->next;

  }

  node_t *new_node;
  new_node = malloc(sizeof(node_t));
  new_node->val = val;
  new_node->next = current->next;
  omp_init_lock(&new_node->lock);

  current->next = new_node;
  //can free because we have finish the modification
  omp_unset_lock(&current->lock);
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
  //lock the new head to be sure it will not be delete by an other one
  omp_set_lock(&(*head)->next->lock);

  next_node = (*head)->next;
  retval = (*head)->val;
  //we have to destroy the lock !!!
  omp_destroy_lock(&(*head)->lock);
  free(*head);
  *head = next_node;

  //unlock the new head when finish
  omp_unset_lock(&next_node->lock);
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

    //lock the head
    omp_set_lock(&current->lock);

    for (int i = 0; i <= index-1; i++) {
        if (current->next == NULL) {
            return -1;
        }
        //lock the next node
        omp_set_lock(&current->next->lock);
        //if it's done unlock the current
        if(i != 0 && i != index-1){
          omp_unset_lock(&previous->lock);
        }
        previous = current;
        current = current->next;
    }

    // Current node has to be removed!
    /*
    there is no need to lock the current->next because
    if an other thread attempt to delete it, the thread
    would first lock the previous which are our current
    so we would not be able to continue. We have to wait
    for it to delete and replace it.
    */
    previous->next = current->next;
    retval = current->val;
    omp_destroy_lock(&current->lock);
    free(current);
    omp_unset_lock(&previous->lock);

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

  //lock head and next
  omp_set_lock(&previous->lock);
  omp_set_lock(&current->lock);

  while (current) {
    if (current->val == val) {
      previous->next = current->next;
      omp_destroy_lock(&current->lock);
      free(current);
      return val;
    }

    //lock next
    omp_set_lock(&current->next->lock);
    //if ok unlock previous
    omp_unset_lock(&previous->lock);
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

  //lock the head
  omp_set_lock(&current->lock);

  while (current) {
    if (current->val == val) {
      return index;
    }
    //lock the next one
    omp_set_lock(&current->next->lock);
    //if ok unlock the current and jump
    omp_unset_lock(&current->lock);
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
  omp_set_lock(&current->lock);
  while (current) {
    //lock the next one before continue
    omp_set_lock(&current->next->lock);
    next = current->next;
    //destroy before free
    omp_destroy_lock(&current->lock);
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
