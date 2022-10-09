
bool graph_token_equal(graph_token *token, const char *str) {
	return graph_utility_strings_equal(token->start, token->length, str, graph_utility_string_length(str));
}

bool graph_loader_node_equal(graph_loader_node *node, const char *str) {
	return graph_token_equal(node->token, str);
}

graph_loader_node *graph_loader_node_get_child(graph_error_context *ctx, graph_loader_node *node) {
	if (!node) {
		graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "missing node");
	}
	else if (!node->children) {
		graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "missing node");
	}
	else {
		return node->children;
	}
	return NULL;
}

graph_loader_node *graph_loader_node_get_next(graph_error_context *ctx, graph_loader_node *node) {
	if (!node) {
		graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "missing node");
	}
	else if (!node->next) {
		graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, node->token, "missing node");
	}
	else {
		return node->next;
	}
	return NULL;
}

void graph_token_print(graph_token *token) {
	assert(token);
	for (graph_u64 i = 0; i < token->length; i++) {
		putc(token->start[i], stdout);
	}
//	putc('\n', stdout);
}

void graph_loader_init(graph_error_context *ctx, graph_loader *loader) {
	graph_pool_init(ctx, &loader->poolNode);
	graph_pool_init(ctx, &loader->poolToken);
	loader->buffer = NULL;
	loader->root = NULL;
	loader->rootToken = NULL;
	loader->lastToken = NULL;
	loader->curToken = NULL;
}

void graph_loader_deinit(graph_loader *loader) {
	graph_loader_clear(loader);
	graph_pool_deinit(&loader->poolNode);
	graph_pool_deinit(&loader->poolToken);
}

void graph_loader_clear(graph_loader *loader) {
	free(loader->buffer);
	graph_pool_clear(&loader->poolNode);
	graph_pool_clear(&loader->poolToken);
	loader->buffer = NULL;
	loader->root = NULL;
	loader->rootToken = NULL;
	loader->lastToken = NULL;
	loader->curToken = NULL;
}

void graph_loader_add_token(graph_error_context *ctx, graph_loader *loader, const char *start, graph_token_type type, graph_u64 lineno) {
	graph_token *token = (graph_token *)graph_pool_push(ctx, &loader->poolToken, sizeof(graph_token));
	if (graph_error_context_check(ctx)) { return; }

	if (loader->lastToken) {
		loader->lastToken->next = token;
	}
	loader->lastToken = token;
	loader->lastToken->start = start;
	loader->lastToken->type = type;
	loader->lastToken->length = 1;
	loader->lastToken->lineno = lineno;
	loader->lastToken->next = NULL;
}

graph_loader_node *graph_loader_add_node(graph_error_context *ctx, graph_loader *loader, graph_token *token) {
	graph_loader_node *node = (graph_loader_node *)graph_pool_push(ctx, &loader->poolNode, sizeof(graph_loader_node));
	if (graph_error_context_check(ctx)) { return NULL; }
	node->token = token;
	node->parent = NULL;
	node->next = NULL;
	node->children = NULL;
	return node;
}

void graph_loader_node_add_child(graph_loader_node *parent, graph_loader_node *child) {
	if (parent == NULL) return;
	if (parent->children == NULL) {
		parent->children = child;
	}
	else {
		// we can make these double linked lists to really improve performance here.
		graph_loader_node *childNext = parent->children;
		while (childNext->next != NULL) { childNext = childNext->next; }
		childNext->next = child;
	}
	child->parent = parent;
}

void graph_loader_increment_last_token_length(graph_loader *loader) {
	assert(loader->lastToken);
	loader->lastToken->length++;
}

graph_token *graph_loader_consume_token(graph_loader *loader) {
	graph_token *token = loader->curToken;
	if (!token) {
		return token;
	}
	loader->curToken = loader->curToken->next;
	return token;
}

graph_token *graph_loader_peek_token(graph_loader *loader) {
	return loader->curToken;
}

void graph_loader_parse(graph_error_context *ctx, graph_loader *loader, graph_loader_node *parentNode) {
	graph_token *token;
	while (token = graph_loader_consume_token(loader)) {
		if (token->type == GRAPH_TOKEN_TYPE_NAME) {

			if (parentNode) {
				parentNode->token->type = GRAPH_TOKEN_TYPE_COMPOSITE_TYPE;
			}
			graph_token *peek = graph_loader_peek_token(loader);
			if (graph_error_context_check(ctx)) { return; }

			if (peek->type == GRAPH_TOKEN_TYPE_VALUE_ASSIGN) {

				graph_loader_node *node = graph_loader_add_node(ctx, loader, token);
				if (graph_error_context_check(ctx)) { return; }
				graph_loader_node_add_child(parentNode, node);
				graph_loader_parse(ctx, loader, node);
				if (graph_error_context_check(ctx)) { return; }
			}
			else {
				graph_loader_node *node = graph_loader_add_node(ctx, loader, token);
				if (graph_error_context_check(ctx)) { return; }
				graph_loader_node_add_child(parentNode, node);
			}
		}
		else if (token->type == GRAPH_TOKEN_TYPE_STRING || token->type == GRAPH_TOKEN_TYPE_FLOAT || token->type == GRAPH_TOKEN_TYPE_INTEGER) {
			graph_loader_node *node = graph_loader_add_node(ctx, loader, token);
			if (graph_error_context_check(ctx)) { return; }
			graph_loader_node_add_child(parentNode, node);
		}
		else if (token->type == GRAPH_TOKEN_TYPE_CURLY_CLOSE) {
			break;
		}
		else if (token->type == GRAPH_TOKEN_TYPE_END_FILE) {
			break;
		}
	}
}

