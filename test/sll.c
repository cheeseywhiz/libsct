#include "test.h"
#include "exception.h"
#include "sll.h"

#define CHECK_APPEND(node) \
        if (!node) { \
                EXITFAIL(12); \
                goto exit; \
        }

SMALL_TEST test_append(void) {
        SCORE_INIT();
        char *first_str = "hello world";
        struct sll_node *first = NULL;
        sll_append(&first, first_str);
        CHECK_APPEND(first);
        ASSERT(first);
        ASSERT(first->ptr == first_str);

        char *second_str = "second world";
        struct sll_node *second = sll_append(&first, second_str);
        CHECK_APPEND(second);
        ASSERT(second);
        ASSERT(second->ptr == second_str);
        ASSERT(first->next == second);

        char *third_str = "Sudan (third world)";
        struct sll_node *third = sll_append(&first, third_str);
        CHECK_APPEND(third);
        ASSERT(third);
        ASSERT(third->ptr == third_str);
        ASSERT(second->next == third);

        char *fourth_str = "append not to first";
        struct sll_node *fourth = sll_append(&third, fourth_str);
        CHECK_APPEND(fourth);
        ASSERT(fourth);
        ASSERT(fourth->ptr == fourth_str);
        ASSERT(third->next == fourth);
        ASSERT(first->next->next->next == fourth);

exit:
        sll_shallow_free(&first);
        RETURN_SCORE();
}

BIG_TEST sll_test_main(void) {
        SCORE_INIT();
        SMALL_REPORT("sll_append()", test_append());
        RETURN_SCORE();
}
