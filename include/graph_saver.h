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

typedef struct graph_saver {
	graph_error_context errorCtx;
	graph_pool poolNode;
	graph_pool poolString;
	graph_saver_node root;
} graph_saver;

/* Must be called before doing anything with the saver object. When finished
 * call graph_saver_finish */
void graph_saver_start(graph_saver *saver, graph_error_context_callback callback);
/* Resets the saver object so that it contains no data. */
void graph_saver_reset(graph_saver *saver);
/* Called when finished with the saver object. Deallocates data */
void graph_saver_finish(graph_saver *saver);
/* Returns true if the saver object threw an error */
bool graph_saver_check_error(graph_saver *saver);

/* Adds a node to the saver object. The string span is used as the name. It is copied. Returns
 * this node. If there is an error, this will return NULL. */
graph_saver_node *graph_save_node_span(graph_saver *saver, graph_saver_node *parent, const char *name, graph_u64 nameLength);
/* Adds a node to the saver object. The string must be null terminated. This string is the
 * name of the node. Returns this node. If there is an error this will return NULL. */
graph_saver_node *graph_save_node(graph_saver *saver, graph_saver_node *parent, const char *name);
/* Adds an unsigned 64 bit integer to the saver object. Returns the node that stores this value  * If there is an error it will return NULL. */
graph_saver_node *graph_save_u64(graph_saver *saver, graph_saver_node *parent, graph_u64 value);
/* Adds an signed 64 bit integer to the saver object. Returns the node that stores this value  * If there is an error it will return NULL. */
graph_saver_node *graph_save_s64(graph_saver *saver, graph_saver_node *parent, graph_s64 value);
/* Adds a double to the saver object. Returns the node that stores this value  * If there is an error it will return NULL. */
graph_saver_node *graph_save_double(graph_saver *saver, graph_saver_node *parent, double value);
/* Adds a string span to the saver object. Returns the node that stores this value  * If there is an error it will return NULL. */
graph_saver_node *graph_save_string_span(graph_saver *saver, graph_saver_node *parent, const char *value, graph_u64 length);
/* Adds a null terminated string to the saver object. Returns the node that stores this value  * If there is an error it will return NULL. */
graph_saver_node *graph_save_string_null_terminated(graph_saver *saver, graph_saver_node *parent, const char *value);
/* Saves the contents of the saver object to a file. Overwrites the file if it exists already */
void graph_save_to_file(graph_saver *saver, const char *filename);
