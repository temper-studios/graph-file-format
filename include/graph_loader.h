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

void graph_token_print(graph_token *token);
bool graph_token_equal(graph_token *token, const char *str);

typedef struct graph_loader_node {
	graph_token *token;
	struct graph_loader_node *parent;
	struct graph_loader_node *next;
	struct graph_loader_node *children;
} graph_loader_node;

bool graph_loader_node_equal(graph_loader_node *node, const char *str);
void graph_loader_node_add_child(graph_loader_node *parent, graph_loader_node *child);
graph_loader_node *graph_loader_node_get_child(graph_error_context *ctx, graph_loader_node *node);
graph_loader_node *graph_loader_node_get_next(graph_error_context *ctx, graph_loader_node *node);

typedef struct graph_loader {
	graph_pool poolNode;
	graph_pool poolToken;
	graph_loader_node *root;
	graph_token *rootToken;
	graph_token *lastToken;
	graph_token *curToken;
	char *buffer;
} graph_loader;

/* functions used to load, tokenise and parse the file into nodes */
void graph_loader_init(graph_error_context *ctx, graph_loader *loader);
void graph_loader_clear(graph_loader *loader);
void graph_loader_add_token(graph_error_context *ctx, graph_loader *loader, const char *start, graph_token_type type, graph_u64 lineno);
graph_loader_node *graph_loader_add_node(graph_error_context *ctx, graph_loader *loader, graph_token *token);
void graph_loader_increment_last_token_length(graph_loader *loader);
graph_token *graph_loader_consume_token(graph_loader *loader);
graph_token *graph_loader_peek_token(graph_loader *loader);
void graph_loader_parse(graph_error_context *ctx, graph_loader *loader, graph_loader_node *parentNode);
void graph_loader_tokenise(graph_error_context *ctx, graph_loader *loader);
void graph_loader_load(graph_error_context *ctx, graph_loader *loader, const char *filename);

/* 
	functions used to extract data from the nodes 
	these are mostly helper functions. 
	ideally you would define these yourself for your own datatypes
*/
void graph_loader_node_to_float(graph_error_context *ctx, graph_loader_node *node, float *number);
void graph_loader_node_to_double(graph_error_context *ctx, graph_loader_node *node, double *number);
void graph_loader_node_to_u64(graph_error_context *ctx, graph_loader_node *node, graph_u64 *number);
void graph_loader_node_to_s64(graph_error_context *ctx, graph_loader_node *node, graph_s64 *number);
void graph_loader_node_to_string(graph_error_context *ctx, graph_loader_node *node, char *src, graph_u64 srcCapacityIncludesNullTerminator);