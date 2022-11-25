#include <stdint.h>
#include <stdbool.h>

#define list_foreach(list_node, list) for (ListNode* list_node = list->head; NULL != list_node ; list_node = list_node->next)

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
ListNode* list_get_last_node(List* list);