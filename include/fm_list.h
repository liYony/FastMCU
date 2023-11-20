#ifndef __FM_LIST_H__
#define __FM_LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "fm_type.h"

struct list_head
{
    struct list_head *next, *prev;
};

#define WRITE_ONCE(var, val) (var) = (val)

#define LIST_HEAD_INIT(name) \
    {                        \
        &(name), &(name)     \
    }

#define LIST_HEAD(name) struct list_head name = LIST_HEAD_INIT(name)

#define INIT_LIST_HEAD(ptr)  \
    do                       \
    {                        \
        (ptr)->next = (ptr); \
        (ptr)->prev = (ptr); \
    } while (0)

/**
 * list_entry - get the struct for this entry
 * @ptr: the &struct list_head pointer.
 * @type: the type of the struct this is embedded in.
 * @member: the name of the list_struct within the struct.
 */
#define list_entry(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
__STATIC_INLINE void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next)
{
    next->prev = new;
    new->next  = next;
    new->prev  = prev;
    WRITE_ONCE(prev->next, new);
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
__STATIC_INLINE void list_add(struct list_head *new, struct list_head *head)
{
    __list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
__STATIC_INLINE void list_add_tail(struct list_head *new, struct list_head *head)
{
    __list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
__STATIC_INLINE void __list_del(struct list_head *prev, struct list_head *next)
{
    next->prev = prev;
    WRITE_ONCE(prev->next, next);
}

/**
 * list_is_first -- tests whether @list is the first entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
__STATIC_INLINE int list_is_first(const struct list_head *list, const struct list_head *head)
{
    return list->prev == head;
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
__STATIC_INLINE int list_is_last(const struct list_head *list, const struct list_head *head)
{
    return list->next == head;
}

/**
 * list_is_head - tests whether @list is the list @head
 * @list: the entry to test
 * @head: the head of the list
 */
__STATIC_INLINE int list_is_head(const struct list_head *list, const struct list_head *head)
{
    return list == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
__STATIC_INLINE int list_empty(const struct list_head *head)
{
    return head->next == head;
}

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each(pos, head) \
    for (pos = (head)->next; !list_is_head(pos, (head)); pos = pos->next)

/**
 * list_for_each_continue - continue iteration over a list
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 *
 * Continue to iterate over a list, continuing after the current position.
 */
#define list_for_each_continue(pos, head) \
    for (pos = pos->next; !list_is_head(pos, (head)); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.
 * @head:	the head for your list.
 */
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; !list_is_head(pos, (head)); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; !list_is_head(pos, (head)); pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.
 * @n:		another &struct list_head to use as temporary storage
 * @head:	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; !list_is_head(pos, (head)); pos = n, n = pos->prev)

/**
 * list_count_nodes - count nodes in the list
 * @head:	the head for your list.
 */
__STATIC_INLINE uint32_t list_count_nodes(struct list_head *head)
{
    struct list_head *pos;
    uint32_t          count = 0;

    list_for_each(pos, head) count++;

    return count;
}

//================================================================================================//
//================================================================================================//

struct slist_head
{
    struct slist_head *next;
};

/**
 * @brief initialize a single list
 *
 * @param l the single list to be initialized
 */
__STATIC_INLINE void slist_init(struct slist_head *l)
{
    l->next = NULL;
}

__STATIC_INLINE void slist_append(struct slist_head *l, struct slist_head *n)
{
    struct slist_head  *node;

    node = l;
    while (node->next) node = node->next;

    /* append the node to the tail */
    node->next = n;
    n->next = NULL;
}

__STATIC_INLINE void slist_insert(struct slist_head *l, struct slist_head *n)
{
    n->next = l->next;
    l->next = n;
}

__STATIC_INLINE unsigned int slist_len(const struct slist_head *l)
{
    unsigned int len = 0;
    const struct slist_head *list = l->next;
    while (list != NULL)
    {
        list = list->next;
        len ++;
    }

    return len;
}

__STATIC_INLINE struct slist_head *slist_remove(struct slist_head *l, struct slist_head *n)
{
    /* remove slist head */
    struct slist_head  *node = l;
    while (node->next && node->next != n) node = node->next;

    /* remove node */
    if (node->next != (struct slist_head *)0) node->next = node->next->next;

    return l;
}

__STATIC_INLINE struct slist_head *slist_first(struct slist_head *l)
{
    return l->next;
}

__STATIC_INLINE struct slist_head *slist_tail(struct slist_head *l)
{
    while (l->next) l = l->next;

    return l;
}

__STATIC_INLINE struct slist_head *slist_next(struct slist_head *n)
{
    return n->next;
}

__STATIC_INLINE int slist_isempty(struct slist_head *l)
{
    return l->next == NULL;
}

/**
 * @brief get the struct for this single list node
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define slist_entry(node, type, member) \
    fm_container_of(node, type, member)

/**
 * slist_for_each - iterate over a single list
 * @param pos the struct slist_head * to use as a loop cursor.
 * @param head the head for your single list.
 */
#define slist_for_each(pos, head) \
    for (pos = (head)->next; pos != NULL; pos = pos->next)

/**
 * slist_for_each_entry  -   iterate over single list of given type
 * @param pos the type * to use as a loop cursor.
 * @param head the head for your single list.
 * @param member the name of the list_struct within the struct.
 */
#define slist_for_each_entry(pos, head, member) \
    for (pos = slist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (NULL); \
         pos = slist_entry(pos->member.next, typeof(*pos), member))

/**
 * slist_first_entry - get the first element from a slist
 * @param ptr the slist head to take the element from.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define slist_first_entry(ptr, type, member) \
    slist_entry((ptr)->next, type, member)

/**
 * slist_tail_entry - get the tail element from a slist
 * @param ptr the slist head to take the element from.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define slist_tail_entry(ptr, type, member) \
    slist_entry(slist_tail(ptr), type, member)

#ifdef __cplusplus
}
#endif

#endif
