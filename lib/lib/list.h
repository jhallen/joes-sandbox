#define List_link(type_t) struct { \
	type_t *next; \
}

#define List_base(type_t) struct { \
	type_t *first; \
	type_t *last; \
}

#define iz_list(base_p) do { \
	base_p->first = base_p->last = 0; \
} while (0)

#define list_first(base_p) (base_p)->first

#define list_last(base_p) (base_p)->last

#define list_next(item_p) (item_p)->link.next

#define list_add(base_p, item_p) do { \
	item_p->link.next = 0; \
	if (base_p->first) { \
		base_p->last->link.next = item_p; \
		base_p->last = item_p; \
	} else { \
		base_p->first = base_p->last = item_p; \
	} \
} while (0)

#define list_add_list(base_p, list_p) do { \
	if (base_p->first) { \
		if (list_p->first) { \
			base_p->last->link.next = list_p->first; \
			base_p->last = list_p->last; \
			list_p->first = list_p->last = 0; \
		} \
	} else { \
		*base_p = *list_p; \
		list_p->first = list_p->last = 0; \
	} \
} while (0)

#define list_push(base_p, item_p) do { \
	item_p->link.next = base_p->first; \
	if (base_p->first) { \
		base_p->first = item_p; \
	} else { \
		base_p->first = base_p->last = item_p; \
	} \
} while (0)

#define list_push_list(base_p, list_p) do { \
	if (base_p->first) { \
		if (list_p->first) { \
			list_p->last->link.next = base_p->first; \
			base_p->first = list_p->first; \
			list_p->first = list_p->last = 0; \
		} \
	} else { \
		*base_p = *list_p; \
		list_p->first = list_p->last = 0; \
	} \
} while (0)

#define list_pop(base_p) do { \
	if (base_p->first == base_p->last) \
		base_p->last = 0; \
	base_p->first = base_p->first.link.next; \
} while(0)

#define list_reverse(type_t, base_p) do { \
	type_t *first = 0, *next, *i; \
	base_p->last = base_p->first; \
	for (i = base_p->first; i; i = next) { \
		next = i->link.next; \
		i->link.next = first; \
		first = i; \
	} \
	base_p->first = first; \
} while(0)
