#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "list.h"
typedef struct __node {
    long value;
    struct list_head list;
} node_t;

struct list_head *list_tail(struct list_head *head)
{
    while (head && head->next)
        head = head->next;
    return head;
}

int list_length(struct list_head *left)
{
    int n = 0;
    struct list_head *node;
    list_for_each(node, left)
        n++;
    return n;
}

void list_construct(struct list_head *list, int n)
{
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->value = n;
    list_add(&node->list, list);
}

void list_free(struct list_head *head)
{
    node_t *entry, *safe;
    list_for_each_entry_safe(entry, safe, head, list){
        free(entry);
    }
}
static inline void rebuild_list_link(struct list_head *head)
{
    if (!head)
        return;
    struct list_head *node, *prev;
    prev = head;
    node = head->next;
    while (node) {
        node->prev = prev;
        prev = node;
        node = node->next;
    }
    prev->next = head;
    head->prev = prev;
}
int max_cnt[30]  = {0};

void quick_sort(struct list_head *list)
{
    int n = list_length(list);
    int value;
    int i = 0;
    int max_depth = 0;
    int max_level = 2 * n;
    struct list_head *begin[max_level];
    struct list_head *result = NULL, *left = NULL, *right = NULL;
    struct list_head *head_record = list;
    begin[0] = list->next;
    list->prev->next = NULL;
    while (i >= 0) {
        struct list_head *L = begin[i], *R = list_tail(begin[i]);
        if (L != R) {
            struct list_head *pivot = L;
            value = list_entry(pivot, node_t, list)->value;
            struct list_head *p = pivot->next;
            pivot->next = NULL; // break the list
    
            while (p) {
                struct list_head *n = p;
                p = p->next;
                int n_value = list_entry(n, node_t, list)->value;
                if(n_value > value) {
                    n->next = right;
                    right = n;
                } else {
                    n->next = left;
                    left = n;
                }
            }

            begin[i] = left;
            begin[i + 1] = pivot;
            begin[i + 2] = right;
            if(i > max_depth)
                max_depth = i;
            left = right = NULL;
            i += 2;

        } else {
            if (L){
                L->next = result;
                result = L;
            }
            i--;
        }
    }
    list->next = result;
    rebuild_list_link(list);
    max_cnt[max_depth]++;
    // list = head_record;
}

/* Verify if list is order */
static bool list_is_ordered(struct list_head *head)
{   
    int value = list_entry(head->next, node_t, list)->value;
    node_t *entry;
    list_for_each_entry(entry, head, list){
        if(entry->value < value)
            return false;
        else
            value = entry->value;
    }

    return true;
}

/* shuffle array, only work if n < RAND_MAX */
void shuffle(int *array, size_t n)
{
    if (n <= 0)
        return;

    for (size_t i = 0; i < n - 1; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void swap(int *x, int *y)
{
    int temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

void permuteAndSort(int* a, int l, int r, int len, struct list_head *list) 
{ 
    int i;
    if (l == r) {
        while (len--)
            list_construct(list, a[len]);
        quick_sort(list);
        list_free(list);
        INIT_LIST_HEAD(list);
    }
    else { 
        for (i = l; i <= r; i++) {
            swap((a + l), (a + i));
            permuteAndSort(a, l + 1, r, len, list);
            swap((a + l), (a + i));
        }
    }
}

int fact(int k){
    int a = 1;
    while (k > 0) {
        a *= k;
        k--;
    }
    return a;
}

void statistic(struct list_head *list, int len){
    int *arr = malloc(sizeof(int) * len);
    for(int i = 0; i < len; i++){
        arr[i] = i + 1;
    }
    int per = fact(len);
    permuteAndSort(arr, 0, len - 1, len, list); 

    for(int i = 0; i < 30; i++){
        if(max_cnt[i] > 0)
            printf("%2d depth prob: %10f = %8d / %8d\n", i+2, max_cnt[i]/(double)per, max_cnt[i], per);
    }
}

int main(int argc, char **argv)
{
    struct list_head *list = malloc(sizeof(struct list_head));
    INIT_LIST_HEAD(list);

    size_t count = 100000;
    int *test_arr = malloc(sizeof(int) * count);
    for (int i = 0; i < count; ++i)
    test_arr[i] = i;
    shuffle(test_arr, count);
    
    while (count--)
    list_construct(list, test_arr[count]);
    quick_sort(list);
    assert(list_is_ordered(list));
    list_free(list);
    free(test_arr);
    if(argc == 2){
        INIT_LIST_HEAD(list);
        int len = atoi(argv[1]);
        statistic(list, len);
    }
    free(list);
    return 0;
}
