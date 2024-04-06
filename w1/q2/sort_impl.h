#ifndef _SORT_IMPL_
#define _SORT_IMPL_

struct list_head;

typedef int (*list_cmp_func_t)(void *,
                               const struct list_head *,
                               const struct list_head *);

void timsort(void *priv, struct list_head *head, list_cmp_func_t cmp);

void adaptive_ShiversSort(void *priv, struct list_head *head, list_cmp_func_t cmp);

void power_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);

void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);
#endif // !_SORT_IMPL_