#include <string.h>
#include <stddef.h>
#include "sct_internal.h"
#include "ht.h"
#include "test.h"

struct key_value_pair {
	char *key;
	char *value;
};

int init_test_table(struct ht_hash_table *table) {
	if (ht_init(table, ht_string_type)) {
		return 1;
	}

	struct key_value_pair key_value_pairs[] = {
		{"hello", "world"},
		{"goodbye", "earth"},
		{"123", "numbers"},
		{"", "blank"},
		{"__doc__", "docstring"},
		{"__eq__", "equals"},
		{"__getitem__", "self explanatory"},
		{"__new__", "constructor"},
		{"__del__", "destructor"},
		{"__enter__", "context manager enter"},
		{"__exit__", "context manager exit"},
		{"__init__", "initializer"},
		{"__repr__", "representation"},
		{"__str__", "string"},
		{"__lt__", "less than"},
		{"__gt__", "greater than"},
		{"__hash__", "self explanatory"},
		{"__bool__", "boolin"},
	};

	for (size_t pair_i = 0; pair_i < ARRAY_LENGTH(key_value_pairs); pair_i++) {
		struct key_value_pair pair = key_value_pairs[pair_i];

		if (ht_set_item(table, pair.key, pair.value)) {
			return 1;
		}
	}

	return 0;
}

UNIT_TEST test_pop(void) {
	SCORE_INIT();
	struct ht_hash_table table;

	if (init_test_table(&table)) {
		goto exit;
	}

	struct key_value_pair pop_cases[] = {
		{"__doc__", "docstring"},
		{"123", "numbers"},
		{"__del__", "destructor"},
		{"goodbye", "earth"},
		{"__new__", "constructor"},
		{"__repr__", "representation"},
		{"__str__", "string"},
		{"__init__", "initializer"},
	};
	char sentinel;

	for (size_t case_i = 0; case_i < ARRAY_LENGTH(pop_cases); case_i++) {
		struct key_value_pair case_ = pop_cases[case_i];
		char *value = ht_pop(&table, case_.key, &sentinel);
		ASSERT(value != NULL);
		ASSERT(value != &sentinel);
		ASSERT(value == case_.value);
		ASSERT(!ht_contains(&table, case_.key));
	}

	ssize_t old_length = ht_length(&table);
	ASSERT(ht_pop(&table, "string that is not in table", &sentinel) == &sentinel);
	ASSERT(old_length == ht_length(&table));
	ASSERT(ht_pop(NULL, "", &sentinel) == NULL);
	ASSERT(!ht_pop(&table, NULL, &sentinel));

exit:
	ht_free(&table);
	EXIT_TEST(4 * ARRAY_LENGTH(pop_cases) + 4);
}

UNIT_TEST test_contains(void) {
	SCORE_INIT();
	struct ht_hash_table table;

	if (init_test_table(&table)) {
		goto exit;
	}

	char *contains_cases[] = {
		"__repr__",
		"__getitem__",
		"",
		"__new__",
		"hello",
		"__hash__",
		"__bool__",
		"__enter__",
	};

	for (size_t case_i = 0; case_i < ARRAY_LENGTH(contains_cases); case_i++) {
		char *key = contains_cases[case_i];
		ASSERT(ht_contains(&table, key));
	}

	ASSERT(!ht_contains(&table, "string that is not in table"));
	ASSERT(!ht_contains(&table, NULL));
	ASSERT(!ht_contains(NULL, NULL));

exit:
	ht_free(&table);
	EXIT_TEST(ARRAY_LENGTH(contains_cases) + 3);
}

UNIT_TEST test_get(void) {
	SCORE_INIT();
	struct ht_hash_table table;

	if (init_test_table(&table)) {
		goto exit;
	}

	char sentinel;
	struct key_value_pair get_cases[] = {
		{"hello", "world"},
		{"__lt__", "less than"},
		{"__del__", "destructor"},
		{"__init__", "initializer"},
		{"__repr__", "representation"},
		{"", "blank"},
		{"__enter__", "context manager enter"},
		{"goodbye", "earth"},
		{"5ub76PIa", &sentinel},
		{"84yRnBUb", &sentinel},
		{"HoWCFJ8w", &sentinel},
		{"oR9WDsbA", &sentinel},
		{"16JPglOU", &sentinel},
		{"3shVvUTG", &sentinel},
		{"7BG1Qmts", &sentinel},
		{"L4F2Gz96", &sentinel},
	};


	for (size_t case_i = 0; case_i < ARRAY_LENGTH(get_cases); case_i++) {
		struct key_value_pair case_ = get_cases[case_i];
		ASSERT(ht_get(&table, case_.key, &sentinel) == case_.value);
	}

	ASSERT(!ht_get(NULL, "", &sentinel));
	ASSERT(!ht_get(&table, NULL, &sentinel));

exit:
	ht_free(&table);
	EXIT_TEST(ARRAY_LENGTH(get_cases) + 2);
}

