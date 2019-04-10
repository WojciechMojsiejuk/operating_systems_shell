#include <stdio.h>
#include <stdlib.h>

struct Node {
	char* data;
	struct Node *next;
};

struct Queue {
	struct Node *front;
	struct Node *last;
	unsigned int size;
};

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

// Driver code.
int main(void)
{
	struct Queue q;
	init(&q);
  char* napis1 = "abc";
  char* napis2 = "abcd";
  char* napis3 = "bcd";
  char* napis4 = "aaa";
	push(&q, napis1);
  push(&q, napis2);
  push(&q, napis3);
  push(&q, napis4);
  pop(&q);
  print_queue(&q);
  printf("%d",current_queue_size(&q));
}
