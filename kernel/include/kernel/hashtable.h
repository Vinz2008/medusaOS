#include <kernel/list.h>
#include <stddef.h>

#define HASHTABLE_INITIAL_CAPACITY 16

typedef struct {
  char* key; // key is NULL if this slot is empty
  void* value;
} hashtable_entry;

typedef struct Hashtable {
  list_t* entries; // hash slots
} hashtable_t;

hashtable_t* hashtable_create();
void* hashtable_get(const char* key, hashtable_t* htable);
const char* hashtable_set(hashtable_t* htable, const char* key, void* value);
void hashtable_destroy(hashtable_t* htable);