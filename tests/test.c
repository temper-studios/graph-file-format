#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "graph_utility.h"
#include "graph_error_context.h"
#include "graph_pool.h"
#include "graph_loader.h"
#include "graph_saver.h"

#include "graph_utility.c"
#include "graph_error_context.c"
#include "graph_pool.c"
#include "graph_loader.c"
#include "graph_saver.c"

int main() {
#define DOUBLE_CHECK(a, b) if (a < b - EPSILON || a > b + EPSILON) { printf("double check failed at [%d]: %lf != %lf\n", __LINE__, a, b); return 0; }
#define U64_CHECK(a, b) if (a != b) { printf("u64 check failed at [%d]: %lu != %lu\n", __LINE__, a, b); return 0; }
#define S64_CHECK(a, b) if (a != b) { printf("u64 check failed at [%d]: %ld != %ld\n", __LINE__, a, b); return 0; }
#define STRING_CHECK(a, b) if (strcmp(a, b) != 0) { printf("string check failed at [%d]: %s != %s\n", __LINE__, a, b);  return 0; }
#define NODE_CHECK(a, b) if (!graph_loader_node_equal(a, b)) { printf("node check failed at [%d]: %s\n", __LINE__, b);  return 0; }


    const double EPSILON = 0.001;

    puts("Starting graph tests...");

    puts("Test0 starting...");
    {
        graph_loader loader;

        graph_load_file(&loader, "test0.graph", graph_error_context_default_exit_callback);

        graph_loader_node *root  = graph_loader_get_root(&loader);

        graph_loader_node *child = graph_loader_get_child(&loader, root);

        double number = 0.0;

        graph_loader_node_to_double(&loader, child, &number);
        DOUBLE_CHECK(number, 1.0);

        child = graph_loader_get_next(&loader, child);
        graph_loader_node_to_double(&loader, child, &number);
        DOUBLE_CHECK(number, 2.0);

        child = graph_loader_get_next(&loader, child);
        char str[64];
        graph_loader_node_to_string(&loader, child, str, 64);
        STRING_CHECK(str, "Hello");

        graph_unload(&loader);
    }
    puts("Test0 passed!");

    puts("Test1 starting...");
    {
        char *buffer = "1.0 2.0 \"Hello\"";

        graph_loader loader;

        graph_load_str(&loader, buffer, graph_utility_string_length(buffer), graph_error_context_default_exit_callback);

        graph_loader_node *root  = graph_loader_get_root(&loader);

        graph_loader_node *child = graph_loader_get_child(&loader, root);

        double number = 0.0;

        graph_loader_node_to_double(&loader, child, &number);
        DOUBLE_CHECK(number, 1.0);

        child = graph_loader_get_next(&loader, child);
        graph_loader_node_to_double(&loader, child, &number);
        DOUBLE_CHECK(number, 2.0);

        child = graph_loader_get_next(&loader, child);
        char str[64];
        graph_loader_node_to_string(&loader, child, str, 64);
        STRING_CHECK(str, "Hello");

        graph_unload(&loader);
    }
    puts("Test1 passed!");

    puts("Test2 starting...");
    {
        graph_loader loader;

        graph_load_file(&loader, "example.graph", graph_error_context_default_exit_callback);

        graph_loader_node *root = graph_loader_get_root(&loader);

        graph_loader_node *child = graph_loader_get_child(&loader, root);

        NODE_CHECK(child, "MyStruct");

        child = graph_loader_get_child(&loader, child);

        NODE_CHECK(child, "a");

        graph_loader_node *value = graph_loader_get_child(&loader, child);

        double number = 0.0;
        graph_loader_node_to_double(&loader, value, &number);
        DOUBLE_CHECK(number, 3.14);

        child = graph_loader_get_next(&loader, child);
        NODE_CHECK(child, "b");

        graph_u64 unumber = 0;
        value = graph_loader_get_child(&loader, child);
        graph_loader_node_to_u64(&loader, value, &unumber);
        U64_CHECK(unumber, (graph_u64)101);

        child = graph_loader_get_next(&loader, child);
        NODE_CHECK(child, "c");

        graph_s64 snumber = 0;
        value = graph_loader_get_child(&loader, child);
        graph_loader_node_to_s64(&loader, value, &snumber);
        S64_CHECK(snumber, (graph_s64)-13);

        child = graph_loader_get_next(&loader, child);
        NODE_CHECK(child, "str");
        char str[64];
        value = graph_loader_get_child(&loader, child);
        graph_loader_node_to_string(&loader, value, str, 64);
        STRING_CHECK(str, "hello");

        graph_unload(&loader);
    }
    puts("Test2 passed!");

    puts("Test3 starting...");
    {

        {
            graph_saver saver;
            graph_saver_start(&saver, graph_error_context_default_exit_callback);

            graph_u64 value1 = 10;
            graph_s64 value2 = -10;
            double value3 = 3.1;
            float value4 = 4.1f;
            const char *str = "hello";

            graph_saver_node *parent = graph_save_node(&saver, NULL, "MyData");

            graph_saver_node *nodeU64 = graph_save_node(&saver, parent, "u64");
            graph_save_u64(&saver, nodeU64, value1);

            graph_saver_node *nodeS64 = graph_save_node(&saver, parent, "s64");
            graph_save_s64(&saver, nodeS64, value2);

            graph_saver_node *nodeDouble = graph_save_node(&saver, parent, "double" );
            graph_save_double(&saver, nodeDouble, value3);

            graph_saver_node *nodeFloat = graph_save_node(&saver, parent, "float" );
            graph_save_double(&saver, nodeFloat, value4);

            graph_saver_node *nodeStr = graph_save_node(&saver, parent, "str");
            graph_save_string_null_terminated(&saver, nodeStr, str);

            graph_save_to_file(&saver, "test3.graph");

            graph_saver_finish(&saver);
        }

        {
            graph_loader loader;
            graph_load_file(&loader, "test3.graph", graph_error_context_default_exit_callback);

            graph_loader_node *root = graph_loader_get_root(&loader);

            graph_loader_node *child = graph_loader_get_child(&loader, root);

            NODE_CHECK(child, "MyData");

            child = graph_loader_get_child(&loader, child);
            NODE_CHECK(child, "u64");
            graph_loader_node *value = graph_loader_get_child(&loader, child);
            graph_u64 valueU64;
            graph_loader_node_to_u64(&loader, value, &valueU64);
            U64_CHECK(valueU64, (graph_u64)10);

            child = graph_loader_get_next(&loader, child);
            NODE_CHECK(child, "s64");
            graph_s64 valueS64 = 0;
            value = graph_loader_get_child(&loader, child);
            graph_loader_node_to_s64(&loader, value, &valueS64);
            S64_CHECK(valueS64, (graph_s64)-10);

            child = graph_loader_get_next(&loader, child);
            NODE_CHECK(child, "double");
            double valueDouble = 0;
            value = graph_loader_get_child(&loader, child);
            graph_loader_node_to_double(&loader, value, &valueDouble);
            DOUBLE_CHECK(valueDouble, 3.1);

            child = graph_loader_get_next(&loader, child);
            NODE_CHECK(child, "float");
            float valueFloat = 0;
            value = graph_loader_get_child(&loader, child);
            graph_loader_node_to_float(&loader, value, &valueFloat);
            DOUBLE_CHECK(valueFloat, 4.1f);

            child = graph_loader_get_next(&loader, child);
            NODE_CHECK(child, "str");
            char str[64];
            value = graph_loader_get_child(&loader, child);
            graph_loader_node_to_string(&loader, value, str, 64);
            STRING_CHECK(str, "hello");

            graph_unload(&loader);
        }
    }
    puts("Test3 passed!");

    return 1;
}