struct set_get_case {
	char *key;
	char *value1;
	char *value2;
};

UNIT_TEST test_set_get(void) {
	SCORE_INIT();
	struct ht_hash_table table;

	if (ht_init(&table, ht_string_type)) {
		goto exit;
	}

	struct set_get_case set_get_cases[] = {
		{"hello", NULL, "EpVZzAQl"},
		{"goodbye", "earth", "4SXRVxfC"},
		{"123", "numbers", "gXkH1zxF"},
		{"", NULL, "EctT2NL3"},
		{"__doc__", "docstring", "k9GoaXmI"},
		{"__eq__", NULL, "zGw2K7Dr"},
		{"__getitem__", "self explanatory", "MUayY20V"},
		{"__new__", NULL, "HXJnBdEx"},
		{"__del__", "destructor", "m8GAF0w6"},
		{"__enter__", NULL, NULL},
		{"__exit__", NULL, "OlJL83fZ"},
		{"__init__", "initializer", NULL},
		{"__repr__", "representation", NULL},
		{"__str__", NULL, "kqH9QrPT"},
		{"__lt__", "less than", "SuFEhbty"},
		{"__gt__", "greater than", "Ymqg7UGn"},
		{"__hash__", "self explanatory", NULL},
		{"__bool__", "boolin", "TGRBI1oO"},
	};

	for (size_t case_i = 0; case_i < ARRAY_LENGTH(set_get_cases); case_i++) {
		struct set_get_case case_ = set_get_cases[case_i];
		int exit_code;
		void *value;
		exit_code = ht_set_item(&table, case_.key, case_.value1);
		ASSERT(!exit_code);
		value = ht_get_item(&table, case_.key);
		ASSERT(value == case_.value1);
		exit_code = ht_set_item(&table, case_.key, case_.value2);
		ASSERT(!exit_code);
		value = ht_get_item(&table, case_.key);
		ASSERT(value == case_.value2);
	}

	ASSERT(ht_set_item(&table, NULL, "value"));
	ASSERT(ht_set_item(NULL, "key", "value"));
	ASSERT(ht_set_item(NULL, NULL, "value"));
	ASSERT(!ht_get_item(&table, "key"));
	ASSERT(!ht_get_item(NULL, "key"));
	ASSERT(!ht_get_item(&table, NULL));
	ASSERT(!ht_get_item(NULL, NULL));

exit:
	ht_free(&table);
	EXIT_TEST(4 * ARRAY_LENGTH(set_get_cases) + 7);
}

UNIT_TEST test_length(void) {
	SCORE_INIT();
	struct ht_hash_table table;

	if (ht_init(&table, ht_string_type)) {
		goto exit;
	}

	char *random_keys[] = {
		"HCo9PbzR",
		"KZuQOBdV",
		"tin4d9e2",
		"et68VkAK",
		"qiaJ42cL",
		"0n5KDk9X",
		"TNGcymwg",
		"nKR5BLmF",
	};
	ssize_t length = 0;
	size_t key_i = 0;

	for (size_t key_i = 0; key_i < ARRAY_LENGTH(random_keys); key_i++) {
		char *key = random_keys[key_i];
		ASSERT(ht_length(&table) == length);

		if (ht_set_item(&table, key, NULL)) {
			goto exit;
		}

		length++;
	}

	ASSERT(ht_length(&table) == length);
	ASSERT(ht_length(NULL) < 0);

exit:
	ht_free(&table);
	EXIT_TEST(ARRAY_LENGTH(random_keys) + 2);
}

