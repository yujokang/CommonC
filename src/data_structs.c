#include <data_structs.h>

#include <logger.h>
#include <debug_assert.h>
#include <string.h>
#include <errno.h>

/*
 * type-independent macro for swapping values of two primitive variables.
 * a:	first lvalue, which will hold the old value of "b"
 * b:	second lvalue, which will hold the old value of "a"
 */
#define SWAP(a, b) do { \
	a ^= b; \
	b ^= a; \
	a ^= b; \
} while (0)

/*
 * Debugging function for checking that a position is between
 * 1 and the size of the heap, inclusively.
 * heap:	the source heap
 * position:	the position to check
 * returns	1 iff the position is in range, 0 otherwise
 */
static inline int position_in_range(struct min_heap *heap, size_t position)
{
	if (position < 1) {
		printlg(ERROR_LEVEL, "Position %u is too low.\n",
			(unsigned) position);
		return 0;
	}
	if (position > heap->size) {
		printlg(ERROR_LEVEL, "Position %u is higher than size %u.\n",
			(unsigned) position, (unsigned) heap->size);
		return 0;
	}
	return 1;
}

/*
 * helper function for extracting the key value
 * of an element at a given position.
 * heap:	the source heap
 * position:	the position of the element,
 *		which must be between 1 and "heap->size" inclusively
 * returns	the "key" field of the element in the heap
 */
static inline int heap_key(struct min_heap *heap, size_t position)
{
	debug_assert(position_in_range(heap, position));
	return heap->elements[position].key;
}

/*
 * Swap two elements in a heap.
 * heap:	the source and target heap
 * position_a:	the position of the first element to swap
 * position_b:	the position of the second element to swap
 */
static inline void swap_in_heap(struct min_heap *heap, size_t position_a,
				size_t position_b)
{
	struct heap_element *element_a = &(heap->elements[position_a]);
	struct heap_element *element_b = &(heap->elements[position_b]);
	void *temp_data;
#ifdef DEBUG
	void *old_data_a = element_a->data, *old_data_b = element_b->data;
#endif /* DEBUG */

	temp_data = element_a->data;
	element_a->data = element_b->data;
	element_b->data = temp_data;
	debug_assert(element_a->data == old_data_b);
	debug_assert(element_b->data == old_data_a);
	debug_assert_imply(old_data_a != old_data_b,
			   element_a->data != old_data_a);
	debug_assert_imply(old_data_a != old_data_b,
			   element_b->data != old_data_b);

	SWAP(element_a->key, element_b->key);
}

/*
 * For heapifying an array and popping an element in a heap,
 * check a heap node with its children,
 * and swap it with the smallest child if it is smaller,
 * and recurse to the new position.
 * heap:	the target heap
 * position:	the current parent to check
 */
static void trickle_down(struct min_heap *heap, size_t position)
{
	size_t child_0 = position * 2, child_1 = child_0 + 1;
	size_t next_position = position;
	int child_0_in_range, child_1_in_range;

	/*
	 * We check ranges using the smaller "position" value,
	 * since the actual "child_0" and "child_1" may have wrapped around.
	 */
	child_0_in_range = (position <= (heap->size / 2));
	child_1_in_range = (heap->size >= 1) &&
			   (position <= ((heap->size - 1) / 2));

	debug_assert_imply(child_1_in_range, child_0_in_range);
	if (child_0_in_range) {
		if (heap_key(heap, next_position) > heap_key(heap, child_0)) {
			next_position = child_0;
		}

		if (child_1_in_range) {
			if (heap_key(heap, next_position) >
			    heap_key(heap, child_1)) {
				next_position = child_1;
			}
		}
	}

	if (next_position != position) {
		swap_in_heap(heap, position, next_position);
		trickle_down(heap, next_position);
	}

	debug_assert_imply(child_0_in_range,
			   heap_key(heap, position) <= heap_key(heap, child_0));
	debug_assert_imply(child_1_in_range,
			   heap_key(heap, position) <= heap_key(heap, child_1));
}

int
heapify(struct min_heap *heap_out, struct heap_element *array_in, size_t size)
{
	size_t position;
	if (size > heap_out->capacity) {
		printlg(ERROR_LEVEL, "Size %u exceeds capacity %u.\n",
			(unsigned) size, (unsigned) heap_out->capacity);
		errno = ERANGE;
		return -1;
	}

	printlg(DEBUG_LEVEL, "Heapifying %u elements.\n", (unsigned) size);
	memcpy(&(heap_out->elements[1]), array_in,
	       sizeof(struct heap_element) * size);
	heap_out->size = size;

	for (position = size / 2; position >= 1; position--) {
		trickle_down(heap_out, position);
	}

	return 0;
}

int pop_min_heap(struct heap_element *head_out, struct min_heap *heap_in)
{
	if (heap_in->size == 0) {
		return -1;
	}

	memcpy(head_out, &(heap_in->elements[1]),
	       sizeof(struct heap_element));
	debug_assert(head_out->data == heap_in->elements[1].data);
	memcpy(&(heap_in->elements[1]), &(heap_in->elements[heap_in->size]),
	       sizeof(struct heap_element));
	heap_in->size--;
	trickle_down(heap_in, 1);

	return 0;
}

int heap_sort(struct heap_element *array, size_t size)
{
	struct min_heap sorter;
	size_t element_i;

	if (init_min_heap(&sorter, size)) {
		printlg(ERROR_LEVEL, "Could not create heap for sorting.\n");
		return -1;
	}

	if (heapify(&sorter, array, size)) {
		/*
		 * Failure should not occur,
		 * since the capacity is exactly the size.
		 */
		debug_assert(0);
	}

	for (element_i = 0; element_i < size; element_i++) {
		if (pop_min_heap(&array[element_i], &sorter)) {
			/*
			 * Failure should not occur,
			 * since this loop is popping as many elements
			 * as were put into the array.
			 */
			debug_assert(0);
		}
	}

	teardown_min_heap(&sorter);
	return 0;
}
