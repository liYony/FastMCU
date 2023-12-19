/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-16     Bernard      the first version
 * 2006-09-07     Bernard      move the kservice APIs to rtthread.h
 * 2007-06-27     Bernard      fix the rt_list_remove bug
 * 2012-03-22     Bernard      rename kservice.h to rtservice.h
 * 2017-11-15     JasonJia     Modify rt_slist_foreach to rt_slist_for_each_entry.
 *                             Make code cleanup.
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#ifndef __FM_SERVICE_H__
#define __FM_SERVICE_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup KernelService
 */

/**@{*/

/**
 * fm_container_of - return the start address of struct type, while ptr is the
 * member of struct type.
 */
#define fm_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))


/**
 * @brief initialize a list object
 */
#define FM_LIST_OBJECT_INIT(object) { &(object), &(object) }

/**
 * @brief initialize a list
 *
 * @param l list to be initialized
 */
fm_inline void fm_list_init(fm_list_t *l)
{
    l->next = l->prev = l;
}

/**
 * @brief insert a node after a list
 *
 * @param l list to insert it
 * @param n new node to be inserted
 */
fm_inline void fm_list_insert_after(fm_list_t *l, fm_list_t *n)
{
    l->next->prev = n;
    n->next = l->next;

    l->next = n;
    n->prev = l;
}

/**
 * @brief insert a node before a list
 *
 * @param n new node to be inserted
 * @param l list to insert it
 */
fm_inline void fm_list_insert_before(fm_list_t *l, fm_list_t *n)
{
    l->prev->next = n;
    n->prev = l->prev;

    l->prev = n;
    n->next = l;
}

/**
 * @brief remove node from list.
 * @param n the node to remove from the list.
 */
fm_inline void fm_list_remove(fm_list_t *n)
{
    n->next->prev = n->prev;
    n->prev->next = n->next;

    n->next = n->prev = n;
}

/**
 * @brief tests whether a list is empty
 * @param l the list to test.
 */
fm_inline int fm_list_isempty(const fm_list_t *l)
{
    return l->next == l;
}

/**
 * @brief get the list length
 * @param l the list to get.
 */
fm_inline unsigned int fm_list_len(const fm_list_t *l)
{
    unsigned int len = 0;
    const fm_list_t *p = l;
    while (p->next != l)
    {
        p = p->next;
        len ++;
    }

    return len;
}

/**
 * @brief get the struct for this entry
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define fm_list_entry(node, type, member) \
    fm_container_of(node, type, member)

/**
 * fm_list_for_each - iterate over a list
 * @param pos the fm_list_t * to use as a loop cursor.
 * @param head the head for your list.
 */
#define fm_list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * fm_list_for_each_safe - iterate over a list safe against removal of list entry
 * @param pos the fm_list_t * to use as a loop cursor.
 * @param n another fm_list_t * to use as temporary storage
 * @param head the head for your list.
 */
#define fm_list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

/**
 * fm_list_for_each_entry  -   iterate over list of given type
 * @param pos the type * to use as a loop cursor.
 * @param head the head for your list.
 * @param member the name of the list_struct within the struct.
 */
#define fm_list_for_each_entry(pos, head, member) \
    for (pos = fm_list_entry((head)->next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = fm_list_entry(pos->member.next, typeof(*pos), member))

/**
 * fm_list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @param pos the type * to use as a loop cursor.
 * @param n another type * to use as temporary storage
 * @param head the head for your list.
 * @param member the name of the list_struct within the struct.
 */
#define fm_list_for_each_entry_safe(pos, n, head, member) \
    for (pos = fm_list_entry((head)->next, typeof(*pos), member), \
         n = fm_list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, n = fm_list_entry(n->member.next, typeof(*n), member))

/**
 * fm_list_first_entry - get the first element from a list
 * @param ptr the list head to take the element from.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the list_struct within the struct.
 *
 * Note, that list is expected to be not empty.
 */
#define fm_list_first_entry(ptr, type, member) \
    fm_list_entry((ptr)->next, type, member)

#define FM_SLIST_OBJECT_INIT(object) { FM_NULL }

/**
 * @brief initialize a single list
 *
 * @param l the single list to be initialized
 */
fm_inline void fm_slist_init(fm_slist_t *l)
{
    l->next = FM_NULL;
}

fm_inline void fm_slist_append(fm_slist_t *l, fm_slist_t *n)
{
    struct fm_slist_node *node;

    node = l;
    while (node->next) node = node->next;

    /* append the node to the tail */
    node->next = n;
    n->next = FM_NULL;
}

fm_inline void fm_slist_insert(fm_slist_t *l, fm_slist_t *n)
{
    n->next = l->next;
    l->next = n;
}

fm_inline unsigned int fm_slist_len(const fm_slist_t *l)
{
    unsigned int len = 0;
    const fm_slist_t *list = l->next;
    while (list != FM_NULL)
    {
        list = list->next;
        len ++;
    }

    return len;
}

fm_inline fm_slist_t *fm_slist_remove(fm_slist_t *l, fm_slist_t *n)
{
    /* remove slist head */
    struct fm_slist_node *node = l;
    while (node->next && node->next != n) node = node->next;

    /* remove node */
    if (node->next != (fm_slist_t *)0) node->next = node->next->next;

    return l;
}

fm_inline fm_slist_t *fm_slist_first(fm_slist_t *l)
{
    return l->next;
}

fm_inline fm_slist_t *fm_slist_tail(fm_slist_t *l)
{
    while (l->next) l = l->next;

    return l;
}

fm_inline fm_slist_t *fm_slist_next(fm_slist_t *n)
{
    return n->next;
}

fm_inline int fm_slist_isempty(fm_slist_t *l)
{
    return l->next == FM_NULL;
}

/**
 * @brief get the struct for this single list node
 * @param node the entry point
 * @param type the type of structure
 * @param member the name of list in structure
 */
#define fm_slist_entry(node, type, member) \
    fm_container_of(node, type, member)

/**
 * fm_slist_for_each - iterate over a single list
 * @param pos the fm_slist_t * to use as a loop cursor.
 * @param head the head for your single list.
 */
#define fm_slist_for_each(pos, head) \
    for (pos = (head)->next; pos != FM_NULL; pos = pos->next)

/**
 * fm_slist_for_each_entry  -   iterate over single list of given type
 * @param pos the type * to use as a loop cursor.
 * @param head the head for your single list.
 * @param member the name of the list_struct within the struct.
 */
#define fm_slist_for_each_entry(pos, head, member) \
    for (pos = fm_slist_entry((head)->next, typeof(*pos), member); \
         &pos->member != (FM_NULL); \
         pos = fm_slist_entry(pos->member.next, typeof(*pos), member))

/**
 * fm_slist_first_entry - get the first element from a slist
 * @param ptr the slist head to take the element from.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define fm_slist_first_entry(ptr, type, member) \
    fm_slist_entry((ptr)->next, type, member)

/**
 * fm_slist_tail_entry - get the tail element from a slist
 * @param ptr the slist head to take the element from.
 * @param type the type of the struct this is embedded in.
 * @param member the name of the slist_struct within the struct.
 *
 * Note, that slist is expected to be not empty.
 */
#define fm_slist_tail_entry(ptr, type, member) \
    fm_slist_entry(fm_slist_tail(ptr), type, member)

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
