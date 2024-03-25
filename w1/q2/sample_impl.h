#ifndef __SAMPLE_H

#include "list.h"
typedef struct {
    struct list_head list;
    int val;
    int seq;
} element_t;

// static void create_sample(struct list_head *head, element_t *space, int samples);
void create_sample_rnd(struct list_head *head, element_t *space, int samples);
void create_sample_descend_strict(struct list_head *head, element_t *space, int samples);
void create_sample_descend(struct list_head *head, element_t *space, int samples);
void create_sample_ascend_strict(struct list_head *head, element_t *space, int samples);
void create_sample_ascend(struct list_head *head, element_t *space, int samples);
void create_sample_rnd3(struct list_head *head, element_t *space, int samples);
void create_sample_ascend_10(struct list_head *head, element_t *space, int samples);
void create_sample_rnd_1_percent(struct list_head *head, element_t *space, int samples);
void create_sample_dup(struct list_head *head, element_t *space, int samples);
void create_sample_same(struct list_head *head, element_t *space, int samples);
void shuffle_worst(int* arr, int arr_len);
void create_sample_worst(struct list_head *head, element_t *space, int samples);


#endif // !__SAMPLE_H