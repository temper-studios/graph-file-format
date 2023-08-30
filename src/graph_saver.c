#pragma once
static void node_add_name(graph_saver *saver, graph_saver_node *node, graph_pool *pool, const char *name, graph_u64 nameLength) {

	assert(saver);
	assert(node);

	if (name == NULL) {
		node->name.ptr = NULL;
		node->name.length = 0;
		return;
	}
	node->name.ptr = graph_pool_push_string(&saver->errorCtx, pool, &node->name.length, name, nameLength);
}

void graph_saver_start(graph_saver *saver, graph_error_context_callback callback) {
	assert(saver);
	assert(callback);

	graph_error_context_init(&saver->errorCtx, callback);

	graph_pool_init(&saver->errorCtx, &saver->poolNode);
	if (graph_saver_check_error(saver)) { return; }
	graph_pool_init(&saver->errorCtx, &saver->poolString);
	if (graph_saver_check_error(saver)) { return; }
	saver->root.name.ptr = NULL;
	saver->root.name.length = 0;
	saver->root.parent = NULL;
	saver->root.children = NULL;
	saver->root.next = NULL;
	saver->root.type = GRAPH_SAVER_TYPE_COMPOSITE;
}

void graph_saver_reset(graph_saver *saver) {
	assert(saver);

	graph_pool_clear(&saver->poolNode);
	graph_pool_clear(&saver->poolString);
	saver->root.name.ptr = NULL;
	saver->root.name.length = 0;
	saver->root.parent = NULL;
	saver->root.children = NULL;
	saver->root.next = NULL;
	saver->root.type = GRAPH_SAVER_TYPE_COMPOSITE;
}

void graph_saver_finish(graph_saver *saver) {
	assert(saver);

	graph_pool_deinit(&saver->poolNode);
	graph_pool_deinit(&saver->poolString);
	saver->root.name.ptr = NULL;
	saver->root.name.length = 0;
	saver->root.parent = NULL;
	saver->root.children = NULL;
	saver->root.next = NULL;
	saver->root.type = GRAPH_SAVER_TYPE_COMPOSITE;
}

bool graph_saver_check_error(graph_saver *saver) {
	assert(saver);
	return graph_error_context_check(&saver->errorCtx);
}

graph_saver_node *graph_save_node_span(graph_saver *saver, graph_saver_node *parent, const char *name, graph_u64 nameLength) {

	assert(saver);

	graph_saver_node *node = (graph_saver_node *)graph_pool_push(&saver->errorCtx, &saver->poolNode, sizeof(graph_saver_node));
	if (graph_saver_check_error(saver)) { return NULL; }

	node_add_name(saver, node, &saver->poolString, name, nameLength);
	if (graph_saver_check_error(saver)) { return NULL; }

	if (parent == NULL) {
		parent = &saver->root;
	}
	node->parent = parent;
	node->children = NULL;
	node->next = NULL;
	node->type = GRAPH_SAVER_TYPE_COMPOSITE;
	// TODO: turn into double linked list{
	if (parent->children == NULL) {
		parent->children = node;
	}
	else {
		graph_saver_node *child = parent->children;
		while (child->next != NULL) child = child->next;
		child->next = node;
	}

	return node;
}

graph_saver_node *graph_save_node(graph_saver *saver, graph_saver_node *parent, const char *name) {

	assert(saver);

	return graph_save_node_span(saver, parent, name, graph_utility_string_length(name));
}

