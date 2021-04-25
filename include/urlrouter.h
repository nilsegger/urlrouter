#include <stdint.h>

#define MATCHER_EMPTY 0
#define MATCHER_SUCCESS 0
#define MATCHER_NOT_FOUND 0
#define MATCHER_EMPTY_STRING 1
#define MATCHER_NON_MATCHING_FIRST_CHARACTER 2
#define MATCHER_NON_ENDING_PLACEHOLDER 3
#define MATCHER_PLACEHOLDERS_SAME_LEVEL_SAME_NAME_REQUIRED 4
#define MATCHER_RESERVED_ZERO 5

typedef struct path path_t; 

typedef struct placeholder {
	char* name;
	char* value;
	struct placeholder* next;
} placeholder_t;

/**
 *
 * allocates new memory for path struct.
 *
 * @param ptr Reference to pointer which will hold new path
 * @param character If is_placeholder is false, path will represent character
 * @param name is for placeholder paths
 * @param value if path is last in path, value will be returned if it is a match
 * @return pointer to new path object
 */
uint8_t create_path(path_t** root, const char* path, uint64_t value, char placeholder_start, char placeholder_end);

/**
 *
 * tries to match str to the path 
 *
 * @param result Pointer to write result to, will be MATCHER_NOT_FOUND if str doesnt match to any
 * @param placeholders Linked list whilch will be created if any placeholders are used.
 * @param paths Root node of paths, str will be matched to these
 * @param str Complete path to be matched
 * @param separator Text which is between two separators can be viewed as placeholder fillers
 * @param terminator if this character is seen in str, the function stops matching str to paths, e.g. '?' for urls 
 * @return Error code
 */
uint8_t find(uint64_t* result, placeholder_t** placeholders, const path_t* paths, const char* str, char separator, const char terminator);

/**
 * Same as find, but fills separator with '/' and terminator with '?' 
 */
uint8_t find_url(uint64_t* result, placeholder_t** placeholders, const path_t* paths, const char* url);

/**
 * Frees linked list of path_t
 *
 * @param root Root node of linked list
 */
void free_path_tree(path_t* root);

/**
 * Frees linked list of placeholder_t
 *
 * @param placeholder Root node of linked list to free
 */
void free_placeholder_list(placeholder_t* placeholder);

