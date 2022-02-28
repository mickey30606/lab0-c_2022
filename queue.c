#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

#define IS_NULL_POINTER(p) \
    do {                   \
        if (!p)            \
            return 0;      \
    } while (0);

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(*new));
    if (!new)
        return NULL;

    INIT_LIST_HEAD(new);

    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    if (l->next == l) {
        free(l);
        return;
    }
    element_t *i, *safe;
    list_for_each_entry_safe (i, safe, l, list) {
        free(i->value);
        free(i);
    }
    free(l);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(struct list_head *head, char *s)
{
    // check
    IS_NULL_POINTER(head);
    IS_NULL_POINTER(s);
    // new node
    element_t *new_node = malloc(sizeof(*new_node));
    IS_NULL_POINTER(new_node);
    // copy string, need to malloc array
    int len = strlen(s);
    new_node->value = malloc(len + 1);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    for (int i = 0; i < len; i++) {
        new_node->value[i] = s[i];
    }
    new_node->value[len] = '\0';
    // add to list
    list_add(&new_node->list, head);

    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(struct list_head *head, char *s)
{
    // check
    IS_NULL_POINTER(head);
    IS_NULL_POINTER(s);
    // new node
    element_t *new_node = malloc(sizeof(*new_node));
    IS_NULL_POINTER(new_node);
    // copy string
    int len = strlen(s);
    new_node->value = malloc(len + 1);
    if (!new_node->value) {
        free(new_node);
        return false;
    }
    for (int i = 0; i <= len; i++) {
        new_node->value[i] = s[i];
    }
    new_node->value[len] = '\0';
    // add to list
    list_add_tail(&new_node->list, head);
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return target element.
 * Return NULL if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 *
 * NOTE: "remove" is different from "delete"
 * The space used by the list element and the string should not be freed.
 * The only thing "remove" need to do is unlink it.
 *
 * REF:
 * https://english.stackexchange.com/questions/52508/difference-between-delete-and-remove
 */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // check head is NULL or queue is empty
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;
    // unlink first node
    element_t *remove = container_of(head->next, element_t, list);
    list_del_init(head->next);
    // copy string to sp
    if (sp) {
        char *s = remove->value;
        int len = strlen(s);
        if (len > bufsize - 1) {
            len = bufsize - 1;
        }
        for (int i = 0; i <= len; i++) {
            sp[i] = s[i];
        }
        sp[bufsize - 1] = '\0';
    }
    return remove;
}

/*
 * Attempt to remove element from tail of queue.
 * Other attribute is as same as q_remove_head.
 */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // check
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;
    // return q_remove_head(head->prev->prev, sp, bufsize);

    element_t *remove = container_of(head->prev, element_t, list);
    list_del_init(head->prev);

    if (sp) {
        char *s = remove->value;
        int len = strlen(s);
        if (len > bufsize - 1) {
            len = bufsize - 1;
        }
        for (int i = 0; i <= len; i++) {
            sp[i] = s[i];
        }
        sp[bufsize - 1] = '\0';
    }

    return remove;
}

/*
 * WARN: This is for external usage, don't modify it
 * Attempt to release element.
 */
