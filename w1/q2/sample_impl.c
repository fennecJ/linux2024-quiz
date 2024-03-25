#include "sample_impl.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void create_sample_rnd(struct list_head *head, element_t *space, int samples)
{
    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        elem->val = rand();
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
}
// create sample in strict descend order
void create_sample_descend_strict(struct list_head *head, element_t *space, int samples)
{
    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        elem->val = samples - i;
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
}

// create sample in soft descend order (allow duplicate)
void create_sample_descend(struct list_head *head, element_t *space, int samples)
{
    int k = samples;
    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        // 25% chance not change k
        // 00, 01, 10, 11, only 00 will maintain the value
        if(rand() & 3)
            k = samples - i;
        elem->val = k;
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
}

// create sample in strict ascend order
void create_sample_ascend_strict(struct list_head *head, element_t *space, int samples)
{
    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        elem->val = i;
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
}

// create sample in soft ascend order (allow duplicate)
void create_sample_ascend(struct list_head *head, element_t *space, int samples)
{
    int k = 0;
    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        // 25% chance not change k
        // 00, 01, 10, 11, only 00 will maintain the value
        if(rand() & 3)
            k = i;
        elem->val = k;
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
}

/* create sample in ascend order, then randomly 
exchange the order of elements in groups of three */
void create_sample_rnd3(struct list_head *head, element_t *space, int samples)
{
    if(samples < 3){
        printf("Too few samples for rnd3\n");
        return;
    }

    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        elem->val = i;
        list_add_tail(&elem->list, head);
    }
    // perform 3 shuffle
    for(int i = 0; i <= samples; i += 3){
        /**
         * Shuffel only first and second (123 -> 213) or second and third (123 -> 132)
         * Original order is ABC
         * Different shuffle value will interchangeably swap the first two or the last two elements
         * 0 - swap(0, 1, ABC) -> BAC
         * 1 - swap(1, 2, BAC) -> BCA
         * 2 - swap(0, 1, BCA) -> CBA
         * 3 - swap(1, 2, CBA) -> CAB
         * 4 - swap(0, 1, CAB) -> ACB
         * 5 - swap(1, 2, ACB) -> ABC
         */
        int tmpVal;
        int shuffle = rand() % 6;
        for(int j = 0; j < shuffle; j++){
            int swap_ind = (j & 1) + i;
            element_t *elem_1 = space + swap_ind;
            element_t *elem_2 = space + swap_ind + 1;
            tmpVal = elem_1->val;
            elem_1->val = elem_2->val;
            elem_2->val = tmpVal;
        }

    }

    // set the seq for stable test
    for (int i = 0; i < samples; i++) {
        element_t *elem = space + i;
        elem->seq = i;
    }

}

// create sample in ascend order, then insert 10 randomly number in the end
void create_sample_ascend_10(struct list_head *head, element_t *space, int samples)
{
    if(samples < 10){
        printf("Too few samples for ascend_10\n");
        return;
    }

    for (int i = 0; i < samples - 10; i++) {
        element_t *elem = space + i;
        elem->val = i;
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }

    for(int i = samples - 10; i < samples; i++){
        element_t *elem = space + i;
        elem->val = rand();
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
}
// create sample in ascend order, then randomly set 1% element's value to be random
void create_sample_rnd_1_percent(struct list_head *head, element_t *space, int samples)
{
    if(samples < 100){
        printf("Too few samples for ascend_rnd_1_percent\n");
        return;
    }

    int rnd_cnt = samples / 100;

    for(int i = 0; i < samples; i++){
        element_t *elem = space + i;
        elem->val = i;
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
    // cnt how many idx's value is changed to rand
    int changed = 0;
    int max_ind = samples - rnd_cnt;
    while (changed < rnd_cnt)
    {
        int rnd_idx = rand() % max_ind;
        element_t *elem = space + rnd_idx;

        /**
         * rnd_idx will be in range 0 ~ max_ind - 1.
         * When a rnd_idx is choosed, we can know if
         * this idx is chosen before by examing the
         * corresponding element's value:
         * If its value is the same as its position idx,
         * means it's hasn't be chosen before, set its
         * value to be random. Otherwise, set its value
         * to be max_ind. Because all the possibile idx
         * chosen before < max_ind, thus we can assure
         * max_ind hasn't be choose and ensure all chosen
         * idx are unique.
         * 
         * This method has a minor defactor though:
         * When updating elem->val with rand(), rand()
         * has a little possibility to be the same as
         * previous elem->val (i.e. elem->val is 1 before)
         * and rand() return 1.
         * To address this problem, we can use a while loop
         * to repeatedly choose a rand number until rand 
         * number is different to previous val to update
         * elem->val.
         * With possibility of each trial_cnts is:
         * Let r = (1 / 2^32), f(trial_cnts) = r^(trial_cnts)
         * Possibility Sum S0 of diff trial_cnts will be
         * S0 = r + 2r^2 + 3r^3 + ... + nr^n (n is trial_cnts)
         * Les S1 = rS0 = r^2 + 2r^3 + 3r^4 ...
         * S0 - S1 = r + r^2 + r^3 + ... + r^n = S0(1 - r)
         * Where r + r^2 + ... + r^n = r * (1 - r^n) / (1 - r)
         *  = r / (1 - r) as n -> inf
         * Thus we know the amortized complexity will converged to be:
         * r / ((1 - r)^2). Since r << 1, amortized time complexity of
         * extra trials is roughly to be r. Which is a extremely small
         * number. Thus we can consider the process to be constant time.
         */

        if(elem->val == rnd_idx){
            int old_val = elem->val;
            while (elem->val == old_val)
                elem->val = rand();
        } else {
            elem = space + max_ind;
            int old_val = elem->val;
            while (elem->val == old_val)
                elem->val = rand();
        }
        max_ind++;
        changed++;
    }
}

// Repeatedly generate a sequence 1, 2, 3, 4 as sample
void create_sample_dup(struct list_head *head, element_t *space, int samples)
{
    if(samples < 4){
        printf("Too few samples for smaple duplicate\n");
        return;
    }

    for(int i = 0; i < samples; i += 4){
        for(int j = 0; j < 4; j++){
            element_t *elem = space + i + j;
            elem->val = j;
            elem->seq = i;
            list_add_tail(&elem->list, head);
        }
    }
}

// Generate a sample with all elem->val is 1
void create_sample_same(struct list_head *head, element_t *space, int samples)
{
    for(int i = 0; i < samples; i ++){
        element_t *elem = space + i;
        elem->val = 0;
        elem->seq = i;
        list_add_tail(&elem->list, head);
    }
}