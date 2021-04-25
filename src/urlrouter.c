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

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "urlrouter.h"

/* A path represents one Character or a wildcard in a path, a path can look something like this: "/users/nils" or "/users/{username}"
 * The first path would represent the first 'u' from "users/nils" and keep the path from 's' as a child.
 */
typedef struct path{
	char character;
	const char* name;
	struct path* placeholder;
	struct path* sibling;
	struct path* children;
	uint64_t value;
} path_t;

path_t* create_path_node(path_t** ptr, char character, const char* name, const uint64_t value) {
	*ptr = (path_t*)malloc(sizeof(path_t)); 
	(*ptr)->character = character;
	(*ptr)->placeholder= NULL;
	(*ptr)->children = NULL;
	(*ptr)->sibling = NULL;
	(*ptr)->name= name;
	(*ptr)->value = value;
	return *ptr;
}

/**
 * starts at placeholder_start and tries to find placeholder_end, if found, copies it into name
 * @param start Starting pointer of placeholder_start
 * @param placeholder_end Character to look out for
 * @param name Will contain string of all characters between placeholder_start and placeholder_name
 * @param length Will contain length of name
 * @return Either MATCHER_SUCCESS or MATCHER_NON_ENDING_PLACEHOLDER if placeholder_end was never found
 */
uint8_t read_placeholder_name(const char* start, const char placeholder_end, char** name, uint64_t* length) {
	bool end_found = false;
	for(const char* iter = start + 1; *iter != 0; iter++) {
		if(*iter == placeholder_end) {
			end_found = true;			
			*name = malloc(*length + 1); // length works because counted placeholder_end will be replaced by 0
			memcpy(*name, start + 1, *length);
			(*name)[*length] = 0;
			break;
		} else {
			*length += 1;
		}
	}
	return MATCHER_NON_ENDING_PLACEHOLDER * (!end_found)
		+ MATCHER_SUCCESS * (end_found);
}

/**
 *
 * create_path takes a string and splits it ups into its separate paths
 *
 * @param root path_t** starting node of path tree
 * @param path const char* which will be split up into its separate paths.
 * @param value Integer which will be returned if matcher matches to path
 * @param placeholder_start Char which determined beginning of placeholder.
 * 	Example would be /users/{username}, if '{' is start,
 * 	it tells the code to start viewing the following code until placeholder_end as the placeholder name.
 * @param placholder_end Chat which determines end of placeholder name.
 * @return Returns integer representing error code.
 */
uint8_t create_path(path_t** root, const char* path, uint64_t value, const char placeholder_start, const char placeholder_end) {
	if(path == NULL || *path == 0) {
		return MATCHER_EMPTY_STRING;
	} else if(value == MATCHER_EMPTY) {
		return MATCHER_RESERVED_ZERO;
	}

	// create first path if root is empty, all other paths must start with same character, e.g. '/' for urls
	if(*root == NULL) {
		create_path_node(root, path[0], NULL, MATCHER_EMPTY);	
	} else if((*root)->character != path[0]) {
		return MATCHER_NON_MATCHING_FIRST_CHARACTER;
	}

	path_t* iter = *root;
	for(uint64_t i = 0; path[i] != 0; i++) {
		if(path[i] == placeholder_start) {
			char* name = NULL;
			uint64_t name_length = 0;
			if(read_placeholder_name(path + i, placeholder_end, &name, &name_length) == MATCHER_NON_ENDING_PLACEHOLDER) {
				return MATCHER_NON_ENDING_PLACEHOLDER;
			} 

			if(iter->placeholder != NULL) {
				if(strcmp(name, iter->placeholder->name) == 0) {
					iter = iter->placeholder;
					free(name);
				} else {
					free(name);
					return MATCHER_PLACEHOLDERS_SAME_LEVEL_SAME_NAME_REQUIRED;
				}
			} else {
				iter = create_path_node(&iter->placeholder, MATCHER_EMPTY, name, MATCHER_EMPTY);
			}

			i += name_length + 2; // + 2 because of placeholder_end
			if(path[i] == 0) break;
		}

		if(iter->children == NULL) {
			iter = create_path_node(&iter->children, path[i], NULL, MATCHER_EMPTY);
		} else {
			for(path_t* child = iter->children; child != NULL; child = child->sibling) {
				if(child->character == path[i]) {
					// No need for creating a new path, it already exists
					iter = child;	
					break;
				} else if(child->sibling == NULL) {
					iter = create_path_node(&child->sibling, path[i], NULL, MATCHER_EMPTY);
					break;
				}
			}
		}
	}

	iter->value = value;

	return MATCHER_SUCCESS;
}

