/*
 * Basic data structures and functions
 */
#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#include <stdlib.h>
#include <errno.h>

#include <logger.h>

/* a single, sortable heap element */
struct heap_element {
	int key; /* the key by which to sort the heap */
	void *data; /* the data associated with the key */
};

/* a min-heap data structure */
struct min_heap {
	/* the maximum number of elements that this heap can hold */
	size_t capacity;
	/* the current number of elements that this heap holds */
	size_t size;

	/* holds the heap of elements */
	struct heap_element *elements;
};

/*
 * Initialize a heap and allocate the array of elements.
 * to_init:	the heap to initialize
 * capacity:	the "capacity" field,
 *		as well as the number of elements to allocate
 * returns	0 if successful
 *		-1 if allocation failed, with errno set to ENOMEM
 */
static inline int init_min_heap(struct min_heap *to_init, size_t capacity)
{
	/*
	 * In a heap array, the 0 index is never used,
	 * so the array "starts" one position before the allocated space.
	 */
	struct heap_element *
	malloced_elements = malloc(sizeof(struct heap_element) * capacity);
	if (malloced_elements == NULL) {
		printlg(ERROR_LEVEL, "Could not allocate heap elements.\n");
		errno = ENOMEM;
		return -1;
	}
	to_init->elements = malloced_elements - 1;

	to_init->size = 0;
	to_init->capacity = capacity;

	return 0;
}

/*
 * Reset all the fields in a min-heap
 * and deallocate its array so that the struct can be deallocated.
 * to_teardown:	the min-heap to tear down.
 *		The pointer itself will not be freed.
 */
static inline void teardown_min_heap(struct min_heap *to_teardown)
{
	/*
	 * The allocated space that we want to free is
	 * one position after the pointer.
	 */
	free(to_teardown->elements + 1);
	to_teardown->elements = NULL;
	to_teardown->capacity = 0;
	to_teardown->size = 0;
}

/*
 * Put all the elements of an array into a heap.
 * heap_out:	the destination heap
 * array_in:	the source array
 * size:	the number of elements in array_in,
 *		which should not exceed the capacity of the heap
 * returns	0 on success and
 *		-1 if the size is too large, with errno set to ERANGE
 */
int
heapify(struct min_heap *heap_out, struct heap_element *array_in, size_t size);
/*
 * Remove the smallest element from the min-heap, and rearrange accordingly
 * head_out:	the space to write the head element
 * heap_in:	the source heap
 * returns	0 iff successfully popped the heap.
 *		-1 if the heap was empty
 */
int pop_min_heap(struct heap_element *head_out, struct min_heap *heap_in);
/*
 * Perform heap sort on an array,
 * so that the keys are sorted from lowest to highest.
 * array:	the array to sort
 * size:	the number of elements in the array
 * returns	0 iff successful,
 *		-1 on allocation failure, with errno set to ENOMEM.
 */
int heap_sort(struct heap_element *array, size_t size);

#endif /* DATA_STRUCTS_H */
