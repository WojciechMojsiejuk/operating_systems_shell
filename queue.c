#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

void init(struct Queue *q) {
	q->front = NULL;
	q->last = NULL;
	q->size = 0;
}

char* front(struct Queue *q) {
  if(q->front != NULL)
  {
  	return q->front->data;
  }
  return NULL;
}

char* last(struct Queue *q)
{
  if(q->last != NULL)
  {
  	return q->last->data;
  }
  return NULL;
}

void pop(struct Queue *q) {
	q->size--;

	struct Node *tmp = q->front;
	q->front = q->front->next;
	free(tmp);
}

void push(struct Queue *q, char* data) {
	q->size++;

	if (q->front == NULL) {
		q->front = (struct Node *) malloc(sizeof(struct Node));
		q->front->data = data;
		q->front->next = NULL;
		q->last = q->front;
	} else {
		q->last->next = (struct Node *) malloc(sizeof(struct Node));
		q->last->next->data = data;
		q->last->next->next = NULL;
		q->last = q->last->next;
	}
}

void print_queue(struct Queue *q)
{
  struct Node *temp = q->front;
  while(temp !=NULL)
  {
      printf("%s\n",temp->data);
      temp=temp->next;
  }
}

int current_queue_size(struct Queue *q)
{
  return q->size;
}
