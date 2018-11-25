#include "test.h"

MODULE_TEST sll_test_main(void);
MODULE_TEST arr_test_main(void);
MODULE_TEST hash_test_main(void);

int main() {
        SCORE_INIT();
        MODULE_REPORT("sll.c", sll_test_main());
        MODULE_REPORT("arr.c", arr_test_main());
        MODULE_REPORT("hash.c", hash_test_main());
        MAIN_EXIT();
}
