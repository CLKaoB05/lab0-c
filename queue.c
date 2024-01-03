#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list)
        q_release_element(entry);
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    } else {
        element_t *element = malloc(sizeof(element_t));
        if (!element)
            return false;
        else {
            element->value = strdup(s);
            if (!element->value) {
                free(element);
                return false;
            } else {
                list_add(&element->list, head);
                return true;
            }
        }
    }
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head) {
        return false;
    } else {
        element_t *element = malloc(sizeof(element_t));
        if (!element)
            return false;
        else {
            element->value = strdup(s);
            if (!element->value) {
                free(element);
                return false;
            } else {
                list_add_tail(&element->list, head);
                return true;
            }
        }
    }
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *element = list_first_entry(head, element_t, list);
    if (sp) {
        char *c = element->value;
        while (*c && bufsize > 1) {
            *sp = *c;
            sp++;
            c++;
            bufsize--;
        }
        *sp = '\0';
    }
    list_del(&element->list);
    return element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return -1;
    int size = 0;
    element_t *entry;
    list_for_each_entry (entry, head, list)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head **indirect = &(head->next);
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        indirect = &(*indirect)->next;
    }
    element_t *element = list_entry(*indirect, element_t, list);
    list_del_init(&(element->list));
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head)
        return false;
    element_t *prev = NULL;
    element_t *entry;
    element_t *safe;
    bool same = false;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (prev && !strcmp(prev->value, entry->value)) {
            same = true;
            list_del_init(&(prev->list));
            q_release_element(prev);
        } else if (same) {
            list_del_init(&(prev->list));
            q_release_element(prev);
            same = false;
        }
        prev = entry;
    }
    if (same) {
        list_del_init(&(prev->list));
        q_release_element(prev);
    }
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node;
    list_for_each (node, head) {
        if (node->next == head)
            break;
        list_move(node, node->next);
    }
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    struct list_head *node = head->next;
    struct list_head *nextNode = node->next;
    for (; node != head; node = node->prev) {
        node->next = node->prev;
        node->prev = nextNode;
        nextNode = nextNode->next;
    }
    node->next = node->prev;
    node->prev = nextNode;
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head))
        return;
    struct list_head *node = head->next;
    while (true) {
        struct list_head *ptr = node;
        struct list_head *start = node->prev;
        for (int i = 0; i < k; i++) {
            if (node == head)
                return;
            node = node->next;
        }
        for (int i = 0; i < k; i++) {
            struct list_head *tmp = ptr->next;
            list_move(ptr, start);
            ptr = tmp;
        }
    }
    return;


    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}
// Merge two sorted list to head1
void q_merge_two(struct list_head *head1, struct list_head *head2)
{
    if (!head1 || !head2)
        return;
    struct list_head head;
    INIT_LIST_HEAD(&head);
    while (!list_empty(head1) && !list_empty(head2)) {
        element_t *e1 = list_first_entry(head1, element_t, list);
        element_t *e2 = list_first_entry(head2, element_t, list);
        if (strcmp(e1->value, e2->value) < 0) {
            list_move_tail(&(e1->list), &head);
        } else {
            list_move_tail(&(e2->list), &head);
        }
    }
    list_splice_tail_init(head1, &head);
    list_splice_tail_init(head2, &head);
    list_splice_init(&head, head1);
    return;
}

void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;
    struct list_head *mid = head;
    for (struct list_head *fast = head->next;
         fast != head && fast->next != head; fast = fast->next->next) {
        mid = mid->next;
    }
    struct list_head left;
    INIT_LIST_HEAD(&left);
    list_cut_position(&left, head, mid);
    q_sort(&left, descend);
    q_sort(head, descend);
    q_merge_two(head, &left);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return -1;
    int size = 1;
    struct list_head *node = head->prev;
    element_t *element = list_entry(node, element_t, list);
    char *max = strdup(element->value);
    node = node->prev;
    for (struct list_head *safe = node->prev; node != head;
         node = safe, safe = safe->prev) {
        element = list_entry(node, element_t, list);
        char *tmp = strdup(element->value);
        if (strcmp(tmp, max) >= 0) {
            list_del(node);
            q_release_element(element);
        } else {
            max = strdup(tmp);
            size++;
        }
    }
    return size;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return -1;
    int size = 1;
    struct list_head *node = head->prev;
    element_t *element = list_entry(node, element_t, list);
    char *min = strdup(element->value);
    node = node->prev;
    for (struct list_head *safe = node->prev; node != head;
         node = safe, safe = safe->prev) {
        element = list_entry(node, element_t, list);
        // char *tmp = strdup(element->value);
        if (strcmp(element->value, min) <= 0) {
            list_del(node);
            q_release_element(element);
        } else {
            free(min);
            min = strdup(element->value);
            size++;
        }
    }
    free(min);
    return size;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    queue_contex_t *first, *second;
    first = list_first_entry(head, queue_contex_t, chain);
    second = list_entry(first->chain.next, queue_contex_t, chain);
    while (true) {
        q_merge_two(first->q, second->q);
        list_del(second->q);
        // second->q = NULL;
        if (second->chain.next == head)
            break;
        else
            second = list_entry(second->chain.next, queue_contex_t, chain);
        /*
        if (second->chain.next == head)
            break;
        else
            second = list_entry(second->chain.next, queue_contex_t, chain);
            */
    }

    // https://leetcode.com/problems/merge-k-sorted-lists/
    return q_size(first->q);
}
