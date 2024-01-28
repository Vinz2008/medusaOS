#include <kernel/kmalloc.h>
#include <kernel/pair.h>

pair_t create_pair(void* first, void* second) {
  return (pair_t){.first = first, .second = second};
}

// create pair with the struct allocated in the heap
pair_t* create_pair_a(void* first, void* second) {
  pair_t* pair = kmalloc(sizeof(pair_t));
  pair->first = first;
  pair->second = second;
  return pair;
}