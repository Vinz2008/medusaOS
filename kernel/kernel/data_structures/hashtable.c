#include <assert.h>
#include <kernel/hashtable.h>
#include <kernel/kmalloc.h>
#include <kernel/list.h>
#include <stdlib.h>
#include <string.h>

// https://benhoyt.com/writings/hash-table-in-c/

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

hashtable_t* hashtable_create() {
  hashtable_t* hashtable = kmalloc(sizeof(hashtable_t));
  hashtable->entries = list_create_ext(0, HASHTABLE_INITIAL_CAPACITY);
  for (int i = 0; i < hashtable->entries->size; i++) {
    hashtable->entries->list[i].data = NULL;
  }
  return hashtable;
}

void hashtable_destroy(hashtable_t* htable) {
  for (size_t i = 0; i < htable->entries->size; i++) {
    hashtable_entry* temp_entry = htable->entries->list[i].data;
    kfree(temp_entry->key);
  }

  // Then free entries array and table itself.
  list_destroy(htable->entries);
  kfree(htable);
}

// Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
static uint64_t hash_key(const char* key) {
  uint64_t hash = FNV_OFFSET;
  for (const char* p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  int temp_hash32 = hash;
  log(LOG_SERIAL, false, "hash %d from key %s\n", temp_hash32, key);
  return temp_hash32;
}

static const char* hashtable_set_entry(list_t* entries, const char* key,
                                       void* value) {
  uint64_t hash = hash_key(key);
  size_t index = (size_t)(hash & (uint64_t)(entries->size - 1));
  hashtable_entry* temp_entry = entries->list[index].data;
  log(LOG_SERIAL, false, "TEST\n");
  while (temp_entry->key != NULL) {
    log(LOG_SERIAL, false, "TEST\n");
    if (strcmp(key, temp_entry->key) == 0) {
      temp_entry->value = value;
      return temp_entry->key;
    }
    index++;
    if (index >= entries->size) {
      index = 0;
    }
    temp_entry = entries->list[index].data;
  }
  key = strdup(key);
  entries->used++;
  temp_entry = entries->list[index].data;
  temp_entry->key = (char*)key;
  temp_entry->value = value;
  return key;
}

static bool hashtable_expand(hashtable_t* htable) {
  size_t new_capacity = htable->entries->size * 2;
  if (new_capacity < htable->entries->used) {
    return false; // overflow
  }
  list_t* new_entries = list_create_ext(0, new_capacity);
  for (int i = 0; i < new_entries->size; i++) {
    new_entries->list[i].data = NULL;
  }

  for (size_t i = 0; i < htable->entries->size; i++) {
    hashtable_entry* temp_entry = htable->entries->list[i].data;
    if (temp_entry != NULL || temp_entry->key == NULL) {
      hashtable_set_entry(new_entries, temp_entry->key, temp_entry->value);
    }
  }

  list_destroy(htable->entries);
  htable->entries = new_entries;
  htable->entries->size = new_capacity;
  return true;
}

void* hashtable_get(const char* key, hashtable_t* htable) {
  uint64_t hash = hash_key(key);
  size_t index = (size_t)(hash & (uint64_t)(htable->entries->size - 1));
  hashtable_entry* temp_entry = htable->entries->list[index].data;
  while (temp_entry->key != NULL) {
    if (strcmp(key, temp_entry->key) == 0) {
      return temp_entry->value;
    }
    index++;
    if (index >= htable->entries->size) {
      index = 0;
    }
    temp_entry = htable->entries->list[index].data;
  }
  return NULL;
}

const char* hashtable_set(hashtable_t* htable, const char* key, void* value) {
  // assert(value != NULL);
  if (value == NULL) {
    return NULL;
  }
  if (htable->entries->used >= htable->entries->size / 2) {
    if (!hashtable_expand(htable)) {
      return NULL;
    }
  }
  return hashtable_set_entry(htable->entries, key, value);
}