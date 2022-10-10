// from https://github.com/dipolukarov/osdev/blob/master/ordered_array.h

#include <types.h>

#ifndef _KERNEL_ORDERED_ARRAY_
#define _KERNEL_ORDERED_ARRAY_

typedef void* type_t;

typedef int8 (*lessthan_predicate_t)(type_t,type_t);

typedef struct
{
	type_t			*array;
	uint32_t			size;
	uint32_t			max_size;
	lessthan_predicate_t	less_than;
} ordered_array_t;


int8 standard_lessthan_predicate(type_t a, type_t b);

/**
 * Create an ordered array.
 */
ordered_array_t create_ordered_array(uint32_t max_size, lessthan_predicate_t less_than);
ordered_array_t place_ordered_array(void *addr, uint32_t max_size, lessthan_predicate_t less_than);

/**
 * Destroy an ordered array.
 */
void destroy_ordered_array(ordered_array_t *array);

/**
 * Add an item into the array.
 */
void insert_ordered_array(type_t item, ordered_array_t *array);

/**
 * Lookup the item at index i.
 */
type_t lookup_ordered_array(uint32_t i, ordered_array_t *array);

/**
 * Deletes the item at location i from the array.
 */
void remove_ordered_array(uint32_t i, ordered_array_t *array);

#endif