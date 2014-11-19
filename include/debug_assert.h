/*
 * Wrappers for assert function that only runs if DEBUG is defined.
 * Convenient if only want one or two assert lines.
 */
#ifndef DEBUG_ASSERT_H
#define DEBUG_ASSERT_H

#include <assert.h>

/* just a wrapper around a single "assert" call */
#ifdef DEBUG
#define debug_assert(a) assert(a)
#else
#define debug_assert(a)
#endif

/*
 * assertion that is only checked if condition is met,
 * ie. assert that "condition implies consequence" holds
 * condition:	the condition for checking the consequence
 * consequence:	the expression whose truth value is checked
 *		iff "condition" is true
 */
#ifdef DEBUG
#define debug_assert_imply(condition, consequence) do { \
	if (condition) { \
		debug_assert(consequence); \
	} \
} while (0)
#else
#define debug_assert_imply(condition, consequence)
#endif

#endif /* DEBUG_ASSERT_H */
