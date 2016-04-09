/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.

  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{

  q->compare = comparer;
  q->size = 0;
  q->front = malloc(sizeof(node_t));
  q->front->value = NULL;
  q->front->next = NULL;

}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{

  int index = 0;


  if(priqueue_size(q) == 0){
    q->front->value = ptr;
    q->size++;
    return index;
  }else{

    node_t *temp = q->front;
    int flag = 0;

    if(q->compare(ptr, temp->value) <= 0){
      q->front = malloc(sizeof(node_t));
      q->front->value = ptr;
      q->front->next = temp;
      flag = 1;
    }

    while(flag == 0){

      index++;

      if(temp->next == NULL){

        temp->next = malloc(sizeof(node_t));
        temp->next->value = ptr;
        temp->next->next = NULL;

        flag = 1;

      }else if(q->compare(ptr, temp->next->value) <= 0){

        node_t *temp2 = temp->next;

        temp->next = malloc(sizeof(node_t));
        temp->next->value = ptr;
        temp->next->next = temp2;

        flag = 1;

      }else{

        if(temp == NULL){
          flag = 1;
          return -1;
        }

        temp = temp->next;

      }

    }

    q->size++;
    return index;
  }

  return -1;
}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{

  if(q->size == 0){
    return NULL;
  }else{
    return q->front->value;
  }

}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{

  if(q->size == 0){
    return NULL;
  }else{

    int *value = q->front->value;
    node_t *temp = q->front->next;

    free(q->front);

    q->front = temp;

    q->size--;

    return value;

  }

}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{

  if(index >= q->size){
    return NULL;
  }else{

    node_t *temp = q->front;

    int x;
    for(x = 0; x < index; x++){
      temp = temp->next;
    }

    return temp->value;

  }

}


/**
  Removes all instances of ptr from the queue.

  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{

  node_t *temp = q->front;
  int count = 0;
  int flag = 0;

  while(temp->value == ptr){
    temp = q->front->next;
    free(q->front);
    q->front = temp;
    count++;
    q->size--;
    flag = 1;
  }

  if(flag == 1){
    return count;
  }

  int x;

  for(x = 0; x < q->size - 1; x++){

    if(temp->next->value == ptr){

      node_t *temp2 = temp->next->next;
      free(temp->next);
      temp->next = temp2;

      count++;
      q->size--;

    }else{
      temp = temp->next;
    }

  }

  return count;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.

  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{

  if(index >= q->size || index < 0){
    return NULL;
  }else{

    node_t *temp = q->front;

    if(index == 0){
      temp = temp->next;
      free(q->front);
      q->front = temp;
      q->size--;
      return 0;
    }else{

      int x;
      for(x = 0; x < index - 1; x++){
        temp = temp->next;
      }

      node_t *temp2 = temp->next->next;
      free(temp->next);
      temp->next = temp2;

      q->size--;

    }


  }

  return 0;
}


/**
  Returns the number of elements in the queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
  return q->size;
}


/**
  Destroys and frees all the memory associated with q.

  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{

  node_t *temp = q->front;
  node_t *tempNext;

  while(temp != NULL){
    tempNext = temp->next;
    free(temp);
    q->size--;
    temp = tempNext;
  }

  q->front = NULL;


}
