#include <stdlib.h>
#include "sct_internal.h"
#include "test.h"
#include "arr.h"

UNIT_TEST test_init(struct array *array) {
        SCORE_INIT();
        int exit_code = arr_init(array);
        ASSERT(!exit_code);

        if (exit_code) {
                goto exit;
        }

        ASSERT(array->array);
        ASSERT(!array->length);
        ASSERT(array->size);

        ASSERT(arr_init(NULL));

exit:
        EXIT_TEST(5);
}

UNIT_TEST test_append(struct array *array, char **strings, size_t n_strings) {
        SCORE_INIT();
        ssize_t length = 0;

        for (size_t string_i = 0; string_i < n_strings; string_i++) {
                char *string = strings[string_i];
                int exit_code = arr_append(array, string);
                ASSERT(!exit_code);

                if (exit_code) {
                        goto exit;
                }

                length++;
                ASSERT(array->length == length);
                ASSERT(array->array[array->length - 1] == string);
        }

        ASSERT(arr_append(NULL, "null append"));

exit:
        EXIT_TEST((3 * n_strings) + 1);
}

UNIT_TEST test_get_index(struct array *array, char **strings) {
        SCORE_INIT();
        ASSERT(arr_get_index(array, 0) == strings[0]);
        ASSERT(arr_get_index(array, 1) == strings[1]);
        ASSERT(arr_get_index(array, 2) == strings[2]);
        ASSERT(arr_get_index(array, 3) == strings[3]);

        ASSERT(arr_get_index(array, -4) == strings[0]);
        ASSERT(arr_get_index(array, -3) == strings[1]);
        ASSERT(arr_get_index(array, -2) == strings[2]);
        ASSERT(arr_get_index(array, -1) == strings[3]);

        ASSERT(!arr_get_index(array, 5));
        ASSERT(!arr_get_index(array, -5));
        ASSERT(!arr_get_index(NULL, 0));

        struct array empty_list;

        if (arr_init(&empty_list)) {
                goto exit;
        }

        ASSERT(!arr_get_index(&empty_list, 0));

exit:
        arr_free(&empty_list);
        EXIT_TEST(12);
}

UNIT_TEST test_slice(void) {
        SCORE_INIT();
        struct array numbers, empty_list, slice;
        int exit_code;

        if (arr_init(&numbers)) {
                goto exit;
        }

        for (int i = 0; i < 100; i++) {
                int *item = malloc(sizeof(int));

                if (!item) {
                        goto exit;
                }

                *item = i;

                if (arr_append(&numbers, item)) {
                        goto exit;
                }
        }

        struct slice_case_group case_groups[] = SLICE_CASE_GROUPS;

        for (size_t group_i = 0; group_i < ARRAY_LENGTH(case_groups); group_i++) {
                struct slice_case_group case_group = case_groups[group_i];
                struct slice_args args = case_group.args;
                exit_code = arr_slice(&numbers, &slice, args.start, args.end, args.step);
                ASSERT(!exit_code);

                if (exit_code) {
                        continue;
                }

                for (size_t case_i = 0; case_i < ARRAY_LENGTH(case_group.cases); case_i++) {
                        struct slice_case case_ = case_group.cases[case_i];
                        int *observed = arr_get_index(&slice, case_.index);

                        if (!observed) {
                                continue;
                        }

                        ASSERT(case_.expected == *observed);
                }

                arr_free(&slice);
        }

        struct slice_args empty_cases[] = EMPTY_CASES;

        for (size_t null_i = 0; null_i < ARRAY_LENGTH(empty_cases); null_i++) {
                struct slice_args args = empty_cases[null_i];
                ASSERT(!arr_slice(&numbers, &slice, args.start, args.end, args.step));
                ASSERT(!slice.length);
                arr_free(&slice);
        }

        if (arr_init(&empty_list)) {
                goto exit;
        }

        /* Another empty case */
        ASSERT(!arr_slice(&empty_list, &slice, 2, 5, 1));
        ASSERT(!slice.length);
        arr_free(&slice);

        /* Error cases */
        ASSERT(arr_slice(NULL, &slice, 2, 5, 1));
        ASSERT(arr_slice(&numbers, NULL, 2, 5, 1));
        ASSERT(arr_slice(NULL, NULL, 2, 5, 1));
        ASSERT(arr_slice(&numbers, &slice, 2, 5, 0));

exit:
        arr_free_all(&numbers);
        arr_free(&empty_list);
        arr_free(&slice);
        EXIT_TEST(
                ARRAY_LENGTH(case_groups) * (1 + ARRAY_LENGTH(case_groups[0].cases))
                + (2 * ARRAY_LENGTH(empty_cases))
                + 6
        );
}

