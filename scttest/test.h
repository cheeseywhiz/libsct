#ifndef TEST_H
#define TEST_H

#include <stddef.h>
#include <stdio.h>
#include "sct_internal.h"

struct score {
	size_t passing;
	size_t failing;
};

#define MODULE_TEST struct score
#define UNIT_TEST static struct score

#define SCORE(pass, fail) \
	(struct score) { \
		.passing = (pass), \
		.failing = (fail), \
	}

#define NEW_SCORE() SCORE(0, 0)
#define REPORT_PASS_FAIL(passing, failing) printf("Pass: %ld\nFail: %ld\n", passing, failing)
#define REPORT_SCORE(name) REPORT_PASS_FAIL(name.passing, name.failing)

#define SCORE_INIT() struct score subscore, score_local = NEW_SCORE()
#define PASSING score_local.passing
#define FAILING score_local.failing
#define ADD_PASS(n) PASSING += (n)
#define ADD_FAIL(n) FAILING += (n)

#define ADD_SCORE(pass, fail) \
	ADD_PASS(pass); \
	ADD_FAIL(fail)

#define PASS() ADD_PASS(1)
#define FAIL() ADD_FAIL(1)

#define ASSERT(bool) \
	if (bool) { \
		PASS(); \
	} else { \
		STDERR("Fail on line %d (%s)\n", __LINE__, __FILE__); \
		FAIL(); \
	}

#define SUBSCORE(score) \
	subscore = (score); \
	ADD_SCORE(subscore.passing, subscore.failing)

#define RETURN_SCORE() return score_local

#define EXIT_TEST(n_tests) \
	ADD_FAIL((n_tests) - (PASSING + FAILING)); \
	RETURN_SCORE()

#define REPORT_SUBSCORE() REPORT_SCORE(subscore)
#define REPORT_CURRENT() REPORT_SCORE(score_local)

#define UNIT_REPORT(name, score) \
	printf("%s:\n", (name)); \
	SUBSCORE(score); \
	REPORT_SUBSCORE()

#define MODULE_REPORT(name, score) \
	printf("%s:\n", (name)); \
	SUBSCORE(score); \
	printf("%s summary:\n", (name)); \
	REPORT_SUBSCORE(); \
	putchar('\n')

#define MAIN_EXIT() \
	puts("test summary:"); \
	REPORT_CURRENT(); \
	return FAILING < 255 || FAILING % 256 ? FAILING : 255

struct slice_args {
	ssize_t start;
	ssize_t end;
	ssize_t step;
};

struct slice_case {
	ssize_t index;
	int expected;
};

struct slice_case_group {
	struct slice_args args;
	struct slice_case cases[3];
};

/* struct slice_case_group case_groups[] */
#define SLICE_CASE_GROUPS { \
		{{10, 50, 1}, {     /* Basic usage */ \
			{0, 10}, \
			{1, 11}, \
			{-1, 49}, \
		}}, \
		{{50, 10, -1}, { \
			{0, 50}, \
			{1, 49}, \
			{-1, 11}, \
		}}, \
		{{10, 45, 10}, {    /* Non-one step */ \
			{0, 10}, \
			{1, 20}, \
			{-1, 40}, \
		}}, \
		{{45, 10, -10}, { \
			{0, 45}, \
			{1, 35}, \
			{-1, 15}, \
		}}, \
		{{-40, -10, 2}, {   /* Negative indices */ \
			{0, 60}, \
			{1, 62}, \
			{-1, 88}, \
		}}, \
		{{-10, -40, -2}, { \
			{0, 90}, \
			{1, 88}, \
			{-1, 62}, \
		}}, \
		{{-200, 200, 5}, {  /* Iterate through right and left bounds */ \
			{0, 0}, \
			{1, 5}, \
			{-1, 95}, \
		}}, \
		{{200, -200, -5}, { \
			{0, 99}, \
			{1, 94}, \
			{-1, 4}, \
		}}, \
	}

/* struct slice_args null_cases[]
   Slices that should return an empty list */
#define EMPTY_CASES { \
		{200, 300, 1},  /* out of bounds */ \
		{300, 200, -1}, \
		{-300, -200, 1}, \
		{-200, 300, -1}, \
		{50, 10, 1},    /* start > end */ \
		{-50, -90, 1}, \
		{10, 50, -1},   /* start < end on reverse slice */ \
		{-90, -50, -1}, \
	}

#endif
