#include "heap_sort_tvs.h"

#include <data_structs.h>
#include <logger.h>

/*
 * the data for each heap element,
 * to make sure that sorting was done correctly.
 */
struct element_checker {
	/*
	 * the key of the element,
	 * to make sure that the data and key stay together
	 */
	int key;
	/*
	 * flag to make sure that each element
	 * appears exactly once in the sorted array
	 */
	int found;
	/*
	 * Remember the element's original position in the array
	 * for error reporting purposes.
	 */
	size_t pre_position;
};

/*
 * Run a single heap sort test
 * tv:		the test case vector
 * returns	1 iff passed, 0 otherwise
 */
static int test_heap_sort(struct heap_sort_tv *tv)
{
	size_t n_keys = tv->n_keys;
	int correct = 1;
	struct element_checker element_datas[n_keys];
	struct heap_element elements[n_keys];
	size_t element_i;

	/* initialization */
	for (element_i = 0; element_i < n_keys; element_i++) {
		/* Initialize data. */
		element_datas[element_i].key = tv->keys[element_i];
		element_datas[element_i].found = 0;
		element_datas[element_i].pre_position = element_i;

		/* Initialize elements that contain the data. */
		elements[element_i].key = tv->keys[element_i];
		elements[element_i].data = &element_datas[element_i];
	}

	/* sorting */
	if (heap_sort(elements, n_keys)) {
		printlg(ERROR_LEVEL, "Heap sort failed.\n");
		return 0;
	}

	/* checking */
	for (element_i = 0; element_i < n_keys; element_i++) {
		struct heap_element *element = &elements[element_i];
		struct element_checker *
		checker = (struct element_checker *) element->data;

		/* Check that an element appears no more than once. */
		if (checker->found) {
			printlg(ERROR_LEVEL,
				"%uth element of key %d was already found.\n",
				(unsigned) checker->pre_position, element->key);
			correct = 0;
		} else {
			checker->found = 1;
		}

		/* Check that the data and key move together. */
		if (checker->key != element->key) {
			printlg(ERROR_LEVEL,
				"%uth element had key %d, but now it's %d.\n",
				(unsigned) checker->pre_position, checker->key,
				element->key);
			correct = 0;
		}

		/* Check that keys are non-decreasing. */
		if (element_i > 0) {
			struct heap_element *
			prev_element = &elements[element_i - 1];

			if (element->key < prev_element->key) {
				printlg(ERROR_LEVEL,
					"At position %u, "
					"the key decreased from %d to %d.\n",
					(unsigned) element_i,
					prev_element->key, element->key);
				correct = 0;
			}
		}
	}

	/* Check that an element appears at least once. */
	for (element_i = 0; element_i < n_keys; element_i++) {
		if (!element_datas[element_i].found) {
			printlg(ERROR_LEVEL,
				"Element %u is no longer in the array.\n",
				(unsigned) element_i);
			correct = 0;
		}
	}

	return correct;
}

/*
 * Run all heap sort tests.
 */
static void test_heap_sorts()
{
	size_t test_i;
	for (test_i = 0; test_i < N_HEAP_SORT_TVS; test_i++) {
		printlg(INFO_LEVEL,
			"Heap sort test %u...\n", (unsigned) test_i);
		if (test_heap_sort(heap_sort_tvs[test_i])) {
			printlg(INFO_LEVEL, "Passed!\n\n");
		} else {
			printlg(ERROR_LEVEL, "Failed!\n\n");
		}
	}
}

int main(void)
{
	test_heap_sorts();

	return 0;
}
