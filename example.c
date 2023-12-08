#define GF_IMPLEMENTATION
#include "graph_file_format.h"

int main(void) {

	// Declare some suitably complicated data we can save and load.
	struct InnerStruct {
		double a;
		int64_t b;
		int32_t list[4];
	};

	struct MyStruct {
		uint32_t a;
		float b;
		float xyz[3];
		struct InnerStruct inner;
		char str[4];
		uint64_t c;
	};

	{
		// Initialise our structure for saving.
		struct MyStruct myStruct;
		myStruct.a = 10;
		myStruct.b = 3.14f;
		myStruct.xyz[0] = 1.0;
		myStruct.xyz[1] = 2.0;
		myStruct.xyz[2] = 3.0;
		myStruct.str[0] = 'S';
		myStruct.str[1] = 't';
		myStruct.str[2] = 'r';
		myStruct.str[3] = '\0';
		myStruct.c = 63;

		myStruct.inner.a = -1.32;
		myStruct.inner.b = -232;
		myStruct.inner.list[0] = 0;
		myStruct.inner.list[1] = 1;
		myStruct.inner.list[2] = 2;
		myStruct.inner.list[3] = 3;

		// We use a gf_Saver to save .gf files.
		gf_Saver saver;
		gf_InitSaver(&saver, NULL);

		// Open the file ourselves.
		FILE *file = fopen("data.gf", "wb");
		if (!file) {
			puts("could not open file");
			return 0;
		}

		// A list contains lists or other data like integers, float and strings.
		gf_SaveStartList(&saver, file, "MyStruct");
		{
			gf_SaveVariableU32(&saver, file, "a", &myStruct.a);
			gf_SaveVariableF32(&saver, file, "b", &myStruct.b);
			gf_SaveVariableVec3(&saver, file, "xyz", &myStruct.xyz[0], &myStruct.xyz[1], &myStruct.xyz[2]);

			gf_SaveStartList(&saver, file, "Inner");
			{
				gf_SaveVariableF64(&saver, file, "a", &myStruct.inner.a);
				gf_SaveVariableS64(&saver, file, "b", &myStruct.inner.b);
				gf_SaveArrayS32(&saver, file, "list", myStruct.inner.list, 4);
			}
			gf_SaveEndList(&saver, file);

			gf_SaveVariableString(&saver, file, "str", myStruct.str);
			gf_SaveVariableU64(&saver, file, "c", &myStruct.c);

		}
		gf_SaveEndList(&saver, file);

		fclose(file);
	}


	{
		struct MyStruct myStruct;
		gf_Loader loader;
		int result = 0;

		result = gf_LoadFromFile(&loader, "data.gf", NULL);
		if (!result) {return 0; }
		{
			gf_LoaderNode *root = gf_GetRoot(&loader);

			gf_LoaderNode *nodeMyStruct = gf_GetChild(&loader, root);
			{
				gf_LoaderNode *child = gf_GetChild(&loader, nodeMyStruct);
				gf_LoadVariableU32(&loader, child, &myStruct.a);

				child = gf_GetNext(&loader, child);
				gf_LoadVariableF32(&loader, child, &myStruct.b);

				child = gf_GetNext(&loader, child);
				gf_LoadVariableVec3(&loader, child, &myStruct.xyz[0], &myStruct.xyz[1], &myStruct.xyz[2]);

				child = gf_GetNext(&loader, child);
				{
					gf_LoaderNode *childInner = gf_GetChild(&loader, child);
					gf_LoadVariableF64(&loader, childInner, &myStruct.inner.a);

					childInner = gf_GetNext(&loader, childInner);
					gf_LoadVariableS64(&loader, childInner, &myStruct.inner.b);

					childInner = gf_GetNext(&loader, childInner);
					gf_LoadArrayS32(&loader, childInner, myStruct.inner.list, 4);
				}

				child = gf_GetNext(&loader, child);
				gf_LoadVariableString(&loader, child, myStruct.str, 4);

				child = gf_GetNext(&loader, child);
				gf_LoadVariableU64(&loader, child, &myStruct.c);
			}

		}
		gf_Unload(&loader);
	}

	{
		struct MyStruct myStruct;
		gf_Loader loader;
		gf_LoadFromFile(&loader, "data.gf", NULL);
		{
			gf_LoaderNode *root = gf_GetRoot(&loader);

			gf_LoaderNode *nodeMyStruct = gf_FindFirstChild(&loader, root, "MyStruct");

			gf_LoaderNode *nodeA = gf_FindFirstChild(&loader, nodeMyStruct, "a");
			gf_LoadVariableU32(&loader, nodeA, &myStruct.a);

			gf_LoaderNode *nodeB = gf_FindFirstChild(&loader, nodeMyStruct, "b");
			gf_LoadVariableF32(&loader, nodeB, &myStruct.b);

			gf_LoaderNode *nodeXYZ = gf_FindFirstChild(&loader, nodeMyStruct, "xyz");
			gf_LoadVariableVec3(&loader, nodeXYZ, &myStruct.xyz[0], &myStruct.xyz[1], &myStruct.xyz[2]);

			gf_LoaderNode *nodeInner = gf_FindFirstChild(&loader, nodeMyStruct, "Inner");
			{
				gf_LoaderNode *nodeInnerA = gf_FindFirstChild(&loader, nodeInner, "a");
				gf_LoadVariableF64(&loader, nodeInnerA, &myStruct.inner.a);

				gf_LoaderNode *nodeInnerB = gf_FindFirstChild(&loader, nodeInner, "b");
				gf_LoadVariableS64(&loader, nodeInnerB, &myStruct.inner.b);

				gf_LoaderNode *nodeInnerList = gf_FindFirstChild(&loader, nodeInner, "list");
				gf_LoadArrayS32(&loader, nodeInnerList, myStruct.inner.list, 4);
			}

			gf_LoaderNode *nodeStr = gf_FindFirstChild(&loader, nodeMyStruct, "str");
			gf_LoadVariableString(&loader, nodeStr, myStruct.str, 4);

			gf_LoaderNode *nodeC = gf_FindFirstChild(&loader, nodeMyStruct, "c");
			gf_LoadVariableU64(&loader, nodeC, &myStruct.c);
		}
		gf_Unload(&loader);

	}
	

// Tests

#define GF_TEST_ASSERT(arg, str) if (!arg) { printf("[%s]: test failed! Check logs for more.", str); return 0; }
	{
		const char *str = "   ";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "3.0 { a }";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "{ a, b, c, d }";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "a, b, c, d";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 1, str);
		gf_Unload(&loader);
	}
	{
		gf_Loader loader;
		static char src[1 << 20];
		memset(src, '-', sizeof(src) - 1);
		int result = gf_LoadFromBuffer(&loader, src, sizeof(src) - 1, 0);
		GF_TEST_ASSERT(result == 0, "big test");
		gf_Unload(&loader);
	}

	{
		const char *str = "a b c d e ";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 1, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "a /*";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "a \"";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "1, 1.0";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 1, str);

		gf_LoaderNode *root = gf_GetRoot(&loader);
		gf_LoaderNode *intNode = gf_GetChild(&loader, root);
		GF_TEST_ASSERT(intNode, str);
		GF_TEST_ASSERT(gf_GetType(&loader, intNode) == GF_TOKEN_TYPE_INTEGER, str);
		gf_LoaderNode *floatNode = gf_GetNext(&loader, intNode);
		GF_TEST_ASSERT(floatNode, str);
		GF_TEST_ASSERT(gf_GetType(&loader, floatNode) == GF_TOKEN_TYPE_FLOAT, str);
		gf_Unload(&loader);
	}
	{
		const char *str =
			"MyStruct {\n"
			"  a { 3.14000 }\n"
			"  b { 101 }\n"
			"  c { -13 }\n"
			"  str { \"hello\" }\n"
			"}\n";

		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 1, str);
		gf_Unload(&loader);
	}
	{
		const char *str =
			"1.0\n"
			"2.0\n"
			"Hello\n";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 1, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "/* this is a comment */";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 1, str);
		gf_Unload(&loader);
	}
	{
		const char *str =
			"MyStruct {\n"
			"  a { 3.14000 }\n"
			"  b { *$3dnNONSENSE }\n"
			"  c { -13 }\n"
			"  str { \"hello\" }\n"
			"}\n";

		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	
	puts("All tests passed!");

	return 1;
}
