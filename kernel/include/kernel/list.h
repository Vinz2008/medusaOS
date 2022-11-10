#include <stdint.h>
#include <stdbool.h>

typedef struct list_t {
    void* data;
    struct list_t* next;
    struct list_t* prev;
} list_t;

#define LIST_HEAD_INIT(name) (list_t) { NULL, &(name), &(name) }
#define list_for_each(iter, pos, list) \
    for (iter = (list)->next, pos = (typeof(pos)) iter->data; \
        iter != (list); iter = iter->next, pos = (typeof(pos)) iter->data)
#define list_for_each_entry_rev(iter, pos, list) \
    for (iter = (list)->prev, pos = (typeof(pos)) iter->data; \
        iter != (list); iter = iter->prev, pos = (typeof(pos)) iter->data)
#define list_first_entry(list, type) \
    ((type*) (list)->next->data)
#define list_for_each_entry(pos, list) \
    pos = (typeof(pos)) (list)->next->data; \
    for (list_t* __iter = (list)->next; \
        __iter != (list); __iter = __iter->next, pos = (typeof(pos)) __iter->data)

void __list_add(list_t* new, list_t* prev, list_t* next);
list_t* list_add(list_t* list, void* data);
list_t* list_add_front(list_t* list, void* data);
void __list_del(list_t* prev, list_t* next);
void list_del(list_t* entry);