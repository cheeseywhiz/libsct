#include "test.h"

MODULE_TEST sll_test_main(void);

int main() {
        SCORE_INIT();
        MODULE_REPORT("sll.c", sll_test_main());
        MAIN_EXIT();
}
