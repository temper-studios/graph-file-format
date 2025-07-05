// Include the graph file format header.
// If you want to include the implementation (which should be included in exactly one translation unit of your project)
// you precede the include with either GF_IMPLEMENTATION or GF_IMPLEMENTATION_WITH_TESTS
// The latter includes the tests the former does not.
#define GF_IMPLEMENTATION_WITH_TESTS
#include "graph_file_format.h"

int main(void) {

	// As an example, lets include some semi-realistic data that we might 
	// want to save or load.

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

		// When we want to save we use the gf_Saver object.
		// Before we use this, we must call gf_InitSaver().
		gf_Saver saver;
		gf_InitSaver(&saver, NULL);

		// To save, we need a stream to save to. Let's open the file ourselves.
		FILE *file = fopen("data.gf", "wb");
		if (!file) {
			puts("could not open file");
			return 0;
		}

		// The saver is very simple and is just a helper that writes  to the file.
		// For ease, we want to match the structure of our existing data. Our struct 
		// has various levels of nesting. We can start a nested list by calling gf_SaveStartList.
		// This writes a identifier and an opening brace { to the file.
		gf_SaveStartList(&saver, file, "MyStruct");
		{
			// Now we can save variables to the file. We want to one to one match our struct because 
			// it is just easier to deal with that way. Each of these variable files saves the data
			// along with an identifier that we specify. Like so => a { 2 } or b { 3.2 }.
			gf_SaveVariableU32(&saver, file, "a", &myStruct.a);
			gf_SaveVariableF32(&saver, file, "b", &myStruct.b);
			gf_SaveVariableVec3(&saver, file, "xyz", &myStruct.xyz[0], &myStruct.xyz[1], &myStruct.xyz[2]);

			// Since Inner is nested inside MyStruct, we want to start a new nested list.
			gf_SaveStartList(&saver, file, "Inner");
			{
				gf_SaveVariableF64(&saver, file, "a", &myStruct.inner.a);
				gf_SaveVariableS64(&saver, file, "b", &myStruct.inner.b);
				gf_SaveArrayS32(&saver, file, "list", myStruct.inner.list, 4);
			}
			// Make sure to match a start list with an end list otherwise you get a malformed file!
			gf_SaveEndList(&saver, file);

			gf_SaveVariableString(&saver, file, "str", myStruct.str);
			gf_SaveVariableU64(&saver, file, "c", &myStruct.c);

		}
		// Make sure to match a start list with an end list otherwise you get a malformed file!
		gf_SaveEndList(&saver, file);

		// Close the file at the end and we are done!
		fclose(file);
	}

	{
		// Loading is slightly more involved.
		struct MyStruct myStruct;

		// We can use the gf_Loader object to help us load text.
		gf_Loader loader;
		int result = 0;

		// We can load directly from a file. This allocates an internal buffer that stores the file.
		// This acts as an initialisation for the loader. We don't have to initialise loader ourselves.
		// When you have called a load function on loader, you must match it with gf_Unload(), even if it fails.
		result = gf_LoadFromFile(&loader, "data.gf", NULL);
		if (!result) { gf_Unload(&loader); return 0;}
		{
			// Text is tokenised and parsed into a graph like structure. The graph is made of loader nodes.
			// These can be traversed and converted to values. 
			// When loaded, there is an intrinsic root node that always exists that we can start with.
			gf_LoaderNode *root = gf_GetRoot(&loader);

			// All parsed data in the text is a subtree of the root. The order of the text is maintained in parsing, so
			// we can expect the first child of the intrinsic root to be the MyStruct { identifier we saved previously.
			gf_LoaderNode *nodeMyStruct = gf_GetChild(&loader, root);
			{
				// Our MyStruct nodes contains a list of children.
				gf_LoaderNode *child = gf_GetChild(&loader, nodeMyStruct);
				gf_LoadVariableU32(&loader, child, &myStruct.a);

				// We get the next child in the the MyStruct child list.
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
		// You must call this when loading data into loader.
		gf_Unload(&loader);
	}

	{
		struct MyStruct myStruct;
		gf_Loader loader;
		gf_LoadFromFile(&loader, "data.gf", NULL);
		{
			gf_LoaderNode *root = gf_GetRoot(&loader);

			// This is a different API which instead searches for identifier nodes by name.
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

	// We can also load directly from a buffer
	const char *str =
		"MyStruct {\n"
		"  a { 3.14000 }\n"
		"  b { 101 }\n"
		"  c { -13 }\n"
		"  str { \"hello\" }\n"
		"}\n";

	gf_Loader loader;
	gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
	gf_Unload(&loader);
	
	return gf_Test();
}
