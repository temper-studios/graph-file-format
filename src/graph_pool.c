// mallocs a pool with the given capacity (in bytes)
// returns false if the allocation failed, true if not
void graph_pool_init(graph_error_context *ctx, graph_pool *pool) {
	assert(GRAPH_POOL_SIZE >= sizeof(graph_pool_block *));
	pool->head = (graph_pool_block *)malloc(sizeof(graph_pool_block));
	if (!pool->head) {
		graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "out of memory in pool initialisation");
		return;
	}
	pool->head->next = NULL;
	pool->tail = pool->head;
	pool->cur = pool->tail;
	pool->index = sizeof(graph_pool_block *);
}

void graph_pool_deinit(graph_pool *pool) {
	for (graph_pool_block *block = pool->head; block != NULL;) {
		graph_pool_block *next = block->next;
		free(block);
		block = next;
	}
	pool->index = 0;
	pool->head = NULL;
	pool->tail = NULL;
	pool->cur = NULL;
}

char *graph_pool_push_string(graph_error_context *ctx, graph_pool *pool, graph_u64 *inoutLength, const char *str, graph_u64 strLength) {	
	char *ptr = graph_pool_push(ctx, pool, strLength + 1);
	if (graph_error_context_check(ctx)) {
		return NULL;
	}
	*inoutLength = 0;
	while (*inoutLength < strLength) {
		ptr[*inoutLength] = str[*inoutLength];
		(*inoutLength)++;
	}
	ptr[*inoutLength] = '\0';
	return ptr;
}

void graph_pool_allocate_new_or_get_block(graph_error_context *ctx, graph_pool *pool) {
	assert(pool);
	assert(pool->head);
	assert(pool->cur);

	graph_pool_block *block = NULL;

	if (pool->cur->next) {
		block = pool->cur->next;
	}
	else {
		block = (graph_pool_block *)malloc(sizeof(graph_pool_block));
		if (!block) {
			graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "out of memory in pool initialisation");
			return;
		}
		pool->tail->next = block;
		pool->tail = block;
		block->next = NULL;
	}

	pool->index = sizeof(graph_pool_block *);
	pool->cur = block;
}

char *graph_pool_push(graph_error_context *ctx, graph_pool *pool, graph_u64 size) {
	if (size >= GRAPH_POOL_SIZE) {
		graph_error_context_push_error(ctx, GRAPH_ERROR_UNKNOWN, NULL, NULL, "out of memory");
		return NULL;
	}
	if (pool->index + size >= GRAPH_POOL_SIZE) {	
		graph_pool_allocate_new_or_get_block(ctx, pool);
	}
	char *ptr = &pool->cur->data[pool->index];
	pool->index = pool->index + size;
	return ptr;
}

void graph_pool_clear(graph_pool *pool) {
	pool->index = sizeof(graph_pool_block *);
	pool->cur = pool->head;
}