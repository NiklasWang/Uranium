#ifndef __Clist_H_
#define __Clist_H_

namespace sirius {

#define member_of(ptr, type, member) ({ \
  const decltype(((type *)0)->member) *__mptr = (ptr); \
  (type *)((char *)__mptr - offsetof(type,member));})

struct clist {
  struct clist *next, *prev;
};

static inline void clist_init(struct clist *ptr)
{
  ptr->next = ptr;
  ptr->prev = ptr;
}

static inline void clist_add_tail_node(struct clist *item,
  struct clist *head)
{
  struct clist *prev = head->prev;

  head->prev = item;
  item->next = head;
  item->prev = prev;
  prev->next = item;
}

static inline void clist_insert_before_node(struct clist *item,
  struct clist *node)
{
  item->next = node;
  item->prev = node->prev;
  item->prev->next = item;
  node->prev = item;
}

static inline void clist_del_node(struct clist *ptr)
{
  struct clist *prev = ptr->prev;
  struct clist *next = ptr->next;

  next->prev = ptr->prev;
  prev->next = ptr->next;
  ptr->next = ptr;
  ptr->prev = ptr;
}

};

#endif
