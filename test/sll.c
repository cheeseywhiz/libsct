#include <stddef.h>
#include "test.h"
#include "exception.h"
#include "sll.h"

#define CHECK_APPEND(node) \
        ASSERT(node); \
        if (!(node)) { \
                EXITFAIL(12); \
                RETURN_SCORE(); \
        }

SMALL_TEST test_append(struct sll_node **first) {
        SCORE_INIT();
        char *first_str = "hello world";
        sll_append(first, first_str);
        CHECK_APPEND(*first);
        ASSERT((*first)->ptr == first_str);

        char *second_str = "second world";
        struct sll_node *second = sll_append(first, second_str);
        CHECK_APPEND(second);
        ASSERT(second->ptr == second_str);
        ASSERT((*first)->next == second);

        char *third_str = "Sudan (third world)";
        struct sll_node *third = sll_append(first, third_str);
        CHECK_APPEND(third);
        ASSERT(third->ptr == third_str);
        ASSERT(second->next == third);

        char *fourth_str = "append not to first";
        struct sll_node *fourth = sll_append(&third, fourth_str);
        CHECK_APPEND(fourth);
        ASSERT(fourth->ptr == fourth_str);
        ASSERT(third->next == fourth);
        ASSERT((*first)->next->next->next == fourth);

        RETURN_SCORE();
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

#define CHECK_GET_INDEX(node) \
        ASSERT(node); \
        if (!(node)) { \
                EXITFAIL(11); \
                RETURN_SCORE(); \
        }

SMALL_TEST test_get_index(struct sll_node **list) {
        SCORE_INIT();
        struct sll_node *first = sll_get_index(list, 0);
        CHECK_GET_INDEX(first);
        ASSERT(first == *list);
        struct sll_node *second = sll_get_index(list, 1);
        CHECK_GET_INDEX(second);
        ASSERT(second == first->next);
        struct sll_node *third = sll_get_index(list, 2);
        CHECK_GET_INDEX(third);
        ASSERT(third == second->next);
        struct sll_node *fourth = sll_get_index(list, 3);
        CHECK_GET_INDEX(fourth);
        ASSERT(fourth == third->next);

        ASSERT(sll_get_index(list, 4) == NULL);
        ASSERT(sll_get_index(list, 5) == NULL);

        struct sll_node *empty_list = NULL;
        ASSERT(sll_get_index(&empty_list, 0) == NULL);

        RETURN_SCORE();
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
        RETURN_SCORE();
}