UNIT_TEST test_equals(struct array *array) {
        SCORE_INIT();
        struct array array_copy, numbers, empty_list, mid_slice, mid_slice_copy, beginning_slice;

        if (arr_init(&array_copy)) {
                goto exit;
        }

        for (ssize_t index = 0; index < array->length; index++) {
                if (arr_append(&array_copy, arr_get_index(array, index))) {
                        goto exit;
                }
        }

        ASSERT(arr_equals(array, &array_copy));
        ASSERT(arr_equals(&array_copy, array));  /* reflexive property */

        if (arr_init(&numbers)) {
                goto exit;
        }

        for (int number_i = 0; number_i < 100; number_i++) {
                int *item = malloc(sizeof(int));

                if (!item) {
                        goto exit;
                }

                *item = number_i;

                if (arr_append(&numbers, item)) {
                        goto exit;
                }
        }

        if (arr_init(&empty_list)) {
                goto exit;
        }

        ASSERT(!arr_equals(&numbers, &empty_list));
        ASSERT(!arr_equals(&empty_list, &numbers));

        ASSERT(arr_equals(array, array));  /* is equal to self */
        ASSERT(arr_equals(&empty_list, &empty_list));

        if (arr_slice(&numbers, &mid_slice, 50, 75, 1) ||
                        arr_slice(&numbers, &mid_slice_copy, 50, 75, 1)) {
                goto exit;
        }

        ASSERT(arr_equals(&mid_slice, &mid_slice_copy));
        ASSERT(arr_equals(&mid_slice_copy, &mid_slice));

        if (arr_slice(&numbers, &beginning_slice, 0, 25, 1)) {
                goto exit;
        }

        ASSERT(!arr_equals(&numbers, &beginning_slice));
        ASSERT(!arr_equals(&beginning_slice, &numbers));

        ASSERT(!arr_equals(&beginning_slice, &mid_slice));  /* same lengths */
        ASSERT(!arr_equals(&mid_slice, &beginning_slice));

        /* error cases */
        ASSERT(!arr_equals(&numbers, NULL));
        ASSERT(!arr_equals(NULL, &numbers));
        ASSERT(!arr_equals(NULL, NULL));

exit:
        arr_free(&array_copy);
        arr_free_all(&numbers);
        arr_free(&empty_list);
        arr_free(&mid_slice);
        arr_free(&mid_slice_copy);
        arr_free(&beginning_slice);
        EXIT_TEST(15);
}

UNIT_TEST test_copy(struct array *array) {
        SCORE_INIT();
        struct array copy, empty_list, empty_copy;
        int exit_code = arr_copy(array, &copy);
        ASSERT(!exit_code);

        if (exit_code) {
                goto exit;
        }

        ASSERT(arr_equals(array, &copy));
        arr_free(&copy);

        if (arr_init(&empty_list)) {
                goto exit;
        }

        exit_code = arr_copy(&empty_list, &empty_copy);
        ASSERT(!exit_code);

        if (exit_code) {
                goto exit;
        }

        ASSERT(arr_equals(&empty_list, &empty_copy));

        /* error cases */
        ASSERT(arr_copy(array, NULL));
        ASSERT(arr_copy(NULL, &copy));
        ASSERT(arr_copy(NULL, NULL));

exit:
        arr_free(&copy);
        arr_free(&empty_list);
        arr_free(&empty_copy);
        EXIT_TEST(7);
}

#define FAILSAFE() \
        if (FAILING) { \
                goto exit; \
        }

MODULE_TEST arr_test_main(void) {
        SCORE_INIT();
        char *strings[] = {
                "first",
                "second",
                "third",
                "fourth",
        };
        size_t n_strings = ARRAY_LENGTH(strings);
        struct array array;
        UNIT_REPORT("arr_init()", test_init(&array));
        FAILSAFE();
        UNIT_REPORT("arr_append()", test_append(&array, strings, n_strings));
        FAILSAFE();
        UNIT_REPORT("arr_get_index()", test_get_index(&array, strings));
        UNIT_REPORT("arr_slice()", test_slice());
        UNIT_REPORT("arr_equals()", test_equals(&array));
        UNIT_REPORT("arr_copy()", test_copy(&array));

exit:
        arr_free(&array);
        RETURN_SCORE();
}