void q_release_element(element_t *e)
{
    free(e->value);
    free(e);
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(struct list_head *head)
{
    IS_NULL_POINTER(head);
    if (list_empty(head))
        return false;

    int size = 0;
    struct list_head *i;
    list_for_each (i, head) {
        size++;
    }
    return size;
}

/*
 * Delete the middle node in list.
 * The middle node of a linked list of size n is the
 * ⌊n / 2⌋th node from the start using 0-based indexing.
 * If there're six element, the third member should be return.
 * Return true if successful.
 * Return false if list is NULL or empty.
 */
bool q_delete_mid(struct list_head *head)
{
    if (!head)
        return NULL;
    if (list_empty(head))
        return NULL;
    struct list_head *i;
    struct list_head *i_doubleSpeed = head->next;
    list_for_each (i, head) {
        if (i_doubleSpeed == head || i_doubleSpeed->next == head)
            break;
        i_doubleSpeed = i_doubleSpeed->next->next;
    }
    element_t *remove = list_entry(i, element_t, list);
    list_del(i);
    q_release_element(remove);
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    return true;
}

/*
 * Delete all nodes that have duplicate string,
 * leaving only distinct strings from the original list.
 * Return true if successful.
 * Return false if list is NULL.
 *
 * Note: this function always be called after sorting, in other words,
 * list is guaranteed to be sorted in ascending order.
 */
bool q_delete_dup(struct list_head *head)
{
    // Maybe have bugs
    if (!head)
        return false;

    element_t *i, *safe;
    char *compare = NULL;
    list_for_each_entry_safe (i, safe, head, list) {
        if (&(safe->list) != head) {
            if (strcmp(i->value,
                       (list_entry(i->list.next, element_t, list))->value) ==
                0) {
                // free i
                compare = safe->value;
                list_del(&i->list);
                q_release_element(i);
            } else {
                if (compare) {
                    compare = NULL;
                    list_del(&i->list);
                    q_release_element(i);
                    continue;
                }
            }
        } else {
            if (compare) {
                compare = NULL;
                list_del(&i->list);
                q_release_element(i);
                continue;
            }
        }
    }

    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/*
 * Attempt to swap every two adjacent nodes.
 */
void q_swap(struct list_head *head)
{
    // maybe have bugs!
    if (!head)
        return;
    if (list_empty(head))
        return;
    struct list_head *prev = head;
    while (prev->next != head && prev->next->next != head) {
        list_move(prev->next->next, prev);
        prev = prev->next->next;
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head))
        return;
    if (list_is_singular(head))
        return;
    struct list_head *i, *safe, *tmp;
    list_for_each_safe (i, safe, head) {
        tmp = i->next;
        i->next = i->prev;
        i->prev = tmp;
    }
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
    return;
}

struct list_head *merge(struct list_head *a, struct list_head *b)
{
    if (a == NULL)
        return b;
    if (b == NULL)
        return a;

    struct list_head *head;
    if (strcmp((list_entry(a, element_t, list)->value),
               (list_entry(b, element_t, list)->value)) <= 0) {
        head = a;
        a = a->next;
    } else {
        head = b;
        b = b->next;
    }

    struct list_head **p = &(head->next);
    while (a != NULL) {
        if (b == NULL) {
            b = a;
            break;
        }
        if (strcmp((list_entry(a, element_t, list)->value),
                   (list_entry(b, element_t, list)->value)) <= 0) {
            *p = a;
            p = &((*p)->next);
            a = a->next;
        } else {
            *p = b;
            p = &((*p)->next);
            b = b->next;
        }
    }

    *p = b;
    return head;
}

struct list_head *merge_sort(struct list_head *first)
{
    if (first == NULL)
        return NULL;
    if (first->next == NULL)
        return first;

    struct list_head *p_speed = first, *p_doublespeed = first->next;
    for (; p_doublespeed != NULL && p_doublespeed->next != NULL;
         p_speed = p_speed->next, p_doublespeed = p_doublespeed->next->next)
        ;
    p_doublespeed = p_speed->next;
    p_speed->next = NULL;
    p_speed = merge_sort(first);
    p_doublespeed = merge_sort(p_doublespeed);
    return merge(p_speed, p_doublespeed);
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(struct list_head *head)
{
    // check head
    if (!head)
        return;
    if (list_empty(head))
        return;
    if (list_is_singular(head))
        return;

    head->prev->next = NULL;
    head->next = merge_sort(head->next);

    // rebuild
    struct list_head *p = head;
    for (; p->next != NULL; p = p->next) {
        p->next->prev = p;
    }
    p->next = head;
    head->prev = p;
}
