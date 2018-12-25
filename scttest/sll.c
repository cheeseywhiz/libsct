#include <stddef.h>
#include <stdlib.h>
#include "sct_internal.h"
#include "test.h"
#include "sll.h"
#include "xstdlib.h"

#define CHECK_NODE(node) \
	if (!(node)) { \
		goto exit; \
	}

#define ASSERT_NODE(node) \
	ASSERT(node); \
	CHECK_NODE(node)

UNIT_TEST test_append(struct sll_node **first, char **strings) {
	SCORE_INIT();
	sll_append(first, strings[0]);
	ASSERT_NODE(*first);
	ASSERT((*first)->ptr == strings[0]);

	struct sll_node *second = sll_append(first, strings[1]);
	ASSERT_NODE(second);
	ASSERT(second->ptr == strings[1]);
	ASSERT((*first)->next == second);

	struct sll_node *third = sll_append(first, strings[2]);
	ASSERT_NODE(third);
	ASSERT(third->ptr == strings[2]);
	ASSERT(second->next == third);

	struct sll_node *fourth = sll_append(&third, strings[3]);
	ASSERT_NODE(fourth);
	ASSERT(fourth->ptr == strings[3]);
	ASSERT(third->next == fourth);
	ASSERT((*first)->next->next->next == fourth);

exit:
	EXIT_TEST(12);
}

UNIT_TEST test_length(struct sll_node **list) {
	SCORE_INIT();
	ASSERT(sll_length(list) == 4);
	ASSERT(sll_length(&((*list)->next)) == 3);
	ASSERT(sll_length(&((*list)->next->next)) == 2);
	ASSERT(sll_length(&((*list)->next->next->next)) == 1);
	ASSERT(sll_length(&((*list)->next->next->next->next)) == 0);
	RETURN_SCORE();
}

UNIT_TEST test_get_index(struct sll_node **list) {
	SCORE_INIT();
	struct sll_node *first = sll_get_index(list, 0);
	ASSERT_NODE(first);
	ASSERT(first == *list);
	struct sll_node *second = sll_get_index(list, 1);
	ASSERT_NODE(second);
	ASSERT(second == first->next);
	struct sll_node *third = sll_get_index(list, 2);
	ASSERT_NODE(third);
	ASSERT(third == second->next);
	struct sll_node *fourth = sll_get_index(list, 3);
	ASSERT_NODE(fourth);
	ASSERT(fourth == third->next);

	ASSERT(sll_get_index(list, 4) == NULL);
	ASSERT(sll_get_index(list, 5) == NULL);

	struct sll_node *empty_list = NULL;
	ASSERT(sll_get_index(&empty_list, 0) == NULL);

	struct sll_node *fourth_alias = sll_get_index(list, -1);
	ASSERT_NODE(fourth_alias);
	ASSERT(fourth_alias == fourth);
	struct sll_node *third_alias = sll_get_index(list, -2);
	ASSERT_NODE(third_alias);
	ASSERT(third_alias == third);
	struct sll_node *second_alias = sll_get_index(list, -3);
	ASSERT_NODE(second_alias);
	ASSERT(second_alias == second);
	struct sll_node *first_alias = sll_get_index(list, -4);
	ASSERT_NODE(first_alias);
	ASSERT(first_alias == first);

	ASSERT(sll_get_index(list, -5) == NULL);
	ASSERT(sll_get_index(list, -6) == NULL);
	ASSERT(sll_get_index(&empty_list, -2) == NULL);

exit:
	EXIT_TEST(22);
}

