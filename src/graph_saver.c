#pragma once
void graph_saver_node_add_name(graph_error_context *ctx, graph_saver_node *node, graph_pool *pool, const char *name, graph_u64 nameLength) {
	if (name == NULL) {
		node->name.ptr = NULL;
		node->name.length = 0;
		return;
	}
	node->name.ptr = graph_pool_push_string(ctx, pool, &node->name.length, name, nameLength);
}

void graph_saver_init(graph_error_context *ctx, graph_saver *saver) {
	graph_pool_init(ctx, &saver->poolNode);
	if (graph_error_context_check(ctx)) { return; }
	graph_pool_init(ctx, &saver->poolString);
	if (graph_error_context_check(ctx)) { return; }
	saver->root.name.ptr = NULL;
	saver->root.name.length = 0;
	saver->root.parent = NULL;
	saver->root.children = NULL;
	saver->root.next = NULL;
	saver->root.type = GRAPH_SAVER_TYPE_COMPOSITE;
}

void graph_saver_clear(graph_saver *saver) {
	graph_pool_clear(&saver->poolNode);
	graph_pool_clear(&saver->poolString);
	saver->root.name.ptr = NULL;
	saver->root.name.length = 0;
	saver->root.parent = NULL;
	saver->root.children = NULL;
	saver->root.next = NULL;
	saver->root.type = GRAPH_SAVER_TYPE_COMPOSITE;
}

void graph_saver_deinit(graph_saver *saver) {
	graph_pool_deinit(&saver->poolNode);
	graph_pool_deinit(&saver->poolString);
	saver->root.name.ptr = NULL;
	saver->root.name.length = 0;
	saver->root.parent = NULL;
	saver->root.children = NULL;
	saver->root.next = NULL;
	saver->root.type = GRAPH_SAVER_TYPE_COMPOSITE;
}

