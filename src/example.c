#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <urlrouter.h>

int main(int argc, char** argv) {
	path_t* path = NULL;
	uint8_t res = create_path(&path, "/users/nils", 1, '{', '}');
	res |= create_path(&path, "/users/{username}", 2, '{', '}');
	res |= create_path(&path, "/users/{username}/friends", 3, '{', '}');
	res |= create_path(&path, "/users/{username}/friends/{friend}", 4, '{', '}');
	if(res != MATCHER_SUCCESS) {
		fprintf(stderr, "Failed to create paths.\n");	
		return EXIT_FAILURE;
	}

	uint64_t matched;
	placeholder_t* placeholders;
	res |= find_url(&matched, &placeholders, path, "/users/nils");
	if(res == MATCHER_SUCCESS && matched == 1) {
		printf("Matched to static user nils.\n");	
	}

	res |= find_url(&matched, &placeholders, path, "/users/random");
	if(res == MATCHER_SUCCESS && matched == 2) {
		printf("Matched to variable user \"%s\".\n", placeholders->value);	
		free_placeholder_list(placeholders);
	}

	placeholders = NULL;
	res |= find_url(&matched, &placeholders, path, "/users/random/friends/nils");
	if(res == MATCHER_SUCCESS && matched == 4) {
		printf("Matched to variable user \"%s\" with friend \"%s\".\n", placeholders->value, placeholders->next->value);	
		free_placeholder_list(placeholders);
	}

	free_path_tree(path);
	return EXIT_SUCCESS;
}

