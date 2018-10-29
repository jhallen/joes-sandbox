/* Doubly linked list management junk */

struct link {
	DSPOBJ *next;
	DSPOBJ *prev;
};

DSPOBJ *enquefront(LINK *queue, DSPOBJ *item);
DSPOBJ *enqueback(LINK *queue, DSPOBJ *item);
DSPOBJ *deque(DSPOBJ *item);
void izqueue(LINK *queue);