UNIT_TEST test_set_index(void) {
	SCORE_INIT();
	struct sll_node *numbers = NULL;

	for (size_t i = 0; i < 100; i++) {
		CHECK_NODE(sll_append(&numbers, NULL));
	}

	ssize_t cases[] = {66, -16, -41, 0, -61, 61, -78, 75, -9, -12, -56, -69, -15, -60, 20, 10};

	for (size_t case_i = 0; case_i < ARRAY_LENGTH(cases); case_i++) {
		ssize_t *item = xmalloc(sizeof(ssize_t));

		if (!item) {
			goto case_end;
		}

		ssize_t case_ = cases[case_i];
		*item = case_;
		int exit_code = sll_set_index(&numbers, case_, item);
		ASSERT(!exit_code);

		if (exit_code) {
			goto case_end;
		}

		struct sll_node *node = sll_get_index(&numbers, case_);

		if (!node) {
			goto case_end;
		}

		ASSERT(node->ptr == item);

case_end:
		free(item);
	}

	struct sll_node *empty_list = NULL;
	ASSERT(sll_set_index(&numbers, 101, NULL));
	ASSERT(sll_set_index(&empty_list, 0, NULL));

exit:
	sll_shallow_free(&numbers);
	EXIT_TEST(2 * ARRAY_LENGTH(cases) + 2);
}

UNIT_TEST test_slice(void) {
	SCORE_INIT();
	struct sll_node *numbers = NULL;

	for (int i = 0; i < 100; i++) {
		struct sll_node *new_node = sll_append(&numbers, xmalloc(sizeof(int)));

		if (!(new_node && new_node->ptr)) {
			goto exit;
		}

		*((int*) new_node->ptr) = i;
	}

	struct slice_case_group case_groups[] = SLICE_CASE_GROUPS;

	for (size_t group_i = 0; group_i < ARRAY_LENGTH(case_groups); group_i++) {
		struct slice_case_group case_group = case_groups[group_i];
		struct slice_args args = case_group.args;
		struct sll_node *slice = sll_slice(&numbers, args.start, args.end, args.step);
		ASSERT(slice);

		if (!slice) {
			continue;
		}

		for (size_t case_i = 0; case_i < ARRAY_LENGTH(case_group.cases); case_i++) {
			struct slice_case case_ = case_group.cases[case_i];
			struct sll_node *item = sll_get_index(&slice, case_.index);

			if (!item) {
				continue;
			}

			int observed = *((int*) item->ptr);
			ASSERT(case_.expected == observed);
		}

		sll_shallow_free(&slice);
	}

	struct slice_args empty_cases[] = EMPTY_CASES;

	for (size_t null_i = 0; null_i < ARRAY_LENGTH(empty_cases); null_i++) {
		struct slice_args args = empty_cases[null_i];
		struct sll_node *slice = sll_slice(&numbers, args.start, args.end, args.step);
		ASSERT(!slice);

		if (slice) {
			sll_shallow_free(&slice);
		}
	}

	/* Another empty case */
	struct sll_node *empty_list = NULL;
	ASSERT(!sll_slice(&empty_list, 2, 5, 1));

	/* error case */
	ASSERT(!sll_slice(&numbers, 2, 5, 0));

exit:
	sll_free_all(&numbers);
	EXIT_TEST(
		ARRAY_LENGTH(case_groups) * (1 + ARRAY_LENGTH(case_groups[0].cases))
		+ ARRAY_LENGTH(empty_cases)
		+ 2
	);
}

UNIT_TEST test_equals(struct sll_node *list, char **strings, size_t n_strings) {
	SCORE_INIT();
	struct sll_node *list_copy = NULL, *numbers = NULL, *mid_slice = NULL, *mid_slice_copy = NULL,
	                *beginning_slice = NULL, *empty_list = NULL, *item;

	for (size_t string_i = 0; string_i < n_strings; string_i++) {
		CHECK_NODE(sll_append(&list_copy, strings[string_i]));
	}

	ASSERT(sll_equals(&list, &list_copy));
	ASSERT(sll_equals(&list_copy, &list));  /* reflexive property */

	for (size_t big_i = 0; big_i < 100; big_i++) {
		item = sll_append(&numbers, xmalloc(sizeof(size_t)));

		if (!(item && item->ptr)) {
			goto exit;
		}

		*((size_t*) item->ptr) = big_i;
	}

	ASSERT(!sll_equals(&numbers, &empty_list));
	ASSERT(!sll_equals(&empty_list, &numbers));

	mid_slice = sll_slice(&numbers, 50, 75, 1);
	mid_slice_copy = sll_slice(&numbers, 50, 75, 1);
	CHECK_NODE(mid_slice);
	CHECK_NODE(mid_slice_copy);
	ASSERT(sll_equals(&mid_slice, &mid_slice_copy));
	ASSERT(sll_equals(&mid_slice_copy, &mid_slice));

	beginning_slice = sll_slice(&numbers, 0, 25, 1);
	CHECK_NODE(beginning_slice);
	ASSERT(!sll_equals(&numbers, &beginning_slice));
	ASSERT(!sll_equals(&beginning_slice, &numbers));

	ASSERT(!sll_equals(&beginning_slice, &mid_slice));  /* same lengths */
	ASSERT(!sll_equals(&mid_slice, &beginning_slice));

	ASSERT(sll_equals(&numbers, &numbers));  /* is equal to self */
	ASSERT(sll_equals(&empty_list, &empty_list));

exit:
	sll_shallow_free(&list_copy);
	sll_free_all(&numbers);
	sll_shallow_free(&mid_slice);
	sll_shallow_free(&mid_slice_copy);
	sll_shallow_free(&beginning_slice);
	EXIT_TEST(12);
}

