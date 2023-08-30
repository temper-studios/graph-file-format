#pragma once
typedef enum graph_token_type {
	GRAPH_TOKEN_TYPE_ROOT,
	GRAPH_TOKEN_TYPE_NAME,
	GRAPH_TOKEN_TYPE_CURLY_CLOSE,
	GRAPH_TOKEN_TYPE_COMMENT, 
	GRAPH_TOKEN_TYPE_COMPOSITE_TYPE, 
	GRAPH_TOKEN_TYPE_STRING,
	GRAPH_TOKEN_TYPE_FLOAT,
	GRAPH_TOKEN_TYPE_INTEGER,
	GRAPH_TOKEN_TYPE_END_FILE,
	GRAPH_TOKEN_TYPE_VALUE_ASSIGN
} graph_token_type;

typedef struct graph_token {
	const char *start;
	graph_u64 length;
	graph_token_type type;
	graph_u64 lineno;
	struct graph_token *next;
} graph_token;

/* Prints the token to stdout */
void graph_token_print(graph_token *token);
/* Does a string comparison between the token's string and str. If they match then returns true.
 * Is case sensitive */
bool graph_token_equal(graph_token *token, const char *str);

typedef struct graph_loader_node {
	graph_token *token;
	struct graph_loader_node *parent;
	struct graph_loader_node *next;
	struct graph_loader_node *children;
} graph_loader_node;


typedef struct graph_loader {
	graph_error_context errorCtx;
	graph_pool poolNode;
	graph_pool poolToken;
	graph_loader_node *root;
	graph_token *rootToken;
	graph_token *lastToken;
	graph_token *curToken;
	char *buffer;
} graph_loader;

/* functions used to load, tokenise and parse the file into nodes */

/* Initialises the loader and loads the contents of a file. If there is an error
 * the error callback will get called. These errors can be checked. A call to load must be
 * matched with a call to graph_unload when finished with the loader. */
void graph_load_file(graph_loader *loader, const char *filename, graph_error_context_callback callback);
/* Initialises the loader and loads the contents of a string. If there is an error
 * the error callback will get called. These errors can be checked. A call to load must be
 * matched with a call to graph_unload when finished with the loader.
 * The str is copied, so does not need to exist for the duration of the loader.
 * The strLen is the length of the string (not including a null terminator).
 * The str does not need to be null terminated.*/
void graph_load_str(graph_loader *loader, char *str, int strLen, graph_error_context_callback callback);
/* Unload unloads the contents of the loader after a load function is called. Checking the loader
 * after it is unloaded will be undefined. */
void graph_unload(graph_loader *loader);
/* Checks if an error has occured with the loader object */
bool graph_loader_check_error(graph_loader *loader);
/* Returns the root node of the loader. This can result in an error */
graph_loader_node *graph_loader_get_root(graph_loader *loader);

// Helper function that does a string comparison between the token associated with the node with the str
bool graph_loader_node_equal(graph_loader_node *node, const char *str);
void graph_loader_node_add_child(graph_loader_node *parent, graph_loader_node *child);
graph_loader_node *graph_loader_get_child(graph_loader *loader, graph_loader_node *node);
graph_loader_node *graph_loader_get_next(graph_loader *loader, graph_loader_node *node);

/* 
	functions used to extract data from the nodes 
	these are mostly helper functions. 
	ideally you would define these yourself for your own datatypes
*/
void graph_loader_node_to_float(graph_loader *loader, graph_loader_node *node, float *number);
void graph_loader_node_to_double(graph_loader *loader, graph_loader_node *node, double *number);
void graph_loader_node_to_u64(graph_loader *loader, graph_loader_node *node, graph_u64 *number);
void graph_loader_node_to_s64(graph_loader *loader, graph_loader_node *node, graph_s64 *number);
void graph_loader_node_to_string(graph_loader *loader, graph_loader_node *node, char *src, graph_u64 srcCapacityIncludesNullTerminator);