void graph_loader_tokenise(graph_error_context *ctx, graph_loader *loader) {

	char *c = loader->buffer;
	graph_u64 lineno = 1;

	while (true) {

		if (*c == '\0') {
			graph_loader_add_token(ctx, loader, c, GRAPH_TOKEN_TYPE_END_FILE, lineno);
			if (graph_error_context_check(ctx)) { return; }
			break;
		}
		else if (*c == '{') {
			graph_loader_add_token(ctx, loader, c, GRAPH_TOKEN_TYPE_VALUE_ASSIGN, lineno);
			if (graph_error_context_check(ctx)) { return; }
			c++;
		}
		else if (*c == '}') {
			graph_loader_add_token(ctx, loader, c, GRAPH_TOKEN_TYPE_CURLY_CLOSE, lineno);
			if (graph_error_context_check(ctx)) { return; }
			c++;
		}
		else if (*c == '/') {
			c++;
			if (*c == '*') {
				int nestedCommentDepth = 1;
				c++;
				while (nestedCommentDepth > 0) {

					if (*c == '/') {
						c++;
						if (*c == '*') {
							nestedCommentDepth++;
						}
					}
					else if (*c == '*') {
						c++;
						if (*c == '/') {
							nestedCommentDepth--;
						}
					}
					else if (*c == '\n') {
						lineno++;
						c++;
					}
					else if (*c == '\r') {
						lineno++;
						c++;
						if (*c == '\n') {
							c++;
						}
					}
					else {
						c++;
					}	
				}
			}
		}
		else if (*c == ' ' || *c == ',' || *c == '\t') {
			c++;
		}
		else if (*c == '\n') {
			lineno++;
			c++;
		}
		else if (*c == '\r') {
			lineno++;
			c++;
			if (*c == '\n') {
				c++;
			}
		}
		else if (*c == '\"') {

			c++;
			graph_loader_add_token(ctx, loader, c, GRAPH_TOKEN_TYPE_STRING, lineno);
			if (graph_error_context_check(ctx)) { return; }

			if (*c != '\"') {
				bool lastCharPossibleEscapeChar = false;
				c++;
				while (*c != '\0') {

					if (*c == '\"' && !lastCharPossibleEscapeChar) {
						break;
					}
					else if (*c == '\\') {
						lastCharPossibleEscapeChar = true;
					}
					else {
						lastCharPossibleEscapeChar = false;
					}

					graph_loader_increment_last_token_length(loader);
					c++;
				}
			}

			if (*c != '\0') { c++; }
		}
		else if (isalpha(*c)) {

			graph_loader_add_token(ctx, loader, c, GRAPH_TOKEN_TYPE_NAME, lineno);
			if (graph_error_context_check(ctx)) { return; }
			c++;

			while (isalpha(*c) || isdigit(*c) || *c == '_') {
				graph_loader_increment_last_token_length(loader);
				c++;
			}
		}
		else if (isdigit(*c) || *c == '-' || *c == '+') {
			
			bool hasFloatingPoint = false;

			graph_loader_add_token(ctx, loader, c, GRAPH_TOKEN_TYPE_FLOAT, lineno);
			if (graph_error_context_check(ctx)) { return; }
			c++;

			while (isdigit(*c) || *c == '.') {
				if (*c == '.') {

					if (hasFloatingPoint == false) {
						hasFloatingPoint = true;
					}

					graph_loader_increment_last_token_length(loader);
					c++;
					while (isdigit(*c)) {
						graph_loader_increment_last_token_length(loader);
						c++;
					}
					break;
				}
				else {
					graph_loader_increment_last_token_length(loader);
					c++;
				}
			}

			if (!hasFloatingPoint) {
				assert(loader->lastToken);
				loader->lastToken->type = GRAPH_TOKEN_TYPE_INTEGER;
			}
		}
		else {
			graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "unrecognised character %c", *c);
			return;
		}
	}
}