uint8_t find(uint64_t* result, placeholder_t** placeholders, const path_t* paths, const char* str, char separator, const char terminator) {
	if(*str == 0) {
		return MATCHER_EMPTY_STRING;
	}

	path_t* node = (path_t*)paths;
	placeholder_t* placeholder = NULL;
	uint64_t last_separator_index = 0;
	path_t* last_placeholder_node = NULL;
	for(uint64_t i = 0; str[i] != 0; i++) {
		if(str[i] == terminator) {
			break;
		}
		// if a separator is found, the last placeholder should no longer be used, hence set to NULL
		// e.g /users/{username}/xyz, if the path matches /users/nils/xbz, the placeholder {username} should no longer be valid because of the last '/' separator for urls
		if(str[i] == separator) {
			if(str[i + 1] == 0 || str[i + 1] == terminator) {
				// e.g. /users/nils/ should still match to /users/nils, hence ignore last separator if next char is string terminator
				break;
			}
			last_separator_index = i; 
			last_placeholder_node = NULL;
		} else if(node->placeholder != NULL) {
			last_placeholder_node = node->placeholder;
		}

		// check if any of the children match the character
		path_t* matching_child = NULL;
		for(path_t* iter = node->children; iter != NULL; iter = iter->sibling) {
			if(iter->character == str[i]) {
				matching_child = iter;
				break;
			}
		}

		if(matching_child != NULL) {
			// continue with children of matching child
			node = matching_child;
		} else if(matching_child == NULL && last_placeholder_node != NULL) {
			// try to continue with filling placeholder
			uint64_t next_separator_index = i + 1;
			for(; str[next_separator_index] != 0 && str[next_separator_index] != separator; next_separator_index++);
			uint64_t value_start = last_separator_index + 1;
			uint64_t value_end = next_separator_index;
			uint64_t value_length = value_end - value_start; // this length is non terminated!
			if(placeholder == NULL) {
				placeholder = malloc(sizeof(placeholder_t));
				*placeholders = placeholder;
			} else {
				placeholder->next = malloc(sizeof(placeholder_t));
				placeholder = placeholder->next;
			}
			placeholder->name = malloc(strlen(last_placeholder_node->name) + 1);
			strcpy(placeholder->name, last_placeholder_node->name);
			placeholder->value = malloc(value_length + 1);
			memcpy(placeholder->value, &str[value_start], value_length);
			placeholder->value[value_length] = 0;
			placeholder->next = NULL;
			node = last_placeholder_node;
			last_placeholder_node = NULL;
			i = next_separator_index - 1; // sub one, because loop will increase it by one again, otherwise, separator would get skipped and lead to non matching routes
		} else if(matching_child == NULL && last_placeholder_node == NULL) {
			// There is no match and no placeholder to fill
			node = NULL;
			break;
		}
	}
	
	if(node != NULL) *result = node->value; // If node->value is not a end / value, MATCHER_EMPTY = MATCHER_NOT_FOUND = 0 is returned.
	else *result = MATCHER_NOT_FOUND;
	return MATCHER_SUCCESS;
}

uint8_t find_url(uint64_t* result, placeholder_t** placeholders, const path_t* paths, const char* url) {
	return find(result, placeholders, paths, url, '/', '?');
}

#define ROOT 0
#define SIBLING 1
#define CHILD 2
#define PLACEHOLDER 3

void free_path_tree(path_t* root) {
	path_t* iter = root;
	path_t* last = root;
	uint8_t type = ROOT;
	while(iter != NULL) {

		if(iter->children != NULL) {
			last = iter;
			type = CHILD; 
			iter = iter->children;
			continue; // Continue until deepest child has been reached
		}

		if(iter->placeholder != NULL) {
			last = iter;
			type = PLACEHOLDER; 
			iter = iter->placeholder;
			continue; // Reach deepest placeholder
		}

		if(iter->sibling != NULL) {
			last = iter;
			type = SIBLING; 
			iter = iter->sibling;
			continue; // Continue until end of linked list has been reached
		}

		if(type == SIBLING) last->sibling = NULL;
		else if(type == PLACEHOLDER) {
			last->placeholder = NULL;
			free((char*) iter->name);
		}
		else if(type == CHILD) last->children =NULL;
		free(iter);
		if(iter == root) {
			break;
		}
		iter = root;
	}
}

void free_placeholder_list(placeholder_t* placeholder) {
	placeholder_t* iter = placeholder;
	while(iter != NULL) {
		placeholder_t* temp = iter;
		iter = iter->next;
		free(temp->name);
		free(temp->value);
		free(temp);
	}
}