UNIT_TEST test_popitem(void) {
	SCORE_INIT();
	struct ht_hash_table table;

	if (init_test_table(&table)) {
		goto exit;
	}

	struct key_value_pair popitem_cases[] = {
		{"__bool__", "boolin"},
		{"__hash__", "self explanatory"},
		{"__gt__", "greater than"},
		{"__lt__", "less than"},
		{"__str__", "string"},
		{"__repr__", "representation"},
		{"__init__", "initializer"},
		{"__exit__", "context manager exit"},
		{"__enter__", "context manager enter"},
		{"__del__", "destructor"},
		{"__new__", "constructor"},
		{"__getitem__", "self explanatory"},
		{"__eq__", "equals"},
		{"__doc__", "docstring"},
		{"", "blank"},
		{"123", "numbers"},
		{"goodbye", "earth"},
		{"hello", "world"},
	};

	for (size_t case_i = 0; case_i < ARRAY_LENGTH(popitem_cases); case_i++) {
		struct key_value_pair case_ = popitem_cases[case_i];
		struct ht_entry *entry = ht_popitem(&table);
		ASSERT(entry);

		if (!entry) {
			continue;
		}

		ASSERT(case_.key == entry->key);
		ASSERT(case_.value == entry->value);
	}

	ASSERT(!ht_popitem(&table));
	ASSERT(!ht_length(&table));
	ASSERT(!ht_popitem(NULL));

exit:
	ht_free(&table);
	EXIT_TEST(3 * ARRAY_LENGTH(popitem_cases) + 3);
}

UNIT_TEST test_clear(void) {
	SCORE_INIT();
	struct ht_hash_table table;

	if (init_test_table(&table)) {
		goto exit;
	}

	ASSERT(!ht_clear(&table));
	ASSERT(!ht_length(&table));
	ASSERT(ht_clear(NULL));

exit:
	ht_free(&table);
	EXIT_TEST(3);
}

UNIT_TEST test_equals(void) {
	SCORE_INIT();
	ht_equals_func string_equals = ht_string_type.key_equals;
	struct ht_hash_table table = {0}, table_copy = {0}, random_table = {0}, empty_table = {0};

	if (init_test_table(&table) \
			|| init_test_table(&table_copy) \
			|| init_test_table(&random_table) \
			|| ht_init(&empty_table, ht_string_type)) {
		goto exit;
	}

	struct key_value_pair random_pairs[] = {
		{"g8QysGFb", "uN3z5sbk"},
		{"m2uZKYiv", "BnrRg1yJ"},
		{"w641RIFX", "IpeUcfik"},
		{"k6InrWvM", "G1qxv5hs"},
	};

	for (size_t i = 0; i < ARRAY_LENGTH(random_pairs); i++) {
		if (!ht_popitem(&random_table)) {
			goto exit;
		}

		struct key_value_pair pair = random_pairs[i];

		if (ht_set_item(&random_table, pair.key, pair.value)) {
			goto exit;
		}
	}

	ASSERT(ht_equals(&table, &table, string_equals));
	ASSERT(ht_equals(&table, &table_copy, string_equals));
	ASSERT(ht_equals(&table_copy, &table, string_equals));
	ASSERT(ht_equals(&empty_table, &empty_table, string_equals));
	ASSERT(!ht_equals(&table, &random_table, string_equals));
	ASSERT(!ht_equals(&random_table, &table, string_equals));
	ASSERT(!ht_equals(&table, &empty_table, string_equals));
	ASSERT(!ht_equals(&empty_table, &table, string_equals));
	ASSERT(!ht_equals(&table, NULL, string_equals));
	ASSERT(!ht_equals(NULL, &table, string_equals));
	ASSERT(!ht_equals(NULL, NULL, string_equals));

exit:
	ht_free(&table);
	ht_free(&table_copy);
	ht_free(&random_table);
	ht_free(&empty_table);
	EXIT_TEST(11);
}

MODULE_TEST ht_test_main(void) {
	SCORE_INIT();
	UNIT_REPORT("ht_pop()", test_pop());
	UNIT_REPORT("ht_contains()", test_contains());
	UNIT_REPORT("ht_get()", test_get());
	UNIT_REPORT("ht_set_item()/ht_get_item()", test_set_get());
	UNIT_REPORT("ht_length()", test_length());
	UNIT_REPORT("ht_popitem()", test_popitem());
	UNIT_REPORT("ht_clear()", test_clear());
	UNIT_REPORT("ht_equals()", test_equals());
	RETURN_SCORE();
}
