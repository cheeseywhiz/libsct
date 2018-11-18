#include "test.h"
#include "arr.h"

UNIT_TEST test_hello(void) {
        SCORE_INIT();
        ASSERT(arr_hello(5) == 6);
        EXIT_TEST(1);
}

MODULE_TEST arr_test_main(void) {
        SCORE_INIT();
        UNIT_REPORT("arr_hello()", test_hello());
        RETURN_SCORE();
}