graph_saver_node *graph_save_u64(graph_saver *saver, graph_saver_node *parent, graph_u64 value) {
	graph_saver_node *node = graph_save_node_span(saver, parent, NULL, 0);
	if (graph_saver_check_error(saver)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_UNSIGNED_INTEGER;
	node->value.unsignedInteger = value;
	return node;
}

graph_saver_node *graph_save_s64(graph_saver *saver, graph_saver_node *parent, graph_s64 value) {
	graph_saver_node *node = graph_save_node_span(saver, parent, NULL, 0);
	if (graph_saver_check_error(saver)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_SIGNED_INTEGER;
	node->value.signedInteger = value;
	return node;
}

graph_saver_node *graph_save_double(graph_saver *saver, graph_saver_node *parent, double value) {
	graph_saver_node *node = graph_save_node_span(saver, parent, NULL, 0);
	if (graph_saver_check_error(saver)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_FLOAT;
	node->value.floatingPoint = value;
	return node;
}

graph_saver_node *graph_save_string_span(graph_saver *saver, graph_saver_node *parent, const char *value, graph_u64 length) {
	graph_saver_node *node = graph_save_node_span(saver, parent, NULL, 0);
	if (graph_saver_check_error(saver)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_STRING;
	node->value.str.ptr = graph_pool_push_string(&saver->errorCtx, &saver->poolString, &node->value.str.length, value, length);
	if (graph_saver_check_error(saver)) { return NULL; }
	return node;
}

graph_saver_node *graph_save_string_null_terminated(graph_saver *saver, graph_saver_node *parent, const char *value) {
	graph_saver_node *node = graph_save_node_span(saver, parent, NULL, 0);
	if (graph_saver_check_error(saver)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_STRING;
	node->value.str.ptr = graph_pool_push_string(&saver->errorCtx, &saver->poolString, &node->value.str.length, value, graph_utility_string_length(value));
	if (graph_saver_check_error(saver)) { return NULL; }
	return node;
}

static void write_to_file(graph_saver *saver, FILE *file, graph_saver_node *parent, const int depth) {

	assert(saver);
	assert(file);

	if (parent != &saver->root) {
		int i = depth;
		while (i > 0) {
			if (fprintf(file, "\t") < 0) {
				graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\t");
				return;
			}
			i--;
		}

		if (fprintf(file, "%s { ", parent->name.ptr) < 0) {
			graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%s { ", parent->name.ptr);
			return;
		}
	}

	graph_saver_node *childLast = NULL;
	graph_saver_node *child = parent->children;

	while (child != NULL) {

		if (child->type == GRAPH_SAVER_TYPE_COMPOSITE) {
			if (parent != &saver->root && fprintf(file, "\n") < 0) {
				graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\n");
				return;
			}

			if (child->children) {
				write_to_file(saver, file, child, depth + 1);
				if (graph_error_context_check(&saver->errorCtx)) {
					return;
				}
			}
			else {
				int i = depth + 1;
				while (i > 0) {
					if (fprintf(file, "\t") < 0) {
						graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\t");
						return;
					}
					i--;
				}
				if (fprintf(file, "%s", child->name.ptr) < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%s", child->name.ptr);
					return;
				}
			}
		}
		else if (child->type == GRAPH_SAVER_TYPE_FLOAT) {
			if (child->next == NULL) {
				if (fprintf(file, "%f", child->value.floatingPoint) < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%f", child->value.floatingPoint);
					return;
				}
			}
			else {
				if (fprintf(file, "%f, ", child->value.floatingPoint) < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%f", child->value.floatingPoint);
					return;
				}
			}
		}
		else if (child->type == GRAPH_SAVER_TYPE_UNSIGNED_INTEGER) {
			if (child->next == NULL) {
				if (fprintf(file, "%lu", child->value.unsignedInteger) < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%lu", child->value.unsignedInteger);
					return;
				}
			}
			else {
				if (fprintf(file, "%lu, ", child->value.unsignedInteger) < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%lu", child->value.unsignedInteger);
					return;
				}
			}
		}
		else if (child->type == GRAPH_SAVER_TYPE_SIGNED_INTEGER) {
			if (child->next == NULL) {
				if (fprintf(file, "%ld", child->value.signedInteger) < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%ld", child->value.signedInteger);
					return;
				}
			}
			else {
				if (fprintf(file, "%ld, ", child->value.signedInteger) < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%ld", child->value.signedInteger);
					return;
				}
			}
		}
		else if (child->type == GRAPH_SAVER_TYPE_STRING) {
			if (fprintf(file, "\"") < 0) {
				graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\\"", child->value.signedInteger);
				return;
			}
			for (size_t i = 0; i < child->value.str.length; i++) {
				if (fprintf(file, "%c", child->value.str.ptr[i]) < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%c", child->value.str.ptr[i]);
					return;
				}
			}
			if (child->next == NULL) {
				if (fprintf(file, "\"") < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\");
					return;
				}
			}
			else {
				if (fprintf(file, "\", ") < 0) {
					graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\");
					return;
				}
			}
		}

		childLast = child;
		child = child->next;
	}

	if (childLast && childLast->type != GRAPH_SAVER_TYPE_COMPOSITE) {
		if (fprintf(file, " }") < 0) {
			graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, " }");
			return;
		}
	}
	else {
		if (fprintf(file, "\n") < 0) {
			graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\n");
			return;
		}
		int i = depth;
		while (i > 0) {
			if (fprintf(file, "\t") < 0) {
				graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\t");
				return;
			}
			i--;
		}

		if (parent != &saver->root && fprintf(file, "}") < 0) {
			graph_error_context_push_error(&saver->errorCtx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "}");
			return;
		}
	}
}

void graph_save_to_file(graph_saver *saver, const char *filename) {
	assert(saver);
	assert(filename);

	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		graph_error_context_push_error(&saver->errorCtx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "missing file");
		return;
	}
	write_to_file(saver, file, &saver->root, -1);
	if (graph_saver_check_error(saver)) {
		graph_error_context_push_error(&saver->errorCtx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "failed to write to file");
	}
	fclose(file);
}
