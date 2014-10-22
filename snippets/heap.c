/* Classical heap */

typedef int Heap_value;
typedef int Heap_key;
typedef struct heap_item Heap_item;

struct heap_item {
	Heap_key key;
	Heap_value value;
};

typedef struct heap Heap;

struct heap {
	Heap_item *items;	/* Heap array */
	int len;		/* No. elements in items */
	int size;		/* Malloc size of items */
};

/* Create a heap with space allocated for 'prealloc' elements */

Heap *heap_init(int prealloc)
{
	Heap *heap = (Heap *)malloc(sizeof(Heap));
	heap->len = 1;
	heap->size = prealloc;
	heap->items = (Heap_item *)malloc(sizeof(Heap_item) * heap->size);
	return heap;
}

/* Insert an item into a heap */

void heap_insert(Heap *heap, Heap_key key, Heap_value value)
{
	int x;

	/* Create new item */
	Heap_item new_item;
	new_item.key = key;
	new_item.value = value;

	/* Expand heap if necessary */
	if (heap->len == heap->size) {
		heap->items = realloc(heap->items, heap->size *= 2);
	}

	/* Append new item to heap */
	heap->items[x = heap->len++] = new_item;

	/* Swap upwards until partial order property is restored */
	/* Upheap... */
	for (;;) {
		int parent = (x >> 1);
		if (heap->items[x].key > heap->items[parent].key) {
			Heap_item tmp = heap->items[x];
			heap->items[x] = heap->items[parent];
			heap->items[parent] = tmp;
			x = parent;
		} else {
			/* All done! */
			break;
		}
	}
}

/* Get key/value of max element */

Heap_value heap_get_max(Heap *heap)
{
	return heap->items[1];
}

/* Get no. items in the heap */

int heap_len(Heap *heap)
{
	return heap->len - 1;
}

/* Delete max element */

void heap_remove_max(Heap *heap)
{
	/* Move last element in heap to root */
	heap->items[1] = heap->items[--heap->len];

	/* Downheap... */
	for (;;) {
		int left = (x << 1);
		int right = left + 1;
		if (left < heap->len && heap->items[left].key > heap->item[x].key) {
			/* Swap with left */
			Heap_item tmp = heap->items[x];
			heap->items[x] = heap->items[left];
			heap->items[left] = tmp;
			x = left;
		} else if (right < heap->len && heap->items[right].key > heap->items[x].key) {
			/* Swap with right */
			Heap_item tmp = heap->items[x];
			heap->items[x] = heap->items[right];
			heap->items[right] = tmp;
			x = right;
		} else {
			/* All done! */
			break;
		}
	}
}
