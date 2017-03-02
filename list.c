#include <stdio.h>


struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

struct my_list {
	int data;
	struct list_head head;
};

/*#define LIST_HEAD_INIT(name) { &(name), &(name) }*/

/*#define LIST_HEAD(name) \*/
		/*struct list_head name = LIST_HEAD_INIT(name)*/

#define INIT_LIST_HEAD(ptr) do { \
		(ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

static void __list_add(struct list_head *new, struct list_head *prev,
		       struct list_head *next)
{
	printf("%s %d add new %lx to %lx %lx\n",__func__, __LINE__, new, prev, next);
	next->prev = new;	
	prev->next = new;
	new->next = next;
	new->prev = prev;
}

static void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

static void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

static void __list_del(struct list_head *prev, struct list_head *next)
{
	prev->next = next;
	next->prev = prev;
}

static void list_del(struct list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

#define list_entry(ptr, type, member) \ 
    ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member))) 

#define list_for_each(pos, head) \
			for (pos = (head)->next; pos != (head); pos = pos->next)
int main(void)
{
	struct my_list ma;
	struct my_list mb;
	struct my_list mc;
	struct my_list md;
	struct my_list me;
	struct my_list mf;
	ma.data = 1;
	mb.data = 2;
	mc.data = 3;
	md.data = 4;
	me.data = 5;
	mf.data = 6;
	
	struct list_head list;
	INIT_LIST_HEAD(&list);
	list_add_tail(&(ma.head), &list);
	list_add_tail(&(mb.head), &list);
	list_add_tail(&(mc.head), &list);
	list_add_tail(&(md.head), &list);
	list_add_tail(&(me.head), &list);
	list_add_tail(&(mf.head), &list);

	struct list_head *entry;
	list_for_each(entry, &list) {
	for (entry = &(ma.head); entry != (&(ma.head))->prev; entry = (&(ma.head))->next) {
		printf("%lx \n", entry);
		struct my_list *m = list_entry(entry, struct my_list, head);
		printf("%d\n", m->data);
	}
	return 0;
}
