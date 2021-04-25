#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <urlrouter.h>

#define mu_desc (char*)__func__
#define mu_assert(message, test) do { if(!(test)) return message; } while(0)
#define mu_run_test(test) do { char* message = test(); tests_run++; if(message) return message; } while(0)

uint32_t tests_run;
path_t* path = NULL;

static char * test_create_basic_path() {
	uint8_t res = create_path(&path, "/users/nils", 1, '{', '}');
	mu_assert(mu_desc, res == MATCHER_SUCCESS);
	return 0;
}

static char * test_create_placeholder_path() {
	uint8_t res = create_path(&path, "/users/{username}", 2, '{', '}');
	mu_assert(mu_desc, res == MATCHER_SUCCESS);
	return 0;
}

static char * test_create_placeholder_path1() {
	uint8_t res = create_path(&path, "/users/{username}/friends/{friend}", 3, '{', '}');
	mu_assert(mu_desc, res == MATCHER_SUCCESS);
	return 0;
}

static char * test_find_not_found() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/invalid");
	mu_assert(mu_desc, matched == 0);
	return 0;
}

static char * test_find_basic_url() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/users/nils");
	mu_assert(mu_desc, matched == 1);
	return 0;
}

static char * test_find_basic_terminator() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/users/nils?q=abc");
	mu_assert(mu_desc, matched == 1);
	return 0;
}

static char * test_find_trailing_separator() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/users/nils/");
	mu_assert(mu_desc, matched == 1);
	return 0;
}

static char * test_find_trailing_separator_and_terminator() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/users/nils/?");
	mu_assert(mu_desc, matched == 1);
	return 0;
}

static char * test_find_placeholder() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/users/max");
	mu_assert(mu_desc, matched == 2);
	mu_assert(mu_desc, placeholders != NULL);
	if(placeholders != NULL) {
		mu_assert(mu_desc, strcmp("username", placeholders->name) == 0);
		mu_assert(mu_desc, strcmp("max", placeholders->value) == 0);
		mu_assert(mu_desc, placeholders->next == NULL);
		free_placeholder_list(placeholders);
	}
	return 0;
}

static char * test_find_placeholder_with_trailing_separator() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/users/max/");
	mu_assert(mu_desc, matched == 2);
	mu_assert(mu_desc, placeholders != NULL);
	if(placeholders != NULL) {
		mu_assert(mu_desc, strcmp("username", placeholders->name) == 0);
		mu_assert(mu_desc, strcmp("max", placeholders->value) == 0);
		mu_assert(mu_desc, placeholders->next == NULL);
		free_placeholder_list(placeholders);
	}
	return 0;
}

static char * test_find_placeholder_with_trailing_separator_and_terminator() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/users/max/?q=abc");
	mu_assert(mu_desc, matched == 2);
	mu_assert(mu_desc, placeholders != NULL);
	if(placeholders != NULL) {
		mu_assert(mu_desc, strcmp("username", placeholders->name) == 0);
		mu_assert(mu_desc, strcmp("max", placeholders->value) == 0);
		mu_assert(mu_desc, placeholders->next == NULL);
		free_placeholder_list(placeholders);
	}
	return 0;
}

static char * test_find_multiple_placeholder() {
	uint64_t matched;
	placeholder_t* placeholders;
	int c = find_url(&matched, &placeholders, path, "/users/max/friends/nils");
	mu_assert(mu_desc, matched == 3);
	mu_assert(mu_desc, placeholders != NULL);
	if(placeholders != NULL) {
		mu_assert(mu_desc, strcmp("username", placeholders->name) == 0);
		mu_assert(mu_desc, strcmp("max", placeholders->value) == 0);
		mu_assert(mu_desc, placeholders->next != NULL);
		if(placeholders->next != NULL) {
			placeholder_t* next = placeholders->next;
			mu_assert(mu_desc, strcmp("friend", next->name) == 0);
			mu_assert(mu_desc, strcmp("nils", next->value) == 0);
		}
		free_placeholder_list(placeholders);
	}
	return 0;
}

static char* all_tests() {
	mu_run_test(test_create_basic_path);
	mu_run_test(test_create_placeholder_path);
	mu_run_test(test_create_placeholder_path1);
	mu_run_test(test_find_not_found);
	mu_run_test(test_find_basic_url);
	mu_run_test(test_find_basic_terminator);
	mu_run_test(test_find_trailing_separator);
	mu_run_test(test_find_trailing_separator_and_terminator);
	mu_run_test(test_find_placeholder);
	mu_run_test(test_find_placeholder_with_trailing_separator);
	mu_run_test(test_find_placeholder_with_trailing_separator_and_terminator);
	mu_run_test(test_find_multiple_placeholder);
	return 0;
}	

int main(int argc, char** argv) {
	char* result = all_tests();
	if(result != 0) {
		fprintf(stderr, "Failed #%d: %s\n", tests_run, result);
	} else {
		fprintf(stdout, "Passed all (%d) tests.\n", tests_run);
	}
	
	if(path != NULL) free_path_tree(path);
	return 0;
}
