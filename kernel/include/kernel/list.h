#include <stdbool.h>
#include <stdint.h>

#pragma once

/*#define list_foreach(list_node, list) for (ListNode* list_node = list->head;
NULL != list_node ; list_node = list_node->next)

typedef struct ListNode
{
    struct ListNode* previous;
    struct ListNode* next;
    void* data;
} ListNode;

typedef struct List
{
    struct ListNode* head;
    struct ListNode* tail;
} List;

List* list_create();
void list_clear(List* list);
void list_destroy(List* list);
List* list_create_clone(List* list);
bool list_is_empty(List* list);
void list_append(List* list, void* data);
void list_prepend(List* list, void* data);
ListNode* list_get_first_node(List* list);
ListNode* list_get_last_node(List* list);*/

#define list_foreach(data, list)                                               \
  for (int i = 0, void *data = NULL; i < list->used;                           \
       i++, data = list->list[i].data)

struct listItem {
  void* data;
};

typedef struct List {
  size_t used;
  size_t size;
  struct listItem* list;
} list_t;

list_t* list_create();
list_t* list_create_ext(size_t used, size_t size);
void list_clear(list_t* list);
void list_destroy(list_t* list);
bool list_is_empty(list_t* list);
void list_append(void* data, list_t* list);
list_t* list_remove(void* data, list_t* list);
list_t* list_remove_index(int index, list_t* list);
void* list_find(void* data, list_t* list);
void* list_find_at(int index, list_t* list);