#pragma once

typedef enum graph_saver_type {
	GRAPH_SAVER_TYPE_SIGNED_INTEGER,
	GRAPH_SAVER_TYPE_UNSIGNED_INTEGER,
	GRAPH_SAVER_TYPE_FLOAT,
	GRAPH_SAVER_TYPE_STRING,
	GRAPH_SAVER_TYPE_COMPOSITE
} graph_saver_type;

typedef struct graph_saver_node {
	union {
		graph_s64 signedInteger;
		graph_u64 unsignedInteger;
		double floatingPoint;
		struct {
			const char *ptr;
			graph_u64 length;
		} str;
	} value;
	struct {
		const char *ptr;
		graph_u64 length;
	} name;
	graph_saver_type type;
	struct graph_saver_node *parent;
	struct graph_saver_node *children;
	struct graph_saver_node *next;
} graph_saver_node;

void graph_saver_node_add_name(graph_error_context *ctx, graph_saver_node *node, graph_pool *pool, const char *name, graph_u64 nameLength);

typedef struct graph_saver {
	graph_pool poolNode;
	graph_pool poolString;
	graph_saver_node root;
} graph_saver;

void graph_saver_init(graph_error_context *ctx, graph_saver *saver);
void graph_saver_clear(graph_saver *saver);
void graph_saver_deinit(graph_saver *saver);

graph_saver_node *graph_saver_add_node_with_len(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, const char *name, graph_u64 nameLength);
graph_saver_node *graph_saver_add_node(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, const char *name);
graph_saver_node *graph_saver_add_u64(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, graph_u64 value);
graph_saver_node *graph_saver_add_s64(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, graph_s64 value);
graph_saver_node *graph_saver_add_double(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, double value);
graph_saver_node *graph_saver_add_string(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, const char *value, graph_u64 length);
graph_saver_node *graph_saver_add_string_null_terminated(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, const char *value);
void graph_saver_write_to_file(graph_error_context *ctx, graph_saver *saver, FILE *file, graph_saver_node *parent, const int depth);
void graph_saver_save(graph_error_context *ctx, graph_saver *saver, const char *filename);