#include "graph.h"

int main() {

	typedef struct MyStruct {
		double a;
		graph_u64 b;
		graph_s64 c;
		char str[64];
	} MyStruct;

	MyStruct myStructSave;
	myStructSave.a = 3.14;
	myStructSave.b = 101;
	myStructSave.c = -13;
	strncpy(myStructSave.str, "hello", 5);
	myStructSave.str[5] = '\0';

	// because we use the exit callback we don't need to check for errors since we will exit on error.
	graph_error_context ctx;
	graph_error_context_init(&ctx, graph_error_context_default_exit_callback);

	{
		graph_saver saver;
		graph_saver_init(&ctx, &saver);

		graph_saver_clear(&saver);
		graph_saver_node *nodeMyStruct = graph_saver_add_node(&ctx, &saver, NULL, "MyStruct");

		graph_saver_node *nodeA = graph_saver_add_node(&ctx, &saver, nodeMyStruct, "a");
		graph_saver_add_double(&ctx, &saver, nodeA, myStructSave.a);

		graph_saver_node *nodeB = graph_saver_add_node(&ctx, &saver, nodeMyStruct, "b");
		graph_saver_add_u64(&ctx, &saver, nodeB, myStructSave.b);

		graph_saver_node *nodeC = graph_saver_add_node(&ctx, &saver, nodeMyStruct, "c");
		graph_saver_add_s64(&ctx, &saver, nodeC, myStructSave.c);

		graph_saver_node *nodeStr = graph_saver_add_node(&ctx, &saver, nodeMyStruct, "str");
		graph_saver_add_string(&ctx, &saver, nodeStr, myStructSave.str, strlen(myStructSave.str));

		graph_saver_save(&ctx, &saver, "example.graph");

		graph_saver_deinit(&saver);
	}

	MyStruct myStructLoad;
	{
		graph_loader loader;
		graph_loader_init(&ctx, &loader);

		graph_loader_load(&ctx, &loader, "example.graph");

		// Access the implicit root node of the loader (where our data has been saved);
		graph_loader_node *nodeMyStruct = graph_loader_node_get_child(&ctx, loader.root);

		graph_loader_node *nodeA = graph_loader_node_get_child(&ctx, nodeMyStruct);
		graph_loader_node_to_double(&ctx, nodeA->children, &myStructLoad.a);

		graph_loader_node *nodeB = graph_loader_node_get_next(&ctx, nodeA);
		graph_loader_node_to_u64(&ctx, nodeB->children, &myStructLoad.b);

		graph_loader_node *nodeC = graph_loader_node_get_next(&ctx, nodeB);
		graph_loader_node_to_s64(&ctx, nodeC->children, &myStructLoad.c);

		graph_loader_node *nodeStr = graph_loader_node_get_next(&ctx, nodeC);
		graph_loader_node_to_string(&ctx, nodeStr->children, myStructLoad.str, 64);

		graph_loader_deinit(&loader);
	}

	return 1;
}
