#pragma once

#define GRAPH_POOL_SIZE 4096

typedef struct graph_pool_block {
	union {
		struct graph_pool_block *next;
		char data[GRAPH_POOL_SIZE];
	};
} graph_pool_block;

typedef struct graph_pool {
	graph_pool_block *head;
	graph_pool_block *tail;
	graph_pool_block *cur;
	graph_u64 index;
} graph_pool;

// mallocs a pool with the given capacity (in bytes)
// returns false if the allocation failed, true if not
void graph_pool_init(graph_error_context *ctx, graph_pool *pool);
void graph_pool_allocate_new_or_get_block(graph_error_context *ctx, graph_pool *pool);
char *graph_pool_push(graph_error_context *ctx, graph_pool *pool, graph_u64 size);
void graph_pool_clear(graph_pool *pool);
void graph_pool_deinit(graph_pool *pool);
char *graph_pool_push_string(graph_error_context *ctx, graph_pool *pool, graph_u64 *inoutLength, const char *str, graph_u64 strLength);