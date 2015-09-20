/* Doubly linked list management junk */

struct link {
	LINK *next;
	LINK *prev;
};

LINK *enquefront(LINK *queue, LINK *item);
LINK *enqueback(LINK *queue, LINK *item);
LINK *deque(LINK *item);
LINK *izqueue(LINK *queue);
