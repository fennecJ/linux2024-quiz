#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "sort_impl.h"
#include "def.h"

typedef struct {
    struct list_head list;
    int val;
    int seq;
} element_t;

static int stats[3][EXP_CNT] = {0};

static void create_sample(struct list_head *head, element_t *space, int samples)
{
    if(EXP_CNT == 1)
        printf("Creating sample\n");
    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        elem->val = rand();
        list_add_tail(&elem->list, head);
    }
}

static void copy_list(struct list_head *from,
                      struct list_head *to,
                      element_t *space)
{
    if (list_empty(from))
        return;

    element_t *entry;
    list_for_each_entry (entry, from, list) {
        element_t *copy = space++;
        copy->val = entry->val;
        copy->seq = entry->seq;
        list_add_tail(&copy->list, to);
    }
}

int compare(void *priv, const struct list_head *a, const struct list_head *b)
{
    if (a == b)
        return 0;

    int res = list_entry(a, element_t, list)->val -
              list_entry(b, element_t, list)->val;

    if (priv)
        *((int *) priv) += 1;

    return res;
}

bool check_list(struct list_head *head, int count)
{
    if (list_empty(head))
        return 0 == count;

    element_t *entry, *safe;
    size_t ctr = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        ctr++;
    }
    int unstable = 0;
    list_for_each_entry_safe (entry, safe, head, list) {
        if (entry->list.next != head) {
            if (entry->val > safe->val) {
                fprintf(stderr, "\nERROR: Wrong order\n");
                return false;
            }
            if (entry->val == safe->val && entry->seq > safe->seq)
                unstable++;
        }
    }
    if (unstable) {
        fprintf(stderr, "\nERROR: unstable %d\n", unstable);
        return false;
    }

    if (ctr != SAMPLES) {
        fprintf(stderr, "\nERROR: Inconsistent number of elements: %ld\n", ctr);
        return false;
    }
    return true;
}

typedef void (*test_func_t)(void *priv,
                            struct list_head *head,
                            list_cmp_func_t cmp);

typedef struct {
    char *name;
    test_func_t impl;
} test_t;

int main(void)
{
    struct list_head sample_head, warmdata_head, testdata_head;
    int count;
    int nums = SAMPLES;

    /* Assume ASLR */
    srand((uintptr_t) &main);

    test_t tests[] = {
        {.name = "timsort", .impl = timsort},
        {.name = "adaptive_ShiversSort", .impl = adaptive_ShiversSort},
        {.name = "powerSort", .impl = power_sort},
        {NULL, NULL},
    };

    INIT_LIST_HEAD(&sample_head);

    element_t *samples = malloc(sizeof(*samples) * SAMPLES);
    element_t *warmdata = malloc(sizeof(*warmdata) * SAMPLES);
    element_t *testdata = malloc(sizeof(*testdata) * SAMPLES);
    test_t *test;
    for(int i = 0; i < EXP_CNT; i++){
        test = tests;
        int a = 0;
        create_sample(&sample_head, samples, nums);
        while (test->impl) {
            if(EXP_CNT == 1)
                printf("==== Testing %s ====\n", test->name);
            /* Warm up */
            INIT_LIST_HEAD(&warmdata_head);
            INIT_LIST_HEAD(&testdata_head);
            copy_list(&sample_head, &testdata_head, testdata);
            copy_list(&sample_head, &warmdata_head, warmdata);
            test->impl(&count, &warmdata_head, compare);

            /* Test */
            count = 0;
            test->impl(&count, &testdata_head, compare);
            if(EXP_CNT == 1){
            printf("  Comparisons:    %d\n", count);
            printf("  List is %s\n",
                check_list(&testdata_head, nums) ? "sorted" : "not sorted");
            }
            stats[a][i] = count;
            test++;
            a++;
        }
    }

    FILE *stat_res = fopen("stat_res.txt", "w");
    test = tests;
    for(int i = 0; i < 3; i++){
        fprintf(stat_res, "\n%s:\n", test->name);
        for(int j = 0; j < EXP_CNT; j++){
            fprintf(stat_res, "%d ", stats[i][j]);
        }
        test++;
    }
    fclose(stat_res);
    return 0;
}