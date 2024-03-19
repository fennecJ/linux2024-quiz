#include <stdio.h>
#define SAMPLES 16000
#define EXP_CNT 1000

typedef struct {
    int compare;
    size_t max_run;
    double time;
} stat_t;