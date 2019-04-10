#ifndef QUEUE_H
	#define QUEUE_H
	struct Node 
	{
	char* data;
	struct Node *next;
	};

	struct Queue 
	{
	struct Node *front;
	struct Node *last;
	unsigned int size;
	};

	void init(struct Queue *q);

	char* front(struct Queue *q);

	void pop(struct Queue *q);

	void push(struct Queue *q, char* data);

	void print_queue(struct Queue *q);

	int current_queue_size(struct Queue *q);
#endif
