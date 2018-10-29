#include <stddef.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "types.h"
#include "queue.h"
#include "xjunk.h"
#include "display.h"

DSPOBJ *enquefront(LINK *queue, DSPOBJ *item)
{
	item->link.next = queue->next;
	item->link.prev = (DSPOBJ *)queue;
	queue->next->link.prev = item;
	queue->next = item;
	return item;
}

DSPOBJ *enqueback(LINK *queue, DSPOBJ *item)
{
	item->link.next = (DSPOBJ *)queue;
	item->link.prev = queue->prev;
	queue->prev->link.next = item;
	queue->prev = item;
	return item;
}

DSPOBJ *deque(DSPOBJ *item)
{
	item->link.next->link.prev = item->link.prev;
	item->link.prev->link.next = item->link.next;
	return item;
}

void izqueue(LINK *queue)
{
	queue->next = (DSPOBJ *)queue;
	queue->prev = (DSPOBJ *)queue;
}
