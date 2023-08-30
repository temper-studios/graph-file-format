#pragma once
typedef enum graph_error_code {
	GRAPH_ERROR_UNKNOWN,
	GRAPH_CONVERSION_ERROR,
	GRAPH_NO_ERROR,
	GRAPH_LOAD_ERROR,
	GRAPH_UNEXPECTED_NODE,
	GRAPH_LOGIC_ERROR,
	GRAPH_FAILED_TO_PRINT
} graph_error_code;

const char *graph_error_code_to_string(graph_error_code error);

struct graph_error_context;
struct graph_token;
typedef void (*graph_error_context_callback)(struct graph_error_context *, void *data, struct graph_token *token, const char *format, ...);
typedef struct graph_error_context {
	graph_error_context_callback callback;
	graph_error_code errorStack[64];
	graph_u64 errorStackPtr;
} graph_error_context;

void graph_error_context_default_exit_callback(graph_error_context *ctx, void *data, struct graph_token *token, const char *format, ...);
void graph_error_context_default_callback(graph_error_context *ctx, void *data, struct graph_token *token, const char *format, ...);
/* Initialises the error context. This must be called before initialising the error context. It does not need to be
 * matched with a call to a deinit function. The callback function defines the kind of behaviour that occurs when an error is encountered.
 * this can range from printing to std out, to exiting the application. */
void graph_error_context_init(graph_error_context *ctx, graph_error_context_callback callback);
void graph_error_context_clear(graph_error_context *ctx);
bool graph_error_context_check(graph_error_context *ctx);
void graph_error_context_push_error(graph_error_context *ctx, graph_error_code error, void *data, struct graph_token *token, const char *format, ...);