void graph_loader_load(graph_error_context *ctx, graph_loader *loader, const char *filename) {
	
	const char *errorMessage = "error";
	graph_u64 fsize = 0;
	graph_u64 readBytes = 0;
	FILE *file = NULL;

	graph_loader_clear(loader);

	file = fopen(filename, "rb");
	if (file == NULL) {
		errorMessage = "failed to load file because it could not be opened";
		goto LABEL_error;
	}
	fseek(file, 0, SEEK_END);
	fsize = ftell(file);
	fseek(file, 0, SEEK_SET);

	loader->buffer = (char *)malloc(fsize + 1);
	if (loader->buffer == NULL) {
		errorMessage = "failed to allocate buffer";
		goto LABEL_error;
	}
	readBytes = fread(loader->buffer, fsize, sizeof(char), file);
	if (readBytes != 1) {
		errorMessage = "failed to read file";
		goto LABEL_error;
	}
	if (fclose(file) != 0) {
		errorMessage = "failed to close file";
		goto LABEL_error;
	}
	file = NULL;
	loader->buffer[fsize] = '\0';

	graph_loader_add_token(ctx, loader, "root", GRAPH_TOKEN_TYPE_ROOT, 0);
	if (graph_error_context_check(ctx)) {
		errorMessage = "failed to create root token";
		goto LABEL_error;
	}
	loader->rootToken = loader->lastToken;
	loader->curToken = loader->rootToken;

	graph_loader_tokenise(ctx, loader);
	if (graph_error_context_check(ctx)) {
		errorMessage = "failed to tokenise";
		goto LABEL_error;
	}

	loader->root = graph_loader_add_node(ctx, loader, loader->rootToken);
	if (graph_error_context_check(ctx)) {
		errorMessage = "failed to create root node";
		goto LABEL_error;
	}

	graph_loader_parse(ctx, loader, loader->root);
	if (graph_error_context_check(ctx)) {
		errorMessage = "failed to parse";
		goto LABEL_error;
	}

	return;

LABEL_error:
	graph_error_context_push_error(ctx, GRAPH_LOAD_ERROR, NULL, NULL, errorMessage);
	if (file) {
		fclose(file);
	}
	graph_loader_clear(loader);
	return;
}

void graph_loader_node_to_string(graph_error_context *ctx, graph_loader_node *node, char *src, graph_u64 srcCapacityIncludesNullTerminator) {
	if (!node) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, NULL, "node is null in loader node to string");
		return;
	}
	if (node->token->type != GRAPH_TOKEN_TYPE_STRING) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, node->token, "node token is not of type string");
		return;
	}
	if (node->token->length >= srcCapacityIncludesNullTerminator) {
		graph_error_context_push_error(ctx, GRAPH_LOGIC_ERROR, NULL, node->token, "node token value can not fit inside provided string");
		return;
	}
	memcpy(src, node->token->start, node->token->length);
	src[node->token->length] = '\0';
}

void graph_loader_node_to_float(graph_error_context *ctx, graph_loader_node *node, float *number) {
	if (!node) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, NULL, "node is null in loader node to float");
		return;
	}
	if (node->token->type != GRAPH_TOKEN_TYPE_FLOAT) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, node->token, "node token is not of type float");
		return;
	}
	if (!graph_utility_spanning_str_to_float(node->token->start, node->token->length, number)) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, node->token, "could not convert to float");
		return;
	}
}

void graph_loader_node_to_double(graph_error_context *ctx, graph_loader_node *node, double *number) {
	if (!node) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, NULL, "node is null in loader node to double");
		return;
	}
	if (node->token->type != GRAPH_TOKEN_TYPE_FLOAT) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, node->token, "node token is not of type float");
		return;
	}
	if (!graph_utility_spanning_str_to_double(node->token->start, node->token->length, number)) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, node->token, "could not convert to double");
		return;
	}
}

void graph_loader_node_to_u64(graph_error_context *ctx, graph_loader_node *node, graph_u64 *number) {
	if (!node) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, NULL, "node is null in loader node to u64");
		return;
	}
	if (node->token->type != GRAPH_TOKEN_TYPE_INTEGER) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, NULL, "node token is not of type integer");
		return;
	}
	if (!graph_utility_spanning_str_to_u64(node->token->start, node->token->length, number)) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, node->token, "could not convert to u64");
		return;
	}
}

void graph_loader_node_to_s64(graph_error_context *ctx, graph_loader_node *node, graph_s64 *number) {
	if (!node) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, NULL, "node is null in loader node to s64");
		return;
	}
	if (node->token->type != GRAPH_TOKEN_TYPE_INTEGER) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, node->token, "node token is not of type integer");
		return;
	}
	if (!graph_utility_spanning_str_to_s64(node->token->start, node->token->length, number)) {
		graph_error_context_push_error(ctx, GRAPH_CONVERSION_ERROR, NULL, node->token, "could not convert to s64");
		return;
	}
}