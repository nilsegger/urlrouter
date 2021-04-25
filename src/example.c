/*
* Copyright 2021 Nils Egger 
* 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <stdio.h>
#include <stdlib.h>

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