graph_saver_node *graph_saver_add_node_with_len(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, const char *name, graph_u64 nameLength) {
	graph_saver_node *node = (graph_saver_node *)graph_pool_push(ctx, &saver->poolNode, sizeof(graph_saver_node));
	if (graph_error_context_check(ctx)) { return NULL; }

	graph_saver_node_add_name(ctx, node, &saver->poolString, name, nameLength);
	if (graph_error_context_check(ctx)) { return NULL; }

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

graph_saver_node *graph_saver_add_node(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, const char *name) {
	return graph_saver_add_node_with_len(ctx, saver, parent, name, graph_utility_string_length(name));
}

graph_saver_node *graph_saver_add_u64(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, graph_u64 value) {
	graph_saver_node *node = graph_saver_add_node_with_len(ctx, saver, parent, NULL, 0);
	if (graph_error_context_check(ctx)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_UNSIGNED_INTEGER;
	node->value.unsignedInteger = value;
	return node;
}

graph_saver_node *graph_saver_add_s64(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, graph_s64 value) {
	graph_saver_node *node = graph_saver_add_node_with_len(ctx, saver, parent, NULL, 0);
	if (graph_error_context_check(ctx)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_SIGNED_INTEGER;
	node->value.signedInteger = value;
	return node;
}

graph_saver_node *graph_saver_add_double(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, double value) {
	graph_saver_node *node = graph_saver_add_node_with_len(ctx, saver, parent, NULL, 0);
	if (graph_error_context_check(ctx)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_FLOAT;
	node->value.floatingPoint = value;
	return node;
}

graph_saver_node *graph_saver_add_string(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, const char *value, graph_u64 length) {
	graph_saver_node *node = graph_saver_add_node_with_len(ctx, saver, parent, NULL, 0);
	if (graph_error_context_check(ctx)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_STRING;
	node->value.str.ptr = graph_pool_push_string(ctx, &saver->poolString, &node->value.str.length, value, length);
	if (graph_error_context_check(ctx)) { return NULL; }
	return node;
}

graph_saver_node *graph_saver_add_string_null_terminated(graph_error_context *ctx, graph_saver *saver, graph_saver_node *parent, const char *value) {
	graph_saver_node *node = graph_saver_add_node_with_len(ctx, saver, parent, NULL, 0);
	if (graph_error_context_check(ctx)) { return NULL; }
	node->type = GRAPH_SAVER_TYPE_STRING;
	node->value.str.ptr = graph_pool_push_string(ctx, &saver->poolString, &node->value.str.length, value, graph_utility_string_length(value));
	if (graph_error_context_check(ctx)) { return NULL; }
	return node;
}

void graph_saver_write_to_file(graph_error_context *ctx, graph_saver *saver, FILE *file, graph_saver_node *parent, const int depth) {

	if (parent != &saver->root) {
		int i = depth;
		while (i > 0) {
			if (fprintf(file, "\t") < 0) {
				graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\t");
				return;
			}
			i--;
		}

		if (fprintf(file, "%s { ", parent->name.ptr) < 0) {
			graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%s { ", parent->name.ptr);
			return;
		}
	}

	graph_saver_node *childLast = NULL;
	graph_saver_node *child = parent->children;

	while (child != NULL) {

		if (child->type == GRAPH_SAVER_TYPE_COMPOSITE) {
			if (parent != &saver->root && fprintf(file, "\n") < 0) {
				graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\n");
				return;
			}

			if (child->children) {
				graph_saver_write_to_file(ctx, saver, file, child, depth + 1);
				if (graph_error_context_check(ctx)) {
					return;
				}
			}
			else {
				int i = depth + 1;
				while (i > 0) {
					if (fprintf(file, "\t") < 0) {
						graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\t");
						return;
					}
					i--;
				}
				if (fprintf(file, "%s", child->name.ptr) < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%s", child->name.ptr);
					return;
				}
			}
		}
		else if (child->type == GRAPH_SAVER_TYPE_FLOAT) {
			if (child->next == NULL) {
				if (fprintf(file, "%f", child->value.floatingPoint) < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%f", child->value.floatingPoint);
					return;
				}
			}
			else {
				if (fprintf(file, "%f, ", child->value.floatingPoint) < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%f", child->value.floatingPoint);
					return;
				}
			}
		}
		else if (child->type == GRAPH_SAVER_TYPE_UNSIGNED_INTEGER) {
			if (child->next == NULL) {
				if (fprintf(file, "%llu", child->value.unsignedInteger) < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%llu", child->value.unsignedInteger);
					return;
				}
			}
			else {
				if (fprintf(file, "%llu, ", child->value.unsignedInteger) < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%llu", child->value.unsignedInteger);
					return;
				}
			}
		}
		else if (child->type == GRAPH_SAVER_TYPE_SIGNED_INTEGER) {
			if (child->next == NULL) {
				if (fprintf(file, "%lld", child->value.signedInteger) < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%lld", child->value.signedInteger);
					return;
				}
			}
			else {
				if (fprintf(file, "%lld, ", child->value.signedInteger) < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%lld", child->value.signedInteger);
					return;
				}
			}
		}
		else if (child->type == GRAPH_SAVER_TYPE_STRING) {
			if (fprintf(file, "\"") < 0) {
				graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\\"", child->value.signedInteger);
				return;
			}
			for (size_t i = 0; i < child->value.str.length; i++) {
				if (fprintf(file, "%c", child->value.str.ptr[i]) < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "%c", child->value.str.ptr[i]);
					return;
				}
			}
			if (child->next == NULL) {
				if (fprintf(file, "\"") < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\");
					return;
				}
			}
			else {
				if (fprintf(file, "\", ") < 0) {
					graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\");
					return;
				}
			}
		}

		childLast = child;
		child = child->next;
	}

	if (childLast && childLast->type != GRAPH_SAVER_TYPE_COMPOSITE) {
		if (fprintf(file, " }") < 0) {
			graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, " }");
			return;
		}
	}
	else {
		if (fprintf(file, "\n") < 0) {
			graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\n");
			return;
		}
		int i = depth;
		while (i > 0) {
			if (fprintf(file, "\t") < 0) {
				graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "\\t");
				return;
			}
			i--;
		}

		if (parent != &saver->root && fprintf(file, "}") < 0) {
			graph_error_context_push_error(ctx, GRAPH_FAILED_TO_PRINT, NULL, NULL, "}");
			return;
		}
	}
}

void graph_saver_save(graph_error_context *ctx, graph_saver *saver, const char *filename) {

	FILE *file = fopen(filename, "w");
	if (file == NULL) {
		graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "missing file");
		return;
	}
	graph_saver_write_to_file(ctx, saver, file, &saver->root, -1);
	if (graph_error_context_check(ctx)) {
		graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "failed to write to file");
	}
	fclose(file);
}