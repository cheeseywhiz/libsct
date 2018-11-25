#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "sct_internal.h"
#include "test.h"
#include "hash.h"

struct string_case {
        char *string;
        uint64_t expected_hash;
};

struct int_case {
        int number;
        uint64_t expected_hash;
};

UNIT_TEST test_fnv_hash(void) {
        SCORE_INIT();
        struct string_case string_cases[] = {
                {"Hello World", 0x3d58dee72d4e0c27},
                {"hello world", 0x779a65e7023cd2e7},
                {"hello worle", 0x779a64e7023cd134},
                {"", 0xcbf29ce484222325},
                {"abc", 0xe71fa2190541574b},
                {"123", 0x456fc2181822c4db},
                {"1234", 0x1fabbdf10314a21d},
                {"The quick brown fox jumps over the lazy dog.", 0x75c4d4d9092c6c5a},
                {"abcdefghijklmnopqrstuvwxyz", 0x8450deb1cdc382a2},
        };
        struct int_case int_cases[] = {
                {0x0, 0x4d25767f9dce13f5},
                {0x1, 0xad2aca7747985764},
                {0x64, 0xcc9047690baeb3d1},
                {0xf4240, 0x64dfa8965b6fc8c8},
                {0x400, 0x2a7f8a7f8a2e1949},
                {0x401, 0x8a84de7733f85cb8},
                {0x800, 0x7d99e7f768e1e9d},
                {0x801, 0x67def2772058620c},
                {0x7effffff, 0x994ef5653e295e1e},
                {0x7fffffff, 0x994ef6653e295fd1},
                {0x7ffffffe, 0x7053f67088da8040},
        };

        for (size_t string_i = 0; string_i < ARRAY_LENGTH(string_cases); string_i++) {
                struct string_case case_ = string_cases[string_i];
                uint64_t observed_hash = fnv_hash(
                        (unsigned char*) case_.string,
                        strlen(case_.string)
                );
                ASSERT(case_.expected_hash == observed_hash);
        }

        for (size_t int_i = 0; int_i < ARRAY_LENGTH(int_cases); int_i++) {
                struct int_case case_ = int_cases[int_i];
                uint64_t observed_hash = fnv_hash(
                        (unsigned char*) (&case_.number),
                        sizeof(case_.number)
                );
                ASSERT(case_.expected_hash == observed_hash);
        }

        EXIT_TEST(ARRAY_LENGTH(string_cases) + ARRAY_LENGTH(int_cases));
}

MODULE_TEST hash_test_main(void) {
        SCORE_INIT();
        UNIT_REPORT("fnv_hash()", test_fnv_hash());
        RETURN_SCORE();
}
