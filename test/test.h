#ifndef TEST_H
#define TEST_H

#include <stdio.h>

struct score {
        size_t passing;
        size_t failing;
};

#define BIG_TEST struct score
#define SMALL_TEST static struct score

#define SCORE(pass, fail) \
        (struct score) { \
                .passing = pass, \
                .failing = fail, \
        }

#define NEW_SCORE() SCORE(0, 0)

#define SCORE_INIT() struct score subscore, score_local = NEW_SCORE()
#define PASSING score_local.passing
#define FAILING score_local.failing
#define ADD_PASS(n) PASSING += n
#define ADD_FAIL(n) FAILING += n
#define ADD_SCORE(pass, fail) \
        ADD_PASS(pass); \
        ADD_FAIL(fail)
#define PASS() ADD_PASS(1)
#define FAIL() ADD_FAIL(1)
#define ASSERT(bool) if (bool) { PASS(); } else { FAIL(); }
#define SUBSCORE(score) \
        subscore = score; \
        ADD_SCORE(subscore.passing, subscore.failing)
#define RETURN_SCORE() return score_local
#define EXIT_TEST(n_tests) \
        ADD_FAIL(n_tests - (PASSING + FAILING)); \
        RETURN_SCORE()

#define REPORT_SCORE(passing, failing) printf("Pass: %ld\nFail: %ld\n", passing, failing)
#define REPORT_SUBSCORE() REPORT_SCORE(subscore.passing, subscore.failing)
#define REPORT_CURRENT() REPORT_SCORE(PASSING, FAILING)

#define SMALL_REPORT(name, score) \
        printf("%s:\n", name); \
        SUBSCORE(score); \
        REPORT_SUBSCORE()

#define BIG_REPORT(name, score) \
        printf("%s:\n", name); \
        SUBSCORE(score); \
        printf("%s summary:\n", name); \
        REPORT_SUBSCORE(); \
        printf("\n")

#define MAIN_EXIT() \
        puts("test summary:"); \
        REPORT_CURRENT(); \
        return FAILING < 255 || FAILING % 256 ? FAILING : 255

#endif
