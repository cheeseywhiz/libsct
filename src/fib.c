#include <stdlib.h>
#include <stdint.h>
#include "fib.h"
#include "ht.h"

static struct ht_hash_table *fib_table;

int fib_init_table(void) {
        int exit_code = 1;
        fib_table = malloc(sizeof(struct ht_hash_table));

        if (!fib_table) {
                goto exit;
        }

        if (ht_init(fib_table, ht_uint64_type)) {
                goto exit;
        }

        if (fibonacci(93) == (uint64_t) -1) {
                goto exit;
        }

        exit_code = 0;

exit:
        return exit_code;
}

uint64_t fibonacci(uint64_t n) {
        uint64_t *answer_ptr = ht_get(fib_table, &n, NULL);

        if (answer_ptr) {
                return *answer_ptr;
        }

        uint64_t answer;

        if (n <= 1) {
                answer = n;
        } else {
                answer = fibonacci(n - 1) + fibonacci(n - 2);
        }

        answer_ptr = malloc(sizeof(uint64_t));

        if (!answer_ptr) {
                return -1;
        }

        uint64_t *n_ptr = malloc(sizeof(uint64_t));

        if (!n_ptr) {
                free(answer_ptr);
                return -1;
        }

        *n_ptr = n;
        *answer_ptr = answer;

        if (ht_set_item(fib_table, n_ptr, answer_ptr)) {
                free(answer_ptr);
                free(n_ptr);
                return -1;
        }

        return answer;
}

void fib_free_table(void) {
        ht_free_all(fib_table);
        free(fib_table);
        fib_table = NULL;
}
