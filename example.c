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


	{
		graph_saver saver;

		// because we use the exit callback we don't need to check for errors since we will exit on error.
		graph_saver_start(&saver, graph_error_context_default_exit_callback);

		graph_saver_node *nodeMyStruct = graph_save_node(&saver, NULL, "MyStruct");

		graph_saver_node *nodeA = graph_save_node(&saver, nodeMyStruct, "a");
		graph_save_double(&saver, nodeA, myStructSave.a);

		graph_saver_node *nodeB = graph_save_node(&saver, nodeMyStruct, "b");
		graph_save_u64(&saver, nodeB, myStructSave.b);

		graph_saver_node *nodeC = graph_save_node(&saver, nodeMyStruct, "c");
		graph_save_s64(&saver, nodeC, myStructSave.c);

		graph_saver_node *nodeStr = graph_save_node(&saver, nodeMyStruct, "str");
		graph_save_string_span(&saver, nodeStr, myStructSave.str, graph_utility_string_length(myStructSave.str));

		graph_save_to_file(&saver, "example.graph");

		graph_saver_finish(&saver);
	}

	MyStruct myStructLoad;
	{
		graph_loader loader;

		graph_load_file(&loader, "example.graph", graph_error_context_default_exit_callback);

		// Access the implicit root node of the loader (where our data has been saved);
		graph_loader_node *root = graph_loader_get_root(&loader);

		graph_loader_node *nodeMyStruct = graph_loader_get_child(&loader, root);

		graph_loader_node *nodeA = graph_loader_get_child(&loader, nodeMyStruct);
		graph_loader_node_to_double(&loader, nodeA->children, &myStructLoad.a);

		graph_loader_node *nodeB = graph_loader_get_next(&loader, nodeA);
		graph_loader_node_to_u64(&loader, nodeB->children, &myStructLoad.b);

		graph_loader_node *nodeC = graph_loader_get_next(&loader, nodeB);
		graph_loader_node_to_s64(&loader, nodeC->children, &myStructLoad.c);

		graph_loader_node *nodeStr = graph_loader_get_next(&loader, nodeC);
		graph_loader_node_to_string(&loader, nodeStr->children, myStructLoad.str, 64);

		graph_unload(&loader);
	}

	return 1;
}
