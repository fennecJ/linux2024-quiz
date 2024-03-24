#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "list.h"
#include "sort_impl.h"
#include "sample_impl.h"
#include "def.h"


stat_t stats[3][EXP_CNT] = {0};



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

    if (priv){
        ((stat_t*) priv)-> compare += 1;
    }

    return res;
}

bool check_list(struct list_head *head, int statistic)
{
    if (list_empty(head))
        return 0 == statistic;

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

typedef void (*sample_func_t)(struct list_head *head, element_t *space, int samples);
typedef struct {
    char *name;
    sample_func_t impl;
} sample_t;

int main(void)
{
    struct list_head sample_head, warmdata_head, testdata_head;
    stat_t statistic;
    int nums = SAMPLES;

    /* Assume ASLR */
    srand((uintptr_t) &main);

    test_t tests[] = {
        {.name = "timsort", .impl = timsort},
        {.name = "adaptive_ShiversSort", .impl = adaptive_ShiversSort},
        {.name = "powerSort", .impl = power_sort},
        {NULL, NULL},
    };
    sample_t samples_creators[] = {
        {.name = "sample_rnd", .impl = create_sample_rnd},
        {.name = "sample_des_strict", .impl = create_sample_descend_strict},
        {.name = "sample_des", .impl = create_sample_descend},
        {.name = "sample_as_strict", .impl = create_sample_ascend_strict},
        {.name = "sample_des", .impl = create_sample_ascend},
        {.name = "sample_rnd3", .impl = create_sample_rnd3},
        {.name = "sample_as_10", .impl = create_sample_ascend_10},
        {.name = "sample_rnd_1Percent", .impl = create_sample_rnd_1_percent},
        {.name = "sample_dup", .impl = create_sample_dup},
        {.name = "sample_samp", .impl = create_sample_same},
        {NULL, NULL},
    };


    element_t *samples = malloc(sizeof(*samples) * SAMPLES);
    element_t *warmdata = malloc(sizeof(*warmdata) * SAMPLES);
    element_t *testdata = malloc(sizeof(*testdata) * SAMPLES);
    test_t *test;
    sample_t *create_sample = samples_creators;
    while (create_sample->impl) {
    
        for(int i = 0; i < EXP_CNT; i++){
            INIT_LIST_HEAD(&sample_head);
            test = tests;
            int a = 0;
            create_sample->impl(&sample_head, samples, nums);

            while (test->impl) {
                if(EXP_CNT == 1)
                    printf("==== Testing %s with %s ====\n", test->name, create_sample->name);
                /* Warm up */
                INIT_LIST_HEAD(&warmdata_head);
                INIT_LIST_HEAD(&testdata_head);
                copy_list(&sample_head, &testdata_head, testdata);
                copy_list(&sample_head, &warmdata_head, warmdata);
                test->impl(&statistic, &warmdata_head, compare);

                /* Test */
                statistic.compare = 0;
                statistic.max_run = 0;
                double start_time = clock() / (double) CLOCKS_PER_SEC;
                test->impl(&statistic, &testdata_head, compare);
                double end_time = clock() / (double) CLOCKS_PER_SEC;
                
                if(EXP_CNT == 1){
                    printf("  Comparisons:    %d\n", statistic.compare);
                    printf("  List is %s\n",
                    check_list(&testdata_head, nums) ? "sorted" : "not sorted");
                } else if(!check_list(&testdata_head, nums)) {
                    printf("Error: %s has sort error in %s\n", test->name, create_sample->name);
                }
                statistic.time = end_time - start_time;
                stats[a][i] = statistic;
                test++;
                a++;
            }
        }
        char cmp_file[40];
        char max_run_file[40];
        char time_file[40];
        sprintf(cmp_file, "cmp_res_%s.txt", create_sample->name);
        sprintf(max_run_file, "max_run_res_%s.txt", create_sample->name);
        sprintf(time_file, "time_res_%s.txt", create_sample->name);

        FILE *cmp_res = fopen(cmp_file, "w");
        FILE *max_run_res = fopen(max_run_file, "w");
        FILE *time_res = fopen(time_file, "w");
        test = tests;
        for(int i = 0; i < 3; i++){
            fprintf(cmp_res, "\n%s:\n", test->name);
            fprintf(max_run_res, "\n%s:\n", test->name);
            fprintf(time_res, "\n%s:\n", test->name);
            for(int j = 0; j < EXP_CNT; j++){
                fprintf(cmp_res, "%d ", stats[i][j].compare);
                fprintf(max_run_res, "%ld ", stats[i][j].max_run);
                fprintf(time_res, "%f ", stats[i][j].time);

                // reset for next sample 
                stats[i][j].compare = 0;
                stats[i][j].max_run = 0;
                stats[i][j].time = 0.0;
            }
            test++;
        }
        fclose(cmp_res);
        fclose(max_run_res);
        fclose(time_res);
        create_sample++;
    }

    return 0;
}