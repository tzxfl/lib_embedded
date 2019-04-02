#ifndef LPI_LIST_H
#define LPI_LIST_H


/*˫��ѭ������*/

struct list_head{
	struct list_head *prev;
	struct list_head *next;
};

//��̬��ʼ��
#define __INIT_HEAD(name) {&(name),&(name)}

#define INIT_LIST_HEAD(name) \
	struct list_head name = __INIT_HEAD(name)


/*��̬��ʼ��һ������ڵ㣬˫��ѭ��*/
static inline void list_init_head(struct list_head *head)
{
	head->prev = head;
	head->next = head;
}

/*�����������ڵ����������*/
static inline void __list_add(struct list_head *left,struct list_head *new,struct list_head *right)
{
	right->prev = new;
	new->next = right;
	new->prev = left;
	left->next = new;
}

/*������ͷ���������*/
static inline void list_add_head(struct list_head *head,struct list_head *new)
{
	if(head && new){
		__list_add(head,new,head->next);
	}
}

/*������β������*/
static inline void list_add_tail(struct list_head *head,struct list_head *new)
{
	if(head && new){
		__list_add(head->prev,new,head);
	}
}

/*ɾ�����������ڵ�֮���Ԫ��*/
static inline void __list_del(struct list_head *left,struct list_head *right)
{
	left->next = right;
	right->prev = left;
}

/*ɾ������Ԫ��*/
static inline void list_del_entry(struct list_head *entry)
{
	__list_del(entry->prev,entry->next);
	entry->prev = NULL;
	entry->next = NULL;
}

/*�ж������Ƿ�Ϊ��*/
static inline int list_empty(struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next = head->prev);
}

/*�ж������Ƿ������һ��*/
static inline int list_islast(struct list_head *head,struct list_head *entry)
{
	return (entry->next == head);
}

/*�ж������Ƿ��ǵ�һ��*/
static inline int list_isfirst(struct list_head *head,struct list_head *entry)
{
	return (entry->prev == head) && (head->next == entry);
}

/*��ȡ������*/
static inline int list_get_len(struct list_head *head)
{
	struct list_head *pos;
	int count = 0;
	for(pos=head->next;pos != head;pos=pos->next){
		count++;
	}
	return count;
}

/*��ȡһ����Ա�����λ��
 *��0ǿ��ת��Ϊtypeָ������(ǿ��ת��������ı����ݱ����ֵ)����ȡmember��Ա����ȡmember�ĵ�ַ
 *�������ַת��Ϊlong�ͣ��õ����Ǹ�member��ƫ�Ƶ�ַ*/
#ifndef offset_of
#define offset_of(type,member) ((long)&(((type *)0)->member))
#endif

/*ptr��ָ��member��ָ��*/
#define container_of(ptr,type,member) \
	((type *)((char *)ptr - offset_of(type,member)))

#define get_struct(ptr,type,member) \
	container_of(ptr,type,member)


/*��������
 *pos:ָ�������
 *ptr:��ʱָ��
 *head:����ͷ*/
#define list_for_each_safe(pos,ptr,head) \
	for(pos=(head)->next,ptr=pos->next;pos!=(head);\
		pos=ptr,ptr=pos->next)

#define list_for_each(pos,head) \
	for(pos=(head)->next;pos!=(head);\
		pos=pos->next)

/*����ÿ���ڵ㣬��ʹ��*/
#define list_for_each_entry(used,head,type,member) \
	for(used=get_struct((head)->next,type,member);\
		&(used->member)!=(head);\
		used=get_struct(used->member.next,type,member))

#endif // LPI_LIST_H