UNIT_TEST test_copy(struct sll_node *list) {
	SCORE_INIT();
	struct sll_node *list_copy = sll_copy(&list);
	ASSERT_NODE(list_copy);
	ASSERT(sll_equals(&list, &list_copy));

	struct sll_node *empty_list = NULL;
	struct sll_node *empty_copy = sll_copy(&empty_list);
	ASSERT(!empty_copy);

exit:
	sll_shallow_free(&list_copy);
	EXIT_TEST(3);
}

UNIT_TEST test_find(struct sll_node *list, char **strings, size_t n_strings) {
	SCORE_INIT();

	for (size_t string_i = 0; string_i < n_strings; string_i++) {
		char *string = strings[string_i];
		ssize_t index = sll_find(&list, string);
		ASSERT(index >= 0);
		struct sll_node *item = sll_get_index(&list, index);

		if (item) {
			ASSERT(string == item->ptr);
		}
	}

	ASSERT(sll_find(&list, "random string") < 0);
	ASSERT(sll_find(&list, NULL) < 0);

	struct sll_node *empty_list = NULL;
	ASSERT(sll_find(&empty_list, "something else") < 0);
	ASSERT(sll_find(&empty_list, NULL) < 0);

	EXIT_TEST((2 * n_strings) + 4);
}

#define POP_CASE(list_i, string_i) \
	observed = sll_pop(list, (list_i)); \
	expected = strings[(string_i)]; \
	if (observed) { \
		ASSERT(observed == expected); \
	}

UNIT_TEST test_pop(struct sll_node **list, char **strings) {
	SCORE_INIT();
	struct sll_node *empty_list = NULL;
	ASSERT(!sll_pop(&empty_list, 0));
	ASSERT(!sll_pop(list, 5));  /* out of range */

	char *observed, *expected;
	POP_CASE(-2, 2);
	POP_CASE(0, 0);
	POP_CASE(1, 3);
	POP_CASE(0, 1);
	ASSERT(!*list);
	EXIT_TEST(7);
}

MODULE_TEST sll_test_main(void) {
	SCORE_INIT();
	char *strings[] = {
		"hello world",
		"second world",
		"Sudan (third world)",
		"append not to first",
	};
	size_t n_strings = ARRAY_LENGTH(strings);
	struct sll_node *list = NULL;
	UNIT_REPORT("sll_append()", test_append(&list, strings));
	CHECK_NODE(list);
	UNIT_REPORT("sll_length()", test_length(&list));
	UNIT_REPORT("sll_get_index()", test_get_index(&list));
	UNIT_REPORT("sll_set_index()", test_set_index());
	UNIT_REPORT("sll_slice()", test_slice());
	UNIT_REPORT("sll_equals()", test_equals(list, strings, n_strings));
	UNIT_REPORT("sll_copy()", test_copy(list));
	UNIT_REPORT("sll_find()", test_find(list, strings, n_strings));
	UNIT_REPORT("sll_pop()", test_pop(&list, strings));

exit:
	sll_shallow_free(&list);
	RETURN_SCORE();
}
