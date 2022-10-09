const char *graph_error_code_to_string(graph_error_code error) {
	switch (error) {
	case GRAPH_ERROR_UNKNOWN: return "GRAPH_ERROR_UNKNOWN";
	case GRAPH_CONVERSION_ERROR: return "GRAPH_CONVERSION_ERROR";
	case GRAPH_NO_ERROR: return "GRAPH_NO_ERROR";
	case GRAPH_LOAD_ERROR: return "GRAPH_LOAD_ERROR";
	case GRAPH_UNEXPECTED_NODE: return "GRAPH_UNEXPECTED_NODE";
	case GRAPH_LOGIC_ERROR: return "GRAPH_LOGIC_ERROR";
	case GRAPH_FAILED_TO_PRINT: return "GRAPH_FAILED_TO_PRINT";
	default: return "GRAPH_ERROR_UNKNOWN";
	}
}

void graph_error_context_default_exit_callback(graph_error_context *ctx, void *data, graph_token *token, const char *format, ...) {
	graph_error_context_default_callback(ctx, data, token, format);
	exit(1);
}

void graph_error_context_default_callback(graph_error_context *ctx, void *data, graph_token *token, const char *format, ...) {

	if (ctx->errorStackPtr > 0) {
		printf("%s ", graph_error_code_to_string(ctx->errorStack[ctx->errorStackPtr - 1]));
	}

	if (token) {
		printf("[%llu] ", token->lineno);
		printf("%s: ", format);
		putc('\"', stdout);
		graph_token_print(token);
		putc('\"', stdout);
		putc('\n', stdout);
	}
	else {
		puts(format);
	}
}

void graph_error_context_init(graph_error_context *ctx, graph_error_context_callback callback) {
	ctx->callback = callback;
	ctx->errorStackPtr = 0;
}

void graph_error_context_clear(graph_error_context *ctx) {
	ctx->errorStackPtr = 0;
}

bool graph_error_context_check(graph_error_context *ctx) {
	return ctx->errorStackPtr;
}

void graph_error_context_push_error(graph_error_context *ctx, graph_error_code error, void *data, graph_token *token, const char *format, ...) {
	ctx->errorStack[ctx->errorStackPtr] = error;
	ctx->errorStackPtr = (ctx->errorStackPtr + 1) % 64;
	va_list ap;
	va_start(ap, format);
	ctx->callback(ctx, data, token, format, ap);
	va_end(ap);
}