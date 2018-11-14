#include "test.h"
#include "sll.h"

struct case_plus_one {
        int x;
        int y;
};

SMALL_TEST test_plus_one(void) {
        SCORE_INIT();
        int n_cases = 3;
        struct case_plus_one cases[] = {
                {0, 1},
                {2, 3},
                {5, 4},
        };

        for (int i = 0; i < n_cases; i++) {
                struct case_plus_one case_ = cases[i];
                ASSERT(plus_one(case_.x) == case_.y);
        }

        RETURN_SCORE();
}

BIG_TEST sll_test_main(void) {
        SCORE_INIT();
        SMALL_REPORT("plus_one()", test_plus_one());
        RETURN_SCORE();
}
