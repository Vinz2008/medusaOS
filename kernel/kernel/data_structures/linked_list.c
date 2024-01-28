#include <kernel/kmalloc.h>
#include <kernel/linked_list.h>
#include <stdbool.h>

linked_list_t* linked_list_init() {
  linked_list_t* linked_list = kmalloc(sizeof(linked_list_t));
  linked_list->data = NULL;
  linked_list->next = NULL;
  return linked_list;
}

void linked_list_insert_beginning(void* data, linked_list_t** node) {
  linked_list_t* new_node = kmalloc(sizeof(linked_list_t));
  new_node->data = data;
  new_node->next = *node;
  *node = new_node;
}

void append_linked_list(void* data, linked_list_t* node) {
  linked_list_t* new_node = kmalloc(sizeof(linked_list_t));
  new_node->data = data;
  linked_list_t* temp = node;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = new_node;
}

bool linked_list_find(void* data, linked_list_t* node) {
  linked_list_t* temp = node;
  while (temp != NULL) {
    if (temp->data == data) {
      return true;
    }
    temp = temp->next;
  }
  return false;
}