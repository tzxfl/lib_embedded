#ifndef LPI_LIST_H
#define LPI_LIST_H


/*双向循环链表*/

struct list_head{
	struct list_head *prev;
	struct list_head *next;
};

//静态初始化
#define __INIT_HEAD(name) {&(name),&(name)}

#define INIT_LIST_HEAD(name) \
	struct list_head name = __INIT_HEAD(name)


/*动态初始化一个链表节点，双向循环*/
static inline void list_init_head(struct list_head *head)
{
	head->prev = head;
	head->next = head;
}

/*在任意两个节点中添加数据*/
static inline void __list_add(struct list_head *left,struct list_head *new,struct list_head *right)
{
	right->prev = new;
	new->next = right;
	new->prev = left;
	left->next = new;
}

/*在链表头后添加数据*/
static inline void list_add_head(struct list_head *head,struct list_head *new)
{
	if(head && new){
		__list_add(head,new,head->next);
	}
}

/*在链表尾部插入*/
static inline void list_add_tail(struct list_head *head,struct list_head *new)
{
	if(head && new){
		__list_add(head->prev,new,head);
	}
}

/*删除任意两个节点之间的元素*/
static inline void __list_del(struct list_head *left,struct list_head *right)
{
	left->next = right;
	right->prev = left;
}

/*删除任意元素*/
static inline void list_del_entry(struct list_head *entry)
{
	__list_del(entry->prev,entry->next);
	entry->prev = NULL;
	entry->next = NULL;
}

/*判断链表是否为空*/
static inline int list_empty(struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next = head->prev);
}

/*判断链表是否是最后一个*/
static inline int list_islast(struct list_head *head,struct list_head *entry)
{
	return (entry->next == head);
}

/*判断链表是否是第一个*/
static inline int list_isfirst(struct list_head *head,struct list_head *entry)
{
	return (entry->prev == head) && (head->next == entry);
}

/*获取链表长度*/
static inline int list_get_len(struct list_head *head)
{
	struct list_head *pos;
	int count = 0;
	for(pos=head->next;pos != head;pos=pos->next){
		count++;
	}
	return count;
}

/*获取一个成员的相对位置
 *将0强制转换为type指针类型(强制转换并不会改变数据本身的值)，获取member成员，获取member的地址
 *将这个地址转换为long型，得到的是该member的偏移地址*/
#ifndef offset_of
#define offset_of(type,member) ((long)&(((type *)0)->member))
#endif

/*ptr是指向member的指针*/
#define container_of(ptr,type,member) \
	((type *)((char *)ptr - offset_of(type,member)))

#define get_struct(ptr,type,member) \
	container_of(ptr,type,member)


/*遍历链表
 *pos:指向的数据
 *ptr:临时指针
 *head:链表头*/
#define list_for_each_safe(pos,ptr,head) \
	for(pos=(head)->next,ptr=pos->next;pos!=(head);\
		pos=ptr,ptr=pos->next)

#define list_for_each(pos,head) \
	for(pos=(head)->next;pos!=(head);\
		pos=pos->next)

/*遍历每个节点，并使用*/
#define list_for_each_entry(used,head,type,member) \
	for(used=get_struct((head)->next,type,member);\
		&(used->member)!=(head);\
		used=get_struct(used->member.next,type,member))

#endif // LPI_LIST_H
