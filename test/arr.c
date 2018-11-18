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

exit:
        arr_free(&array);
        RETURN_SCORE();
}
