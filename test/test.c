#include "test.h"

BIG_TEST sll_test_main(void);

int main() {
        SCORE_INIT();
        BIG_REPORT("sll.c", sll_test_main());
        MAIN_EXIT();
}
