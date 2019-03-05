#include <assert.h>
#include <stdlib.h>
#include "common.h"
#include "list.h"

static uint16_t values[256];

/**
 * list_merge() - Merge two sorted lists in non-decreasing order
 * @left: pointer to the head of first list
 * @right: pointer to the head of second list
 */
static void list_merge(struct list_head *left, struct list_head *right)
{
    LIST_HEAD(tmp);

    while (!list_empty(left) && !list_empty(right)) {
        uint16_t ldata = list_entry(left->next, struct listitem, list)->i;
        uint16_t rdata = list_entry(right->next, struct listitem, list)->i;
        if (cmpint(&ldata, &rdata) < 0)
            list_move_tail(left->next, &tmp);
        else
            list_move_tail(right->next, &tmp);
    }

    if (list_empty(left))
        list_splice_tail(right, &tmp);
    else if (list_empty(right))
        list_splice_tail(left, &tmp);

    list_cut_position(right, &tmp, tmp.prev);
}

/**
 * list_middle() - Find middle point of the list
 * @head: pointer to the head of the list
 *
 * Return: pointer to the middle point of the list
 */
static struct list_head *list_middle(struct list_head *head)
{
    struct list_head *slow, *fast;
    slow = fast = head->next;

    for (;;) {
        fast = fast->next;
        fast = fast->next;
        if (fast == head || fast == head->next)
            break;
        slow = slow->next;
    }
    return slow;
}

/**
 * list_mergesort() - Perform mergesort on the list
 * @head: pointer to the head of the list
 */
static void list_mergesort(struct list_head *head)
{
    if (list_empty(head) || list_is_singular(head))
        return;

    LIST_HEAD(left);
    struct list_head *middle = list_middle(head);
    list_cut_position(&left, head, middle);

    list_mergesort(&left);
    list_mergesort(head);
    list_merge(&left, head);
}

int main(void)
{
    LIST_HEAD(testlist);
    assert(list_empty(&testlist));
    random_shuffle_array(values, (uint16_t) ARRAY_SIZE(values));

    for (size_t i = 0; i < ARRAY_SIZE(values); ++i) {
        struct listitem *item = malloc(sizeof(struct listitem));
        assert(item);
        item->i = values[i];
        list_add_tail(&item->list, &testlist);
    }
    assert(!list_empty(&testlist));

    qsort(values, ARRAY_SIZE(values), sizeof(values[0]), cmpint);
    list_mergesort(&testlist);

    size_t i = 0;
    struct listitem *item = NULL, *safe = NULL;
    list_for_each_entry_safe (item, safe, &testlist, list) {
        assert(item->i == values[i]);
        list_del(&item->list);
        free(item);
        ++i;
    }
    assert(i == ARRAY_SIZE(values));
    assert(list_empty(&testlist));

    return 0;
}
