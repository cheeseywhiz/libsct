#include <stddef.h>
#include "test.h"
#include "exception.h"
#include "sll.h"

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

#define CHECK_NODE(node) \
        if (!(node)) { \
                goto exit; \
        }

#define ASSERT_NODE(node) \
        ASSERT(node); \
        CHECK_NODE(node)

SMALL_TEST test_append(struct sll_node **first) {
        SCORE_INIT();
        char *first_str = "hello world";
        sll_append(first, first_str);
        ASSERT_NODE(*first);
        ASSERT((*first)->ptr == first_str);

        char *second_str = "second world";
        struct sll_node *second = sll_append(first, second_str);
        ASSERT_NODE(second);
        ASSERT(second->ptr == second_str);
        ASSERT((*first)->next == second);

        char *third_str = "Sudan (third world)";
        struct sll_node *third = sll_append(first, third_str);
        ASSERT_NODE(third);
        ASSERT(third->ptr == third_str);
        ASSERT(second->next == third);

        char *fourth_str = "append not to first";
        struct sll_node *fourth = sll_append(&third, fourth_str);
        ASSERT_NODE(fourth);
        ASSERT(fourth->ptr == fourth_str);
        ASSERT(third->next == fourth);
        ASSERT((*first)->next->next->next == fourth);

exit:
        EXIT_TEST(12);
}

SMALL_TEST test_length(struct sll_node **list) {
        SCORE_INIT();
        ASSERT(sll_length(list) == 4);
        ASSERT(sll_length(&((*list)->next)) == 3);
        ASSERT(sll_length(&((*list)->next->next)) == 2);
        ASSERT(sll_length(&((*list)->next->next->next)) == 1);
        ASSERT(sll_length(&((*list)->next->next->next->next)) == 0);
        RETURN_SCORE();
}

SMALL_TEST test_get_index(struct sll_node **list) {
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

struct slice_args {
        ssize_t start;
        ssize_t end;
        ssize_t step;
};

struct slice_case {
        ssize_t index;
        int expected;
};

struct slice_case_group {
        struct slice_args args;
        struct slice_case cases[3];
};

SMALL_TEST test_slice(void) {
        SCORE_INIT();
        struct sll_node *numbers = NULL;

        for (int i = 0; i < 100; i++) {
                struct sll_node *new_node = sll_append(&numbers, malloc(sizeof(int)));

                if (!(new_node && new_node->ptr)) {
                        goto exit;
                }

                *((int*) new_node->ptr) = i;
        }

        struct slice_case_group case_groups[] = {
                {{10, 50, 1}, {     /* Basic usage */
                        {0, 10},
                        {1, 11},
                        {-1, 49},
                }},
                {{50, 10, -1}, {
                        {0, 50},
                        {1, 49},
                        {-1, 11},
                }},
                {{10, 45, 10}, {    /* Non-one step */
                        {0, 10},
                        {1, 20},
                        {-1, 40},
                }},
                {{45, 10, -10}, {
                        {0, 45},
                        {1, 35},
                        {-1, 15},
                }},
                {{-40, -10, 2}, {   /* Negative indices */
                        {0, 60},
                        {1, 62},
                        {-1, 88},
                }},
                {{-10, -40, -2}, {
                        {0, 90},
                        {1, 88},
                        {-1, 62},
                }},
                {{-200, 200, 5}, {  /* Iterate through right and left bounds */
                        {0, 0},
                        {1, 5},
                        {-1, 95},
                }},
                {{200, -200, -5}, {
                        {0, 99},
                        {1, 94},
                        {-1, 4},
                }},
        };

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

        struct slice_args null_cases[] = {
                {10, 50, 0},    /* zero step */
                {200, 300, 1},  /* out of bounds */
                {300, 200, -1},
                {-300, -200, 1},
                {-200, 300, -1},
                {50, 10, 1},    /* start > end */
                {-50, -90, 1},
                {10, 50, -1},   /* start < end on reverse slice */
                {-90, -50, -1},
        };

        for (size_t null_i = 0; null_i < ARRAY_LENGTH(null_cases); null_i++) {
                struct slice_args args = null_cases[null_i];
                struct sll_node *slice = sll_slice(&numbers, args.start, args.end, args.step);
                ASSERT(!slice);

                if (slice) {
                        sll_shallow_free(&slice);
                }
        }

        struct sll_node *empty_list = NULL;
        ASSERT(sll_slice(&empty_list, 2, 5, 1) == NULL);   /* Slice on empty list is empty list */

exit:
        sll_free_all(&numbers);
        EXIT_TEST(
                ARRAY_LENGTH(case_groups) * (1 + ARRAY_LENGTH(case_groups[0].cases))
                + ARRAY_LENGTH(null_cases)
                + 1
        );
}

BIG_TEST sll_test_main(void) {
        SCORE_INIT();
        struct sll_node *list = NULL;
        SMALL_REPORT("sll_append()", test_append(&list));

        if (!list) {
                RETURN_SCORE();
        }

        SMALL_REPORT("sll_length()", test_length(&list));
        SMALL_REPORT("sll_get_index()", test_get_index(&list));
        sll_shallow_free(&list);
        SMALL_REPORT("sll_slice()", test_slice());
        RETURN_SCORE();
}
