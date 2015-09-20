#include "types.h"
#include "queue.h"

LINK *enquefront(LINK *queue, LINK *item)
{
	item->next = queue->next;
	item->prev = queue;
	queue->next->prev = item;
	queue->next = item;
	return item;
}

LINK *enqueback(LINK *queue, LINK *item)
{
	item->next = queue;
	item->prev = queue->prev;
	queue->prev->next = item;
	queue->prev = item;
	return item;
}

LINK *deque(LINK *item)
{
	item->next->prev = item->prev;
	item->prev->next = item->next;
	return item;
}

LINK *izqueue(LINK *queue)
{
	queue->next = queue;
	queue->prev = queue;
	return queue;
}
