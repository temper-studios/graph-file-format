#pragma once
#ifndef GF_GRAPH_FILE_FORMAT_H
#define GF_GRAPH_FILE_FORMAT_H

// This header file parses the .gf/.graph file format.

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <memory.h>
#include <inttypes.h>

/*----------------------------------TYPEDEFS----------------------------------------*/

typedef uint32_t gf_u32;
typedef uint64_t gf_u64;
typedef int32_t  gf_s32;
typedef int64_t  gf_s64;
typedef float    gf_f32;
typedef double   gf_f64;

/*----------------------------------------------------------------------------------*/

/*-------------------------STRING CONVERSIONS---------------------------------------*/

/*
Name:        int gf_AreStringSpansEqual(const char *a, gf_u64 alength, const char *b, gf_u64 blength);
Description: Checks if *a and *b spanning strings are equal in value.
Assumptions: - *a and *b are not NULL and they are NULL terminated strings.
             - alength and blength are the string lengths of a and b respectively (they do not include the null terminating character).
Returns:     Returns 1 if they are equal. 0 if they are not.
*/
int gf_AreStringSpansEqual(const char *a, gf_u64 alength, const char *b, gf_u64 blength);

/*
Name:        gf_u64 gf_StringLength(const char *str);
Description: Computes the length of *str.
Assumptions: - *str is not NULL and is NULL terminated.
Returns:     The length of the string as a gf_u64.
*/
gf_u64 gf_StringLength(const char *str);

/*
Name:        int gf_StringSpanToU32(const char *start, uint64_t length, gf_u32 *value);
Description: Converts a string that spans a certain length into an unsigned 32 bit integer. 
             If the provided length is longer than the NULL terminated string then it will only parse
             up to the null terminated character.
Assumptions: - *start is not NULL and is NULL terminated. 
             - *value is not NULL
Returns:     Returns 1 if the conversion was successful. 0 if it was not successful.
*/
int gf_StringSpanToU32(const char *start, uint64_t length, gf_u32 *value);

/*
Name:        int gf_StringSpanToU64(const char *start, uint64_t length, gf_u64 *value);
Description: Converts a string that spans a certain length into an unsigned 64 bit integer. 
             If the provided length is longer than the NULL terminated string then it will only parse
             up to the null terminated character.
Assumptions: - *start is not NULL and is NULL terminated. 
             - *value is not NULL
Returns:     Returns 1 if the conversion was successful. 0 if it was not successful.
*/
int gf_StringSpanToU64(const char *start, uint64_t length, gf_u64 *value);

/*
Name:        int gf_StringSpanToS32(const char *start, uint64_t length, gf_s32 *value);
Description: Converts a string that spans a certain length into an signed 32 bit integer. 
             If the provided length is longer than the NULL terminated string then it will only parse
             up to the null terminated character.
Assumptions: - *start is not NULL and is NULL terminated. 
             - *value is not NULL
Returns:     Returns 1 if the conversion was successful. 0 if it was not successful.
*/
int gf_StringSpanToS32(const char *start, uint64_t length, gf_s32 *value);

/*
Name:        int gf_StringSpanToS64(const char *start, uint64_t length, gf_s64 *value);
Description: Converts a string that spans a certain length into an signed 64 bit integer. 
             If the provided length is longer than the NULL terminated string then it will only parse
             up to the null terminated character.
Assumptions: - *start is not NULL and is NULL terminated. 
             - *value is not NULL
Returns:     Returns 1 if the conversion was successful. 0 if it was not successful.
*/
int gf_StringSpanToS64(const char *start, uint64_t length, gf_s64 *value);

/*
Name:        int gf_StringSpanToF64(const char *start, uint64_t length, gf_f64 *value);
Description: Converts a string that spans a certain length into an 64 bit floating point number. 
             If the provided length is longer than the NULL terminated string then it will only parse
             up to the null terminated character.
Assumptions: - *start is not NULL and is NULL terminated. 
             - *value is not NULL
Returns:     Returns 1 if the conversion was successful. 0 if it was not successful.
*/
int gf_StringSpanToF64(const char *start, uint64_t length, gf_f64 *value);

/*
Name:        int gf_StringSpanToF32(const char *start, uint64_t length, gf_f32 *value);
Description: Converts a string that spans a certain length into an 32 bit floating point number. 
             If the provided length is longer than the NULL terminated string then it will only parse
             up to the null terminated character.
Assumptions: - *start is not NULL and is NULL terminated. 
             - *value is not NULL
Returns:     Returns 1 if the conversion was successful. 0 if it was not successful.
*/
int gf_StringSpanToF32(const char *start, uint64_t length, gf_f32 *value);

/*-----------------------------------------------------------------------------------*/

/*-------------------------------------TOKENS----------------------------------------*/

typedef enum gf_TokenType {
	GF_TOKEN_TYPE_ROOT,           // The type exclusively held by the hidden root token of the parser.
	GF_TOKEN_TYPE_NAME,           // A named/identifier token that isn't a string, integer, float or string.
	GF_TOKEN_TYPE_CURLY_CLOSE,    // This } token.
	GF_TOKEN_TYPE_COMMENT,        // The token associated with a comment which is /* and */
	GF_TOKEN_TYPE_COMPOSITE_TYPE, // This is a named/identifier token that contains other tokens.
	GF_TOKEN_TYPE_STRING,         // A string value token which is between two quotations ""
	GF_TOKEN_TYPE_FLOAT,          // A floating point value like 1.0 or 1.
	GF_TOKEN_TYPE_INTEGER,        // An integer value which is a essentially a number without a floating point.
	GF_TOKEN_TYPE_END_FILE,       // The token reserved for the end of a file.
	GF_TOKEN_TYPE_VALUE_ASSIGN    // This token { which designates an assignment is about to happen for a composite token.
} gf_TokenType;

/*
Holds information about each token that is created from the tokeniser.
Tokens are stored as a singly linked list where each token points to the next one.
*/
typedef struct gf_Token {
	const char *start;     // The pointer into the buffer that signifies the start string of the token.
	gf_u64 length;         // The length of the start string.
	gf_TokenType type;     // The token type
	gf_u64 lineno;         // The line number that this token starts on.
	gf_u64 colno;          // The column number on the current line that this token starts on.
	struct gf_Token *next; // The next token in the list
} gf_Token;

/*
Name:        void gf_PrintToken(gf_Token *token);
Description: Prints a token to stdout.
Assumptions: - *token is not NULL and is NULL terminated. 
Returns:     Nothing.
*/
void gf_PrintToken(gf_Token *token);

/*
Name:        void gf_PrintLineToken(gf_Token *token);
Description: Prints a token to stdout with a newline character at the end
Assumptions: - *token is not NULL and is NULL terminated. 
Returns:     Nothing.
*/
void gf_PrintLineToken(gf_Token *token);

/*
Name:        const char *gf_TokenTypeToString(gf_TokenType type);
Description: Returns a NULL terminated string which corresponds to the given name of a token type.
Assumptions: - type is a valid gf_TokenType
Returns:     The names of the token type as a string. The string is NULL terminated.
*/
const char *gf_TokenTypeToString(gf_TokenType type);

/*----------------------------------------------------------------------------------*/

/*-------------------------LOG AND FUNCTION POINTERS--------------------------*/

// Used to specify what kind of "error" you want to log.
typedef enum gf_LogLevel {
	GF_LOG_ERROR = 0,
	GF_LOG_WARNING,
	GF_LOG_INFO
} gf_LogLevel;

/*
Name:        gf_LogFunctionPtr
Description: This is the function signature for the log function. This can be user defined. The provided token can be NULL.
*/
typedef void (*gf_LogFunctionPtr)(gf_LogLevel, int, gf_Token *, const char *, va_list);
/*
Name:        gf_AllocatorFunctionPtr
Description: This is the function signature for the allocation function. This can be user defined. By default this is malloc().
             It must allocate pointers that are not invalidated with subsequent allocations.
*/
typedef void *(*gf_AllocatorFunctionPtr)(size_t);
/*
Name:        gf_FreeFunctionPtr
Description: This is the function signature for the free function. This can be user defined. By default this is free().
			 This function must be able to deal with NULL pointers.
*/
typedef void (*gf_FreeFunctionPtr)(void *);

/*
Name:        gf_LogAllocateFreeFunctions
Description: This is a helper struct that stores function pointers responsible for logging, allocating and freeing.
             If any of these are NULL then gf_DefaultLog(), malloc() and free() are used respectively. 
			 The allocator must obey the rules of malloc() which means it's pointers can not be invalidated after they 
			 are allocated. free() must work on a NULL.
*/
typedef struct gf_LogAllocateFreeFunctions {
	gf_LogFunctionPtr Log;            // A pointer to the log function you want to use. If this is NULL gf_DefaultLog() is used
	gf_AllocatorFunctionPtr Allocate; // A pointer to the allocation function you want to use. If this is NULL malloc() is used.
	gf_FreeFunctionPtr Free;          // A pointer to the free function you want to use. If this is NULL free() is used.
} gf_LogAllocateFreeFunctions;

/*
Name:        void gf_DefaultLog(gf_LogLevel level, int lineno, gf_Token *, const char *format, va_list vlist);
Description: The default logging function used if none is specified. This prints to stdout.
Assumptions: - The token can be NULL.
             - *format is assumed to be not NULL.
Returns:     Nothing.
*/
void gf_DefaultLog(gf_LogLevel level, int lineno, gf_Token *, const char *format, va_list vlist);

/*
Name:        void gf_Log(gf_LogFunctionPtr Log, gf_LogLevel level, int lineno, gf_Token *, const char *format, ...);
Description: An internal function that is called when the logging macro is invoked. This function calls the user specified log function.
Assumptions: - The token can be NULL.
             - *format is assumed to be not NULL.
Returns:     Nothing.
*/
void gf_Log(gf_LogFunctionPtr Log, gf_LogLevel level, int lineno, gf_Token *, const char *format, ...);

/*
Name:        GF_LOG(loaderOrSaver, level, ...) gf_Log(loaderOrSaver->Log, level, __LINE__, NULL, __VA_ARGS__)
Description: A helper macro that calls gf_Log which in turn, calls the users logging function. A macro is used so the line number can be obtained
Assumptions: - loaderOrSave is not NULL and is of type gf_Loader or gf_Saver.
Returns:     Nothing.
*/
#define GF_LOG(loaderOrSaver, level, ...) gf_Log(loaderOrSaver->Log, level, __LINE__, NULL, __VA_ARGS__)

/*
Name:        GF_LOG(loaderOrSaver, level, ...) gf_Log(loaderOrSaver->Log, level, __LINE__, NULL, __VA_ARGS__)
Description: The same as GF_LOG but with a token too.
Assumptions: - loaderOrSave is not NULL and is of type gf_Loader or gf_Saver.
             - The token can be NULL.
Returns: Nothing.
*/
#define GF_LOG_WITH_TOKEN(loaderOrSaver, level, token, ...) gf_Log(loaderOrSaver->Log, level, __LINE__, token, __VA_ARGS__)

/*-----------------------------------------------------------------------------------*/

/*--------------------------------------SAVER----------------------------------------*/

// A helper function used to save data. Use this to begin "serialisation".
typedef struct gf_Saver {
	gf_LogFunctionPtr Log; // The function used to log errors.
	unsigned int indent;   // Tracks the current indent level of your data. Each level of nested data has an indentation.
} gf_Saver;

/*
Name:        void gf_InitSaver(gf_Saver *saver, gf_LogFunctionPtr logfunction);
Description: Initialises the gf_Saver. Must be called before using the gf_Saver. 
             You can call it again to reset the saver.
             If the logfunction is set to the NULL the default gf_DefaultLog function is used instead.
             logfunction is a function pointer to a logging function of gf_LogFunctionPtr signature.
Assumptions: - saver is not NULL.
             - logfunction can be NULL. If it is, the default logging function is used.
Returns:     Nothing.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);
}
*/
void gf_InitSaver(gf_Saver *saver, gf_LogFunctionPtr logfunction);

/*
Name:        int gf_PrintIndent(gf_Saver *saver, FILE *file);
Description: Internal function used to insert spaces into the file corresponding to the current indent level of the saver.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
*/
int gf_PrintIndent(gf_Saver *saver, FILE *file);

/*
Name:        int gf_SaveVariableS64(gf_Saver *saver, FILE *file, const char *identifier, gf_s64 *value);
Description: Saves a variable to the file with the given value. This will be in the format "a { 2 }" 
             where "a" is the identifier and "2" is the value.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
			 - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_s64 value = 1;

	gf_SaveVariableS64(&saver, file, "identifier", &value);

	fclose(file);
}
*/
int gf_SaveVariableS64(gf_Saver *saver, FILE *file, const char *identifier, gf_s64 *value);

/*
Name:        int gf_SaveVariableS32(gf_Saver *saver, FILE *file, const char *identifier, gf_s32 *value);
Description: Saves a variable to the file with the given value. This will be in the format "a { 2 }" 
             where "a" is the identifier and "2" is the value.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_s32 value = 1;

	gf_SaveVariableS32(&saver, file, "identifier", &value);

	fclose(file);
}
*/
int gf_SaveVariableS32(gf_Saver *saver, FILE *file, const char *identifier, gf_s32 *value);

/*
Name:        int gf_SaveVariableU32(gf_Saver *saver, FILE *file, const char *identifier, gf_u32 *value);
Description: Saves a variable to the file with the given value. This will be in the format "a { 2 }" 
             where "a" is the identifier and "2" is the value.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_u32 value = 1;

	gf_SaveVariableU32(&saver, file, "identifier", &value);

	fclose(file);
}
*/
int gf_SaveVariableU32(gf_Saver *saver, FILE *file, const char *identifier, gf_u32 *value);

/*
Name:        int gf_SaveVariableU64(gf_Saver *saver, FILE *file, const char *identifier, gf_u64 *value);
Description: Saves a variable to the file with the given value. This will be in the format "a { 2 }" 
             where "a" is the identifier and "2" is the value.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_u64 value = 1;

	gf_SaveVariableU64(&saver, file, "identifier", &value);

	fclose(file);
}
*/
int gf_SaveVariableU64(gf_Saver *saver, FILE *file, const char *identifier, gf_u64 *value);

/*
Name:        int gf_SaveVariableString(gf_Saver *saver, FILE *file, const char *identifier, const char *str);
Description: Saves a string to the file with the given value. This will be in the format "a { "Hello, world" }" 
             where "a" is the identifier and "Hello, world" is the string.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *str is not NULL and is a NULL terminated string.
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	const char *str = "hello, world";

	gf_SaveVariableString(&saver, file, "identifier", str);

	fclose(file);
}
*/
int gf_SaveVariableString(gf_Saver *saver, FILE *file, const char *identifier, const char *str);

/*
Name:        int gf_SaveVariableF32(gf_Saver *saver, FILE *file, const char *identifier, gf_f32 *value);
Description: Saves a variable to the file with the given value. This will be in the format "a { 2.0 }" 
             where "a" is the identifier and "2.0" is the value.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	float value = 1.0;

	gf_SaveVariableF32(&saver, file, "identifier", &value);

	fclose(file);
}
*/
int gf_SaveVariableF32(gf_Saver *saver, FILE *file, const char *identifier, gf_f32 *value);

/*
Name:        int gf_SaveVariableF64(gf_Saver *saver, FILE *file, const char *identifier, gf_f64 *value);
Description: Saves a variable to the file with the given value. This will be in the format "a { 2.0 }" 
             where "a" is the identifier and "2.0" is the value.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	double value = 1.0;

	gf_SaveVariableF64(&saver, file, "identifier", &value);

	fclose(file);
}
*/
int gf_SaveVariableF64(gf_Saver *saver, FILE *file, const char *identifier, gf_f64 *value);

/*
Name:        int gf_SaveVariableStringSpan(gf_Saver *saver, FILE *file, const char *identifier, const char *str, int strLen);
Description: Saves a string span to the file with the given value. This will be in the format "a { "Hello, world" }" 
             where "a" is the identifier and "Hellow, world" is the string.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *str is not NULL and is a NULL terminated string.
             - if strlen is longer than the NULL terminated string then only the string up to the NULL terminated character is saved.
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	const char *str = "hello, world";

	gf_SaveVariableStringSpan(&saver, file, "identifier", str, 5);

	fclose(file);
}
*/
int gf_SaveVariableStringSpan(gf_Saver *saver, FILE *file, const char *identifier, const char *str, int strLen);

/*
Name:        int gf_SaveVariableVec3(gf_Saver *saver, FILE *file, const char *identifier, gf_f32 *x, gf_f32 *y, gf_f32 *z);
Description: Saves a vector to the file with the given value. This will be in the format "a { 2.0, 1.0, 1.2 }" 
             where "a" is the identifier and "2.0, 1.0, 1.2" is x, y and z respectively.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *x, *y and *z are not NULL
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	float x = 1.0f;
	float y = 2.0f;
	float z = 3.0f;

	gf_SaveVariableVec3(&saver, file, "identifier", x, y, z);

	fclose(file);
}
*/
int gf_SaveVariableVec3(gf_Saver *saver, FILE *file, const char *identifier, gf_f32 *x, gf_f32 *y, gf_f32 *z);

/*
Name:        int gf_SaveArrayU64(gf_Saver *saver, FILE *file, const char *identifier, gf_u64 *value, int count);
Description: Saves an array to the file. This will be in the format "a { 1, 2, ... }" 
             up to a "count" of values.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
             - value is atleast of length count.
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_u64 value[] = { 1, 2 };
	gf_SaveArrayU64(&saver, file, "identifier", value, 2);

	fclose(file);
}
*/
int gf_SaveArrayU64(gf_Saver *saver, FILE *file, const char *identifier, gf_u64 *value, int count);

/*
Name:        int gf_SaveArrayS64(gf_Saver *saver, FILE *file, const char *identifier, gf_s64 *value, int count);
Description: Saves an array to the file. This will be in the format "a { 1, 2, ... }" 
             up to a "count" of values.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
             - value is atleast of length count.
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_s64 value[] = { 1, 2 };
	gf_SaveArrayS64(&saver, file, "identifier", value, 2);

	fclose(file);
}
*/
int gf_SaveArrayS64(gf_Saver *saver, FILE *file, const char *identifier, gf_s64 *value, int count);

/*
Name:        int gf_SaveArrayS32(gf_Saver *saver, FILE *file, const char *identifier, gf_s32 *value, int count);
Description: Saves an array to the file. This will be in the format "a { 1, 2, ... }" 
             up to a "count" of values.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - *value is not NULL
             - value is atleast of length count.
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_u32 value[] = { 1, 2 };
	gf_SaveArrayS32(&saver, file, "identifier", value, 2);

	fclose(file);
}
*/
int gf_SaveArrayS32(gf_Saver *saver, FILE *file, const char *identifier, gf_s32 *value, int count);

/*
Name:        int gf_SaveStartList(gf_Saver *saver, FILE *file, const char *identifier);
Description: Saves a composite list to the file. This will be in the format "a { " 
             If you want to start writing an arbitrary list of values then this is a good start.
             Must be capped off with a call to gf_SaveEndList otherwise your file will be malformed.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - When finished saveing to this list you end with a call to gf_SaveEndList.
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_SaverStartList(&saver, file);
	{
		float value = 1.0f;
		gf_SaveVariableF64(&saver, file, "myfloat", &value);
	}
	gf_SaverEndList(&saver, file);

	fclose(file);
}
*/
int gf_SaveStartList(gf_Saver *saver, FILE *file, const char *identifier);

/*
Name:        int gf_SaveEndList(gf_Saver *saver, FILE *file);
Description: Saves the end of a composite list to the file. This will be in the format " }" 
             If you want to end a list this is how you do it.
Assumptions: - gf_InitSaver must have been called on saver atleast once.
             - *saver is not NULL
             - *file is not NULL and is a valid file.
             - *identifier is not NULL and is a NULL terminated string.
             - Assumes gf_SaveEndList was called to start the list you want to end.
Returns:     1 if it was successful. 0 if not. If an error occurs this is logged.
Examples:
{
	gf_Saver saver;
	gf_SaveInit(&saver, NULL);

	FILE *file = fopen(file, "myfile", "rb");
	if (!file) return 0;

	gf_SaverStartList(&saver, file);
	{
		float value = 1.0f;
		gf_SaveVariableF64(&saver, file, "myfloat", &value);
	}
	gf_SaverEndList(&saver, file);

	fclose(file);
}
*/
int gf_SaveEndList(gf_Saver *saver, FILE *file);

/*-----------------------------------------------------------------------------------*/

/*------------------------------------LOADER-----------------------------------------*/

/*
Stores a pointer to a buffer that you want to tokeniser. Tracks the current index 
into this buffer as you tokenise. Also tracks things like the current line number and 
column number.
*/
typedef struct gf_Tokeniser {
	const char *buffer; // A pointer to a null terminated buffer that needs to be tokenised
	gf_u64 index;       // The internal tracking index that records the current character in the buffer
	gf_u64 count;       // The total size of the buffer.
	gf_u64 lineno;      // The current line number.
	gf_u64 colno;       // The current column number.
} gf_Tokeniser;

/*
Name:        void gf_InitTokeniser(gf_Tokeniser *tokeniser, const char *buffer, gf_u64 count);
Description: Initialises the tokeniser. This must be called before using the tokeniser. You can call this multiple times to reset the tokeniser. 
             The buffer you wish to tokenise is passed in, along with the buffer length (count). This is not copied. So, while you 
			 are tokenising and using the tokeniser, this buffer must be valid.
			 This buffer must be NULL terminated.
Assumptions: - *tokeniser is not NULL
			 - *buffer is not NULL and is NULL terminated.
			 - The count must represent a valid length for the buffer. If NULL terminating character is 
			   encountered before the count is reached then tokenising will stop.
Returns:     Nothing.
*/
void gf_InitTokeniser(gf_Tokeniser *tokeniser, const char *buffer, gf_u64 count);

/*
Name:        char gf_GetChar(gf_Tokeniser *tokeniser);
Description: Get the character at the tokenisers internal current index, from the buffer pointed to by the tokeniser.
             If the buffer's end is reached this will return a NULL terminating character.
Assumptions: - tokeniser has been initialised with a call to gf_InitTokeniser();
             - *tokeniser is not NULL
			 - Assumes the buffer that the tokeniser is pointing to is still valid.
Returns:     Returns the character of the buffer pointed to by the tokeniser, at the tracked index. Returns a NULL terminator if the end of this buffer is reached.
*/
char gf_GetChar(gf_Tokeniser *tokeniser);

/*
Name:        void gf_IncrementIndex(gf_Tokeniser *tokeniser);
Description: Increments the internal tracking index of the tokeniser. If the end of the buffer tracked
             by the tokeniser is reached, this tracking index is not incremented.
Assumptions: - tokeniser has been initialised with a call to gf_InitTokeniser();
			 - *tokeniser is not NULL
Returns:     Nothing.
*/
void gf_IncrementIndex(gf_Tokeniser *tokeniser);

/*
Name:        void gf_IncrementLineNo(gf_Tokeniser *tokeniser);
Description: Increments the current line number that is being tracked by the tokeniser.
Assumptions: - tokeniser has been initialised with a call to gf_InitTokeniser();
			 - *tokeniser is not NULL
Returns:     Nothing.
*/
void gf_IncrementLineNo(gf_Tokeniser *tokeniser);

/*
Name:        const char *gf_Ptr(gf_Tokeniser *tokeniser);
Description: Returns a pointer into the buffer pointed to by the tokeniser at the internal tokeniser index.
Assumptions: - tokeniser has been initialised with a call to gf_InitTokeniser();
			 - *tokeniser is not NULL
			 - Assumes the buffer that the tokeniser is pointing to is still valid.
Returns:     A pointer into the buffer that the tokeniser points to at the location of the tokenisers internal tracking index.
             If the end of the buffer is reached by the internal tracking index, then a NULL is returned.
*/
const char *gf_Ptr(gf_Tokeniser *tokeniser);

/*
A node that is stored as a graph that represents parsed text.
A node has an associated token. When the text forms a list these 
nodes are stored as a doubly linked list where they are next to each other.
When data is nested, nodes will be stored as children of other nodes.
*/
typedef struct gf_LoaderNode {
	gf_Token *token;                     // The token associated with this list
	struct gf_LoaderNode *parent;        // The parent of this node. This will only ever be NULL for the root token.
	struct gf_LoaderNode *next;          // The next node in the list.
	struct gf_LoaderNode *prev;          // The prev node in the list
	struct gf_LoaderNode *childrenHead;  // The head of the children list of this node
	struct gf_LoaderNode *childrenTail;  // The tail of the children list of this node.
	struct gf_LoaderNode *nextAllocated; // A helper linked list that tracks allocated nodes.
} gf_LoaderNode;

/*
A helper function that is responsible for storing all tokens, nodes and potentially a buffer that
was allocated when opening a file.
*/
typedef struct gf_Loader {
	gf_LogFunctionPtr Log;            // The function used to log. 
	gf_AllocatorFunctionPtr Allocate; // The function used to allocate. 
	gf_FreeFunctionPtr Free;          // The function used to free
	gf_Token rootToken;               // The root token that is always guaranteed to exist when parsing text.
	gf_Token *lastToken;              // The last token in the token list.
	gf_Token *firstToken;             // The first token in the token list.
	gf_Token *curToken;               // The current token in the token list.
	gf_LoaderNode *rootNode;          // The root node in the node graph.
	gf_LoaderNode *lastNode;          // The last allocated node in the node graph.
	char *fileContentsBuffer;         // A pointer that points to memory allocated from a file.
	gf_u64 nestLevel;                 // When parsing, tracks how many {} we are nested in.
} gf_Loader;

/*
Name:        void gf_InitLoader(gf_Loader *loader, gf_LogAllocateFreeFunctions *helperfunctions);
Description: Initialises the loader. This is called internally when calling one of the gf_Load...() functions. Do not call again until gf_Unload is called.
             Ideally don't call at all and just call one of the load function instead to begin using the loader.
	         The loader is initialised with a log, allocate and 
             free function pointer data structure. If this is NULL, the default logging function, malloc() and free() are used respectively.
			 If it is not NULL and any of the helper functions within are NULL these default to gf_DefaultLog, malloc() and free respectively.
			 The allocator MUST have pointers that are not invalidated as more data is allocated.
Assumptions: - *loader is not NULL
			 - gf_InitLoader() has not been called yet.
Returns:     Nothing.
*/
void gf_InitLoader(gf_Loader *loader, gf_LogAllocateFreeFunctions *helperfunctions);

/*
Name:        void gf_IncrementLastTokenLength(gf_Loader *loader);
Description: Increments the length of the last allocated token by loader.
Assumptions: - gf_InitLoader() has been called on *loader.
             - gf_LoadInternal() has been called. i.e. some data has been loaded.
			 - *loader is not NULL.
Returns:     Nothing.
*/
void gf_IncrementLastTokenLength(gf_Loader *loader);

/*
Name:        int gf_AddToken(gf_Loader *loader, const char *start, gf_TokenType type, gf_u64 lineno, gf_u64 colno);
Description: Allocates a new token used by the loader. The user specified allocator is used when the loader is 
             initialised. The token is given the specified type and lineno along with the start string within the loaded
			 buffer. It starts with a length of 1.
Assumptions: - gf_InitLoader() has been called on *loader.
			 - gf_LoadInternal() has been called. i.e. some data has been loaded.
			 - *loader is not NULL 
			 - *start is not NULL
Returns:     Returns 1 if it succeeds. 0 if it fails. The error is logged.
*/
int gf_AddToken(gf_Loader *loader, const char *start, gf_TokenType type, gf_u64 lineno, gf_u64 colno);

/*
Name:        char *gf_AllocateNullTerminatedBufferFromFile(gf_Loader *loader, const char *filename, gf_u64 *bufferCountWithNullTerminator);
Description: Opens the given file. Allocates it's contents using loader's defined allocator (+1 to include the null terminator).
             Then closes the file.
			 This allocated buffer is returned as a NULL terminated string.
			 The size of the allocated buffer is stored in bufferCountWithNullTerminator.
Assumptions: - gf_InitLoader() has been called on *loader.
			 - *loader is not NULL
			 - *filename is NOT NULL
			 - bufferCountWithNullTerminator is not NULL.
Returns:     Returns a terminated string that holds the entire contents of the file +1 for the NULL terminator. 
             Returns NULL if this failed and the error is logged.
*/
char *gf_AllocateNullTerminatedBufferFromFile(gf_Loader *loader, const char *filename, gf_u64 *bufferCountWithNullTerminator);

/*
Name:        int gf_TokeniseInternal(gf_Loader *loader, gf_Tokeniser *tokeniser);
Description: An internal function that begins tokenising the buffer pointed to by the tokeniser.
Assumptions: - gf_InitLoader() has been called on *loader.
             - gf_InitTokeniser() has been called on *tokeniser.
			 - *loader is not NULL
			 - *tokeniser is not NULL
Returns:     Returns 1 if this succeeds. 0 if this fails. The error is logged.
*/
int gf_TokeniseInternal(gf_Loader *loader, gf_Tokeniser *tokeniser);

/*
Name:        int gf_Tokenise(gf_Loader *loader, const char *buffer, gf_u64 count);
Description: Tokenises the NULL terminated buffer that has the length count.
Assumptions: - gf_InitLoader() has been called on *loader.
			 - *buffer is NULL terminated.
			 - count is a valid length for the buffer.
			 - *loader is not NULL
			 - *buffer is not NULL
Returns:     Returns 1 if this succeeds. 0 if this fails. The error is logged.
*/
int gf_Tokenise(gf_Loader *loader, const char *buffer, gf_u64 count);

/*
Name:        gf_Token *gf_ConsumeToken(gf_Loader *loader);
Description: Returns the next token in the token list and increments the current tracking index that 
			 points to the current token.
Assumptions: - gf_InitLoader() has been called on *loader.
			 - *loader is not NULL
Returns:     Returns NULL if there are no more tokens, else returns the token as a pointer.
*/
gf_Token *gf_ConsumeToken(gf_Loader *loader);

/*
Name:        gf_Token *gf_PeekToken(gf_Loader *loader);
Description: Returns the current token.
Assumptions: - gf_InitLoader() has been called on *loader.
			 - *loader is not NULL
Returns:     Returns the current token. This can be NULL.
*/
gf_Token *gf_PeekToken(gf_Loader *loader);

/*
Name:        gf_LoaderNode *gf_AddNode(gf_Loader *loader, gf_Token *token);
Description: Allocates a node using the specified allocation function. Assigns the token to this 
             node. Returns the newly allocated node.
Assumptions: - gf_InitLoader() has been called on *loader.
			 - *token is not NULL.
Returns:     Returns the newly allocated node. Returns NULL if it fails. The error is logged.
*/
gf_LoaderNode *gf_AddNode(gf_Loader *loader, gf_Token *token);

/*
Name:        void gf_AddChild(gf_LoaderNode *parent, gf_LoaderNode *child);
Description: Adds the child node to the parents children list of nodes.
Assumptions: - gf_InitLoader() has been called on *loader.
			 - *parent is not NULL.
			 - *child is not NULL.
Returns:     Nothing.
*/
void gf_AddChild(gf_LoaderNode *parent, gf_LoaderNode *child);

/*
Name:        int gf_Parse(gf_Loader *loader, gf_LoaderNode *parentNode);
Description: Parses the token list. The tokeniser must have been called before this happens and 
             the root node of loader must have been created. This is a recursive function. The first time 
			 you call it, parentNode must be the root node of the loader.
Assumptions: - gf_InitLoader() has been called on *loader.
	         - The loader has performed tokenisation.
			 - *loader is not NULL.
			 - *parentNode is not NULL and needs to be the root node when you call this initially.
Returns:     Returns 1 if it succeeds. Returns 0 if it fails. The error is logged.
*/
int gf_Parse(gf_Loader *loader, gf_LoaderNode *parentNode);

/*
Name:        int gf_LoadInternal(gf_Loader *loader, const char *buffer, gf_u64 bufferCount);
Description: Begins tokenising and parsing the passed in buffer, preparing data that can be queried by the user.
			 Buffer needs to be NULL terminated. The bufferCount needs to represent a valid span for the buffer.
Assumptions: - *loader is not NULL.
			 - *buffer is not NULL.
			 - bufferCount is a valid length for the buffer.
Returns:     Returns 1 if it succeeds. Returns 0 if it fails. The error is logged.
*/
int gf_LoadInternal(gf_Loader *loader, const char *buffer, gf_u64 bufferCount);

/*
Name:        int gf_LoadFromBuffer(gf_Loader *loader, const char *buffer, gf_u64 bufferCount, gf_LogAllocateFreeFunctions *funcs);
Description: Begins tokenising and parsing the passed in buffer, preparing data that can be queried by the user.
             This should be the first thing you call before using the loader. After you are done, you need to call gf_Unload(), even if this function fails.
			 The buffer needs to be NULL terminated. The bufferCount needs to represent a valid span for the buffer.
			 *funcs can be NULL or contain NULL function pointers. In this case, the default function pointers are used,
			 gf_DefaultLog, malloc() and free().
			 This function allocates things using the passed in allocation function.
Assumptions: - *loader is not NULL.
			 - *buffer is not NULL.
			 - funcs can be NULL.
			 - bufferCount is a valid length for the buffer.
Returns:     Returns 1 if it succeeds. Returns 0 if it fails. The error is logged.
Examples: 
{
	gf_Loader loader;
	const char *buffer = "mydata { 1.0 }";
	gf_LoadFromBuffer(&loader, buffer, gf_StringLength(buffer), NULL);
	// ... do stuff ...
	gf_Unload(&loader);
}
*/
int gf_LoadFromBuffer(gf_Loader *loader, const char *buffer, gf_u64 bufferCount, gf_LogAllocateFreeFunctions *funcs);

/*
Name:        int gf_LoadFromFile(gf_Loader *loader, const char *filename, gf_LogAllocateFreeFunctions *funcs);
Description: Begins tokenising and parsing the passed in buffer, preparing data that can be queried by the user.
			 This should be the first thing you call before using the loader. After you are done, you need to call gf_Unload(), even if this function fails.
			 This opens the specified file and allocates its contents into a buffer. This buffer is then tokenised and parsed to 
			 be inspected by the user.
			 *funcs can be NULL or contain NULL function pointers. In this case, the default function pointers are used,
			 gf_DefaultLog, malloc() and free().
			 This function allocates things using the passed in allocation function.
			 This open
Assumptions: - *loader is not NULL.
			 - *filename is not NULL.
			 - funcs can be NULL.
Returns:     Returns 1 if it succeeds. Returns 0 if it fails. The error is logged.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);
	// ... do stuff ...
	gf_Unload(&loader);
}
*/
int gf_LoadFromFile(gf_Loader *loader, const char *filename, gf_LogAllocateFreeFunctions *funcs);

/*
Name:        void gf_Unload(gf_Loader *loader);
Description: Frees all data allocated by the loader using the user defined Free() function. Must be called after gf_Load...() 
             function has been called prefably when you are finished with the loader. It does not matter,
			 if the load failed, you still need to call gf_Unload.
			 After unload is called you must load the loader again with new data if you want to use it.
			 If you called gf_LoadFromFile, this deallocates the buffer that was allocated.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called.
             - *loader is not NULL.
Returns:     Nothing
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);
	// ... do stuff ...
	gf_Unload(&loader);
}
*/
void gf_Unload(gf_Loader *loader);

/*
Name:        int gf_LoaderNodeToU32(gf_Loader *loader, gf_LoaderNode *node, gf_u32 *value);
Description: Converts the value of the node and copies it into value.
             Does type checking of the node. 
			 A u32 node is an integer node. This takes the form => 123
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - *value is not NULL.
			 - node can be NULL.
Returns:     Returns 1 if the conversion was successful. Returns 0 if node was NULL, the node 
             was not an integer type or the conversion fail. The error is logged.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);
	
	gf_LoaderNode *root = gf_GetRoot(loader);
	gf_LoaderNode *child = gf_GetChild(loader, root);

	gf_u32 value;
	gf_LoaderNodeToU32(&loader, child, &value);

	gf_Unload(&loader);
}
*/
int gf_LoaderNodeToU32(gf_Loader *loader, gf_LoaderNode *node, gf_u32 *value);

/*
Name:        int gf_LoaderNodeToU64(gf_Loader *loader, gf_LoaderNode *node, gf_u64 *value);
Description: Converts the value of the node and copies it into value.
			 Does type checking of the node.
			 A u64 node is an integer node. This takes the form => 123
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - *value is not NULL.
			 - node can be NULL.
Returns:     Returns 1 if the conversion was successful. Returns 0 if node was NULL, the node
			 was not an integer type or the conversion fail. The error is logged.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(loader);
	gf_LoaderNode *child = gf_GetChild(loader, root);

	gf_u64 value;
	gf_LoaderNodeToU64(&loader, child, &value);

	gf_Unload(&loader);
}
*/
int gf_LoaderNodeToU64(gf_Loader *loader, gf_LoaderNode *node, gf_u64 *value);

/*
Name:        int gf_LoaderNodeToS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value);
Description: Converts the value of the node and copies it into value.
			 Does type checking of the node.
			 A s32 node is an integer node. This takes the form => 123
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - *value is not NULL.
			 - node can be NULL.
Returns:     Returns 1 if the conversion was successful. Returns 0 if node was NULL, the node
			 was not an integer type or the conversion fail. The error is logged.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(loader);
	gf_LoaderNode *child = gf_GetChild(loader, root);

	gf_s32 value;
	gf_LoaderNodeToS32(&loader, child, &value);

	gf_Unload(&loader);
}
*/
int gf_LoaderNodeToS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value);

/*
Name:        int gf_LoaderNodeToS64(gf_Loader *loader, gf_LoaderNode *node, gf_s64 *value);
Description: Converts the value of the node and copies it into value.
			 Does type checking of the node.
			 A s64 node is an integer node. This takes the form => 123
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - *value is not NULL.
			 - node can be NULL.
Returns:     Returns 1 if the conversion was successful. Returns 0 if node was NULL, the node
			 was not an integer type or the conversion fail. The error is logged.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(loader);
	gf_LoaderNode *child = gf_GetChild(loader, root);

	gf_s64 value;
	gf_LoaderNodeToS64(&loader, child, &value);

	gf_Unload(&loader);
}
*/
int gf_LoaderNodeToS64(gf_Loader *loader, gf_LoaderNode *node, gf_s64 *value);

/*
Name:        int gf_LoaderNodeToF64(gf_Loader *loader, gf_LoaderNode *node, gf_f64 *value);
Description: Converts the value of the node and copies it into value.
			 Does type checking of the node.
			 A f64 node is an floating point node. This takes the form => 1.0
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - *value is not NULL.
			 - node can be NULL.
Returns:     Returns 1 if the conversion was successful. Returns 0 if node was NULL, the node
			 was not an floating point type or the conversion fail. The error is logged.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(loader);
	gf_LoaderNode *child = gf_GetChild(loader, root);

	gf_f64 value;
	gf_LoaderNodeToF64(&loader, child, &value);

	gf_Unload(&loader);
}
*/
int gf_LoaderNodeToF64(gf_Loader *loader, gf_LoaderNode *node, gf_f64 *value);

/*
Name:        int gf_LoaderNodeToF32(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *value);
Description: Converts the value of the node and copies it into value.
			 Does type checking of the node.
			 A f32 node is an floating point node. This takes the form => 1.0
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - *value is not NULL.
			 - node can be NULL.
Returns:     Returns 1 if the conversion was successful. Returns 0 if node was NULL, the node
			 was not an floating point type or the conversion fail. The error is logged.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(loader);
	gf_LoaderNode *child = gf_GetChild(loader, root);

	gf_f32 value;
	gf_LoaderNodeToF32(&loader, child, &value);

	gf_Unload(&loader);
}
*/
int gf_LoaderNodeToF32(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *value);

/*
Name:        int gf_LoaderNodeToString(gf_Loader *loader, gf_LoaderNode *node, char *src, gf_u64 srcCapacityIncludesNullTerminator);
Description: Converts the value of the node and copies it into src.
		     srcCapacityIncludesNullTerminator is the length of src + the NULL terminating character.
			 Does type checking of the node.
			 A string node is a string node. This takes the form => "Hello!"
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - *src is not NULL.
			 - node can be NULL.
Returns:     Returns 1 if the conversion was successful. Returns 0 if node was NULL, the node
			 was not an string type or the passed in capacity is not big enough to fit the size of the token. The error is logged.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(loader);
	gf_LoaderNode *child = gf_GetChild(loader, root);

	gf_f32 value;
	gf_LoaderNodeToF32(&loader, child, &value);

	gf_Unload(&loader);
}
*/
int gf_LoaderNodeToString(gf_Loader *loader, gf_LoaderNode *node, char *src, gf_u64 srcCapacityIncludesNullTerminator);

/*
Name:        gf_LoaderNode *gf_GetRoot(gf_Loader *loader);
Description: Gets the root node of the parsed contents of a file. gf_LoadFromBuffer or gf_LoadFromFile 
			 must have been called and must have been successful before a valid root exists.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
Returns:     Returns the root node. This cannot be NULL if the assumptions are satisfied.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_Unload(&loader);
}
*/
gf_LoaderNode *gf_GetRoot(gf_Loader *loader);

/*
Name:        gf_LoaderNode *gf_GetNext(gf_Loader *loader, gf_LoaderNode *node);
Description: Gets the next node after *node.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
Returns:     Returns the next node. Returns NULL if there is no next node or node is NULL
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);
	gf_LoaderNode *nextNode = gf_GetNext(&loader, root);

	gf_Unload(&loader);
}
*/
gf_LoaderNode *gf_GetNext(gf_Loader *loader, gf_LoaderNode *node);

/*
Name:        gf_LoaderNode *gf_GetChild(gf_Loader *loader, gf_LoaderNode *node);
Description: Gets the first child node in *node's child list.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
Returns:     Returns the first child node. If there are no children or node is NULL this returns NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);
	gf_LoaderNode *childNode = gf_GetChild(&loader, root);

	gf_Unload(&loader);
}
*/
gf_LoaderNode *gf_GetChild(gf_Loader *loader, gf_LoaderNode *node);

/*
Name:        gf_TokenType gf_GetType(gf_Loader *loader, gf_LoaderNode *node);
Description: Gets the token type of the node. 
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
Returns:     Returns the token type of the node. If the node is NULL then the returned token type is GF_TOKEN_TYPE_END_FILE
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);
	
	gf_TokenType tokenType = gf_GetType(&loader, root);

	gf_Unload(&loader);
}
*/
gf_TokenType gf_GetType(gf_Loader *loader, gf_LoaderNode *node);

/*
Name:        gf_LoaderNode *gf_FindFirstChild(gf_Loader *loader, gf_LoaderNode *node, const char *str);
Description: Finds the first child in the node's child list that has a name that is equal to str.
             Only finds nodes that are composite/named types (named nodes that may have children).
			 If two nodes have the same name, the first child in the list is returned. The list of children
			 has the same order as the children in the file.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - str is not NULL and is NULL terminated.
Returns:     Returns the first child in the node's child list that has the same name as str. Returns NULL if 
             this could not be found or node is NULL. See description for more details.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_Unload(&loader);
}
*/
gf_LoaderNode *gf_FindFirstChild(gf_Loader *loader, gf_LoaderNode *node, const char *str);

/*
Name:       gf_LoaderNode *gf_FindFirstNext(gf_Loader *loader, gf_LoaderNode *node, const char *str);
Description: Finds the first node in the passed in node's next list. 
             The first node that has a name equal to str in the next list, starting at node, is returned.
			 Only finds nodes that are composite/named types (named nodes that may have children).
			 If two nodes have the same name, the first node in the list is returned. 
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - str is not NULL and is NULL terminated.
Returns:     Returns the first node after the current node that has the same name as str. Returns NULL if
			 this could not be found or node is NULL. See description for more details.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_LoaderNode *nextNode = gf_FindFirstNext(&loader, childNode, "NextName");

	gf_Unload(&loader);
}
*/
gf_LoaderNode *gf_FindFirstNext(gf_Loader *loader, gf_LoaderNode *node, const char *str);

/*
Name:        int gf_LoadVariableU32(gf_Loader *loader, gf_LoaderNode *node, gf_u32 *value);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
             If the node has the form => Var { 1 }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - value is not NULL
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described 
             in the description, the conversion to the value failed, the value node in is the wrong type or
			 node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_u32 value = 0;
	gf_LoadVariableU32(&loader, childNode, &value);

	gf_Unload(&loader);
}
*/
int gf_LoadVariableU32(gf_Loader *loader, gf_LoaderNode *node, gf_u32 *value);

/*
Name:        int gf_LoadVariableU64(gf_Loader *loader, gf_LoaderNode *node, gf_u64 *value);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
			 If the node has the form => Var { 1 }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - value is not NULL
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described
			 in the description, the conversion to the value failed, the value node in is the wrong type or
			 node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_u64 value = 0;
	gf_LoadVariableU64(&loader, childNode, &value);

	gf_Unload(&loader);
}
*/
int gf_LoadVariableU64(gf_Loader *loader, gf_LoaderNode *node, gf_u64 *value);

/*
Name:        int gf_LoadVariableS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
			 If the node has the form => Var { 1 }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - value is not NULL
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described
			 in the description, the conversion to the value failed, the value node in is the wrong type or
			 node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_s32 value = 0;
	gf_LoadVariableS32(&loader, childNode, &value);

	gf_Unload(&loader);
}
*/
int gf_LoadVariableS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value);

/*
Name:        int gf_LoadVariableS64(gf_Loader *loader, gf_LoaderNode *node, gf_s64 *value);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
			 If the node has the form => Var { 1 }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - value is not NULL
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described
			 in the description, the conversion to the value failed, the value node in is the wrong type or
			 node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_s64 value = 0;
	gf_LoadVariableS64(&loader, childNode, &value);

	gf_Unload(&loader);
}
*/
int gf_LoadVariableS64(gf_Loader *loader, gf_LoaderNode *node, gf_s64 *value);

/*
Name:        int gf_LoadVariableF64(gf_Loader *loader, gf_LoaderNode *node, gf_f64 *value);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
			 If the node has the form => Var { 1.0 }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - value is not NULL
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described
			 in the description, the conversion to the value failed, the value node in is the wrong type or
			 node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_f64 value = 0;
	gf_LoadVariableF64(&loader, childNode, &value);

	gf_Unload(&loader);
}
*/
int gf_LoadVariableF64(gf_Loader *loader, gf_LoaderNode *node, gf_f64 *value);

/*
Name:        int gf_LoadVariableF32(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *value);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
			 If the node has the form => Var { 1.0 }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - value is not NULL
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described
			 in the description, the conversion to the value failed, the value node in is the wrong type or
			 node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_f32 value = 0;
	gf_LoadVariableF32(&loader, childNode, &value);

	gf_Unload(&loader);
}
*/
int gf_LoadVariableF32(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *value);

/*
Name:        int gf_LoadVariableVec3(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *x, gf_f32 *y, gf_f32 *z);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
			 If the node has the form => Var { 1.0, 2.0, 3.0 }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - x, y, z is not NULL
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described
			 in the description, the conversion to the value failed, the value nodes are in the wrong type or
			 node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_f32 x = 0.0f;
	gf_f32 y = 0.0f;
	gf_f32 z = 0.0f;
	gf_LoadVariableVec3(&loader, childNode, &x, &y, &z);

	gf_Unload(&loader);
}
*/
int gf_LoadVariableVec3(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *x, gf_f32 *y, gf_f32 *z);

/*
Name:        int gf_LoadVariableString(gf_Loader *loader, gf_LoaderNode *node, char *str, gf_u64 lenWithNullTerminator);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
			 If the node has the form => Var { "Hello, world" }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - str is not NULL
			 - str is NULL terminated.
			 - lenWithNullTerminator corresponds to the length of the str + 1 for the null terminator.
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described
			 in the description, the lenWithNullTerminator is not big enough to fit the value of the node, the value nodes are in the wrong type or the
			 node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	char strbuffer[5];
	gf_LoadVariableString(&loader, childNode, strbuffer, 5);

	gf_Unload(&loader);
}
*/
int gf_LoadVariableString(gf_Loader *loader, gf_LoaderNode *node, char *str, gf_u64 lenWithNullTerminator);

/*
Name:        int gf_LoadArrayS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value, gf_u64 count);
Description: A helper function that takes a set of nodes in a certain form and converts them to a value.
			 If the node has the form => Var { 1, 2, 3, ... }, where Var corresponds to the passed in node, the value inside
			 the braces is converted and copied into value.
Assumptions: - gf_LoadFromBuffer or gf_LoadFromFile has been called and was successful.
			 - *loader is not NULL.
			 - node can be NULL
			 - value is not NULL.
			 - count corresponds to a valid range of the value array.
Returns:     Returns 1 if successful. Returns 0 if, either, the node is not in the form as described
			 in the description, the conversion to the value failed, the value nodes are in the wrong type, the length of the 
			 list is bigger than the count and array provided or node is NULL.
Examples:
{
	gf_Loader loader;
	const char *filename = "myfile.gf";
	gf_LoadFromFile(&loader, filename, NULL);

	gf_LoaderNode *root = gf_GetRoot(&loader);

	gf_LoaderNode *childNode = gf_FindFirstChild(&loader, root, "ChildName");

	gf_s32 arr[3] = {1, 2, 3};
	gf_LoadArrayS32(&loader, childNode, arr, 3);

	gf_Unload(&loader);
}
*/
int gf_LoadArrayS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value, gf_u64 count);

/*-----------------------------------------------------------------------------------*/

/*-------------------------TESTING---------------------------------------------------*/

#ifdef GF_IMPLEMENTATION_WITH_TESTS

#define GF_TEST_ASSERT(arg, str) if (!(arg)) { printf("[%s]: test failed! Check logs for more.", str); return 0; }

int gf_Test(void);

#endif

/*-----------------------------------------------------------------------------------*/

// IMPLEMENTATION
#if defined(GF_IMPLEMENTATION) || defined(GF_IMPLEMENTATION_WITH_TESTS)

/*-------------------------STRING CONVERSIONS---------------------------------------*/

int gf_AreStringSpansEqual(const char *a, gf_u64 alength, const char *b, gf_u64 blength) {
	assert(a);
	assert(b);

	if (alength != blength) return 0;
	gf_u64 i = 0;
	while (i < alength) {
		if (a[i] == '\0' && b[i] != '\0') return 0;
		if (a[i] != '\0' && b[i] == '\0') return 0;
		if (a[i] != b[i]) return 0;
		i++;
	}
	return 1;
}

gf_u64 gf_StringLength(const char *str) {
	assert(str);

	gf_u64 len = 0;
	while (str[len] != '\0') len++;
	return len;
}

int gf_StringSpanToU32(const char *start, uint64_t length, gf_u32 *value) {
	assert(start);

	if (*start == '-') {
		return 0;
	}

	gf_u32 i = strtoul(start, NULL, 10);
	// 0 could mean a failure, or it could just mean the string is actually 0. 
	if (i == 0) {
		uint64_t index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return 0;
		}
	}
	if (i == ULONG_MAX) {
		return 0;
	}
	*value = i;
	return 1;
}

int gf_StringSpanToU64(const char *start, uint64_t length, gf_u64 *value) {
	assert(start);

	if (*start == '-') {
		return 0;
	}

	gf_u64 i = strtoull(start, NULL, 10);
	// 0 could mean a failure, or it could just mean the string is actually 0. 
	if (i == 0) {
		uint64_t index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return 0;
		}
	}
	if (i == ULLONG_MAX) {
		return 0;
	}
	*value = i;
	return 1;
}

int gf_StringSpanToS32(const char *start, uint64_t length, gf_s32 *value) {
	assert(start);

	gf_s32 i = strtol(start, NULL, 10);
	// 0 could mean a failure
	if (i == 0) {
		uint64_t index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return 0;
		}
	}
	if (i == LONG_MIN || i == LONG_MAX) {
		return 0;
	}
	*value = i;
	return 1;
}

int gf_StringSpanToS64(const char *start, uint64_t length, gf_s64 *value) {
	assert(start);

	gf_s64 i = strtoll(start, NULL, 10);
	// 0 could mean a failure, or it could just mean the string is actually 0.
	if (i == 0) {
		uint64_t index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return 0;
		}
	}
	if (i == LLONG_MIN || i == LLONG_MAX) {
		return 0;
	}
	*value = i;
	return 1;
}

int gf_StringSpanToF64(const char *start, uint64_t length, gf_f64 *value) {
	assert(start);

	double d = strtod(start, NULL);
	// 0.0 could mean a failure, or it could just mean the string is actually 0.
	if (d == 0.0) {
		uint64_t index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return 0;
		}
	}
	else if (d == HUGE_VAL || d == -HUGE_VAL) {
		return 0;
	}
	*value = d;
	return 1;
}

int gf_StringSpanToF32(const char *start, uint64_t length, gf_f32 *value) {
	assert(start);

	float f = strtof(start, NULL);
	// 0.0 could mean a failure, or it could just mean the string is actually 0
	if (f == 0.0) {
		uint64_t index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return 0;
		}
	}
	else if (f == HUGE_VALF || f == -HUGE_VALF) {
		return 0;
	}
	*value = f;
	return 1;
}

/*-----------------------------------------------------------------------------------*/

/*-------------------------------------TOKENS----------------------------------------*/

void gf_PrintToken(gf_Token *token) {
	assert(token);

	for (gf_u64 i = 0; i < token->length; i++) {
		printf("%c", token->start[i]);
	}
}

void gf_PrintLineToken(gf_Token *token) {
	assert(token);

	gf_PrintToken(token);
	printf("\n");
}

const char *gf_TokenTypeToString(gf_TokenType type) {
	switch (type) {
	case GF_TOKEN_TYPE_ROOT: return "GF_TOKEN_TYPE_ROOT";
	case GF_TOKEN_TYPE_NAME: return "GF_TOKEN_TYPE_NAME";
	case GF_TOKEN_TYPE_CURLY_CLOSE: return "GF_TOKEN_TYPE_CURLY_CLOSE";
	case GF_TOKEN_TYPE_COMMENT: return "GF_TOKEN_TYPE_COMMENT";
	case GF_TOKEN_TYPE_COMPOSITE_TYPE: return "GF_TOKEN_TYPE_COMPOSITE_TYPE";
	case GF_TOKEN_TYPE_STRING: return "GF_TOKEN_TYPE_STRING";
	case GF_TOKEN_TYPE_FLOAT: return "GF_TOKEN_TYPE_FLOAT";
	case GF_TOKEN_TYPE_INTEGER: return "GF_TOKEN_TYPE_INTEGER";
	case GF_TOKEN_TYPE_END_FILE: return "GF_TOKEN_TYPE_END_FILE";
	case GF_TOKEN_TYPE_VALUE_ASSIGN: return "GF_TOKEN_TYPE_VALUE_ASSIGN";
	default: return "<unknown token type>";
	}
}

/*-----------------------------------------------------------------------------------*/

/*-------------------------------------LOGGING----------------------------------------*/

void gf_DefaultLog(gf_LogLevel level, int lineno, gf_Token *token, const char *format, va_list vlist) {
	switch (level) {
	case GF_LOG_ERROR:   printf("[ERROR][%d]", lineno);   break;
	case GF_LOG_WARNING: printf("[WARNING][%d]", lineno); break;
	case GF_LOG_INFO:    printf("[INFO][%d]", lineno);    break;
	}
	if (token) {
		printf("[\"");
		gf_PrintToken(token);
		printf("\"]");
		printf("[lineno: %" PRIu64 "]", token->lineno);
		printf("[colno: %" PRIu64 "]", token->colno);
	}
	printf(": ");
	vprintf(format, vlist);
	printf("\n");
}

void gf_Log(gf_LogFunctionPtr Log, gf_LogLevel level, int lineno, gf_Token *token, const char *format, ...) {
	va_list args;
	va_start(args, format);
	Log(level, lineno, token, format, args);
	va_end(args);
};

/*-----------------------------------------------------------------------------------*/

/*-------------------------------------SAVER----------------------------------------*/

void gf_InitSaver(gf_Saver *saver, gf_LogFunctionPtr logfunction) {
	assert(saver);

	saver->indent = 0;
	if (logfunction == NULL) {
		saver->Log = gf_DefaultLog;
	}
}

int gf_PrintIndent(gf_Saver *saver, FILE *file) {
	assert(saver);
	assert(file);

	int result = 0;
	for (unsigned int i = 0; i < saver->indent; i++) {
		result = fprintf(file, "  ");
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_PrintIndent", result);
			return 0;
		}
	}
	return 1;
}

int gf_SaveVariableS64(gf_Saver *saver, FILE *file, const char *identifier, gf_s64 *value) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(value);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { %" PRIi64 " }\n", identifier, *value);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableS64", result);
		return 0;
	}
	return 1;
}

int gf_SaveVariableS32(gf_Saver *saver, FILE *file, const char *identifier, gf_s32 *value) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(value);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { %" PRIi32 " }\n", identifier, *value);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableS32", result);
		return 0;
	}
	return 1;
}

int gf_SaveVariableU32(gf_Saver *saver, FILE *file, const char *identifier, gf_u32 *value) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(value);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { %" PRIu32 " }\n", identifier, *value);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableU32", result);
		return 0;
	}
	return 1;
}


int gf_SaveVariableU64(gf_Saver *saver, FILE *file, const char *identifier, gf_u64 *value) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(value);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { %" PRIu64 " }\n", identifier, *value);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableU64", result);
		return 0;
	}
	return 1;
}

int gf_SaveVariableString(gf_Saver *saver, FILE *file, const char *identifier, const char *str) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(str);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { \"%s\" }\n", identifier, str);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableString", result);
		return 0;
	}
	return 1;
}

int gf_SaveVariableF32(gf_Saver *saver, FILE *file, const char *identifier, gf_f32 *value) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(value);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { %f }\n", identifier, *value);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableF32", result);
		return 0;
	}

	return 1;
}

int gf_SaveVariableF64(gf_Saver *saver, FILE *file, const char *identifier, gf_f64 *value) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(value);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { %lf }\n", identifier, *value);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableF64", result);
		return 0;
	}

	return 1;
}

int gf_SaveVariableStringSpan(gf_Saver *saver, FILE *file, const char *identifier, const char *str, int strLen) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(str);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { \"", identifier);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableStringSpan", result);
		return 0;
	}

	for (int i = 0; i < strLen; i++) {
		result = fprintf(file, "%c", str[i]);
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fputc failed with value [%d] in gf_SaveVariableStringSpan", result);
			return 0;
		}
	}
	result = fprintf(file, "\" }\n");
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableStringSpan", result);
		return 0;
	}

	return 1;
}

int gf_SaveVariableVec3(gf_Saver *saver, FILE *file, const char *identifier, gf_f32 *x, gf_f32 *y, gf_f32 *z) {
	assert(saver);
	assert(file);
	assert(identifier);
	assert(x && y && z);

	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s { %f, %f, %f }\n", identifier, *x, *y, *z);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveVariableVec3", result);
		return 0;
	}

	return 1;
}

int gf_SaveArrayU64(gf_Saver *saver, FILE *file, const char *identifier, gf_u64 *value, int count) {
	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	if (count) {
		result = fprintf(file, "%s { ", identifier);
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayU64", result);
			return 0;
		}
		result = fprintf(file, "%" PRIu64, value[0]);
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayU64", result);
			return 0;
		}
		for (int i = 1; i < count; i++) {
			result = fprintf(file, ", %" PRIi64, value[i]);
			if (result < 0) {
				GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayU64", result);
				return 0;
			}
		}
		result = fprintf(file, " }\n");
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayU64", result);
			return 0;
		}
	}

	return 1;
}

int gf_SaveArrayS64(gf_Saver *saver, FILE *file, const char *identifier, gf_s64 *value, int count) {
	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	if (count) {
		result = fprintf(file, "%s { ", identifier);
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayI64", result);
			return 0;
		}
		result = fprintf(file, "%" PRIi64, value[0]);
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayI64", result);
			return 0;
		}
		for (int i = 1; i < count; i++) {
			result = fprintf(file, ", %" PRIi64, value[i]);
			if (result < 0) {
				GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayI64", result);
				return 0;
			}
		}
		result = fprintf(file, " }\n");
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayI64", result);
			return 0;
		}
	}

	return 1;
}


int gf_SaveArrayS32(gf_Saver *saver, FILE *file, const char *identifier, gf_s32 *value, int count) {
	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	if (count) {
		result = fprintf(file, "%s { ", identifier);
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayS32", result);
			return 0;
		}
		result = fprintf(file, "%" PRIi32, value[0]);
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayS32", result);
			return 0;
		}
		for (int i = 1; i < count; i++) {
			result = fprintf(file, ", %" PRIi32, value[i]);
			if (result < 0) {
				GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayS32", result);
				return 0;
			}
		}
		result = fprintf(file, " }\n");
		if (result < 0) {
			GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveArrayS32", result);
			return 0;
		}
	}

	return 1;
}

int gf_SaveStartList(gf_Saver *saver, FILE *file, const char *identifier) {
	int result = 0;

	result = gf_PrintIndent(saver, file);
	if (!result) { return 0; }

	result = fprintf(file, "%s {\n", identifier);
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveStartList", result);
		return 0;
	}
	saver->indent++;

	return 1;
}

int gf_SaveEndList(gf_Saver *saver, FILE *file) {
	int result = 0;

	if (saver->indent) {
		for (unsigned int i = 0; i < saver->indent - 1; i++) {
			result = fprintf(file, "  ");
			if (result < 0) {
				GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveEndList", result);
				return 0;
			}
		}
	}
	result = fprintf(file, "}\n");
	if (result < 0) {
		GF_LOG(saver, GF_LOG_ERROR, "fprintf failed with value [%d] in gf_SaveEndList", result);
		return 0;
	}
	if (saver->indent) saver->indent--;

	return 1;
}

/*-----------------------------------------------------------------------------------*/

/*-------------------------------------LOADER----------------------------------------*/

void gf_InitLoader(gf_Loader *loader, gf_LogAllocateFreeFunctions *helperfunctions) {
	assert(loader);

	if (!helperfunctions || helperfunctions->Log == NULL) {
		loader->Log = gf_DefaultLog;
	}
	else {
		loader->Log = helperfunctions->Log;
	}
	if (!helperfunctions || helperfunctions->Allocate == NULL) {
		loader->Allocate = malloc;
	}
	else {
		loader->Allocate = helperfunctions->Allocate;
	}
	if (!helperfunctions || helperfunctions->Free == NULL) {
		loader->Free = free;
	}
	else {
		loader->Free = helperfunctions->Free;
	}

	loader->curToken = NULL;
	loader->rootNode = NULL;
	loader->lastNode = NULL;
	loader->firstToken = NULL;

	// Set up the root token
	loader->rootToken.start = "root";
	loader->rootToken.length = 4;
	loader->rootToken.lineno = 0;
	loader->rootToken.next = NULL;
	loader->rootToken.type = GF_TOKEN_TYPE_ROOT;

	loader->lastToken = &loader->rootToken;

	loader->fileContentsBuffer = NULL;
	loader->nestLevel = 0;
}

void gf_IncrementLastTokenLength(gf_Loader *loader) {
	assert(loader);
	assert(loader->lastToken);
	loader->lastToken->length++;
}

int gf_AddToken(gf_Loader *loader, const char *start, gf_TokenType type, gf_u64 lineno, gf_u64 colno) {
	assert(loader->lastToken);

	if (!start) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Expecting a token but at the end of the file/buffer");
		return 0;
	}

	gf_Token *token = (gf_Token *)loader->Allocate(sizeof(gf_Token));
	if (!token) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Out of memory in gf_AddToken");
		return 0;
	}

	if (!loader->firstToken) {
		loader->firstToken = token;
	}

	loader->lastToken->next = token;
	loader->lastToken = token;

	token->start = start;
	token->type = type;
	token->length = 1;
	token->lineno = lineno;
	token->colno = colno;
	token->next = NULL;

	return 1;
}

char *gf_AllocateNullTerminatedBufferFromFile(gf_Loader *loader, const char *filename, gf_u64 *bufferCountWithNullTerminator) {
	assert(loader);
	assert(filename);
	assert(bufferCountWithNullTerminator);

	uint64_t fileSize = 0;
	uint64_t readBytes = 0;
	FILE *file = NULL;

	file = fopen(filename, "rb");
	if (file == NULL) {
		GF_LOG(loader, GF_LOG_ERROR, "Failed to load file [%s]. It could not be opened", filename);
		return NULL;
	}
	fseek(file, 0, SEEK_END);
	fileSize = (uint64_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	char *buffer = (char *)loader->Allocate(fileSize + 1);
	if (buffer == NULL) {
		GF_LOG(loader, GF_LOG_ERROR, "Out of memory. Failed to allocate buffer for file [%s]", filename);
		fclose(file);
		return NULL;
	}

	readBytes = fread(buffer, fileSize, sizeof(char), file);
	if (readBytes != 1) {
		GF_LOG(loader, GF_LOG_ERROR, "Failed to read file [%s]", filename);
		fclose(file);
		return NULL;
	}

	fclose(file);

	buffer[fileSize] = '\0';

	*bufferCountWithNullTerminator = fileSize + 1;

	return buffer;
}

void gf_InitTokeniser(gf_Tokeniser *tokeniser, const char *buffer, gf_u64 count) {
	assert(tokeniser);
	assert(buffer);

	tokeniser->buffer = buffer;
	tokeniser->count = count;
	tokeniser->index = 0;
	tokeniser->lineno = 1;
	tokeniser->colno = 1;
}

char gf_GetChar(gf_Tokeniser *tokeniser) {
	if (tokeniser->index >= tokeniser->count) {
		return '\0';
	}
	return tokeniser->buffer[tokeniser->index];
}

void gf_IncrementIndex(gf_Tokeniser *tokeniser) {
	if (tokeniser->index < tokeniser->count) {
		tokeniser->index++;
		tokeniser->colno++;
	}
}

void gf_IncrementLineNo(gf_Tokeniser *tokeniser) {
	tokeniser->lineno++;
	tokeniser->colno = 1;
}

const char *gf_Ptr(gf_Tokeniser *tokeniser) {
	if (tokeniser->index < tokeniser->count) {
		return &tokeniser->buffer[tokeniser->index];
	}
	return NULL;
}

int gf_TokeniseInternal(gf_Loader *loader, gf_Tokeniser *tokeniser) {
	assert(loader);
	assert(tokeniser);

	int result = 0;

	while (1) {

		if (gf_GetChar(tokeniser) == '\0') {

			result = gf_AddToken(loader, "<end token>", GF_TOKEN_TYPE_END_FILE, tokeniser->lineno, tokeniser->colno);
			if (!result) {
				GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Failed to add end file token");
				return 0;
			}
			break;
		}
		else if (gf_GetChar(tokeniser) == '{') {
			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_VALUE_ASSIGN, tokeniser->lineno, tokeniser->colno);
			if (!result) {
				GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Failed to add value assign token");
				return 0;
			}
			gf_IncrementIndex(tokeniser);
		}
		else if (gf_GetChar(tokeniser) == '}') {
			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_CURLY_CLOSE, tokeniser->lineno, tokeniser->colno);
			if (!result) {
				GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Failed to add curly close token");
				return 0;
			}
			gf_IncrementIndex(tokeniser);
		}
		else if (gf_GetChar(tokeniser) == '/') {

			gf_IncrementIndex(tokeniser);

			if (gf_GetChar(tokeniser) == '*') {

				int nestedCommentDepth = 1;
				gf_IncrementIndex(tokeniser);

				while (nestedCommentDepth > 0) {

					if (gf_GetChar(tokeniser) == '\0') {
						GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Comment does not end before the file ends");
						return 0;
					}
					else if (gf_GetChar(tokeniser) == '/') {

						gf_IncrementIndex(tokeniser);
						if (gf_GetChar(tokeniser) == '*') {
							nestedCommentDepth++;
						}
					}
					else if (gf_GetChar(tokeniser) == '*') {
						gf_IncrementIndex(tokeniser);
						if (gf_GetChar(tokeniser) == '/') {
							nestedCommentDepth--;
						}
					}
					else if (gf_GetChar(tokeniser) == '\n') {
						gf_IncrementLineNo(tokeniser);						
						gf_IncrementIndex(tokeniser);
					}
					else if (gf_GetChar(tokeniser) == '\r') {
						gf_IncrementLineNo(tokeniser);
						gf_IncrementIndex(tokeniser);
						if (gf_GetChar(tokeniser) == '\n') {
							gf_IncrementIndex(tokeniser);
						}
					}
					else {
						gf_IncrementIndex(tokeniser);
					}
				}
			}
		}
		else if (gf_GetChar(tokeniser) == ' ' || gf_GetChar(tokeniser) == ',' || gf_GetChar(tokeniser) == '\t') {
			gf_IncrementIndex(tokeniser);
		}
		else if (gf_GetChar(tokeniser) == '\n') {
			gf_IncrementLineNo(tokeniser);
			gf_IncrementIndex(tokeniser);
		}
		else if (gf_GetChar(tokeniser) == '\r') {
			gf_IncrementLineNo(tokeniser);
			gf_IncrementIndex(tokeniser);
			if (gf_GetChar(tokeniser) == '\n') {
				gf_IncrementIndex(tokeniser);
			}
		}
		else if (gf_GetChar(tokeniser) == '\"') {

			gf_IncrementIndex(tokeniser);

			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_STRING, tokeniser->lineno, tokeniser->colno);
			if (!result) {
				GF_LOG(loader, GF_LOG_ERROR, "Failed to add string token");
				return 0;
			}

			if (gf_GetChar(tokeniser) != '\"') {
				int lastCharPossibleEscapeChar = 0;
				gf_IncrementIndex(tokeniser);
				while (1) {

					if (gf_GetChar(tokeniser) == '\0') {
						GF_LOG(loader, GF_LOG_ERROR, "String does not end before the file ends");
						return 0;
					}
					else if (gf_GetChar(tokeniser) == '\"' && !lastCharPossibleEscapeChar) {
						break;
					}
					else if (gf_GetChar(tokeniser) == '\\') {
						lastCharPossibleEscapeChar = 1;
					}
					else {
						lastCharPossibleEscapeChar = 0;
					}

					gf_IncrementLastTokenLength(loader);
					gf_IncrementIndex(tokeniser);
				}
			}

			gf_IncrementIndex(tokeniser);
		}
		else if (isalpha(gf_GetChar(tokeniser))) {

			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_NAME, tokeniser->lineno, tokeniser->colno);
			if (!result) {
				GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Failed to add name token");
				return 0;
			}
			gf_IncrementIndex(tokeniser);

			while (isalpha(gf_GetChar(tokeniser)) || isdigit(gf_GetChar(tokeniser)) || gf_GetChar(tokeniser) == '_') {
				gf_IncrementLastTokenLength(loader);
				gf_IncrementIndex(tokeniser);
			}
		}
		else if (isdigit(gf_GetChar(tokeniser)) || gf_GetChar(tokeniser) == '-' || gf_GetChar(tokeniser) == '+') {

			int hasFloatingPoint = 0;
			int hasPlusOrMinus = gf_GetChar(tokeniser) == '-' || gf_GetChar(tokeniser) == '+';

			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_FLOAT, tokeniser->lineno, tokeniser->colno);
			if (!result) {
				GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Failed to add float token");
				return 0;
			}
			gf_IncrementIndex(tokeniser);

			while (isdigit(gf_GetChar(tokeniser)) || gf_GetChar(tokeniser) == '.') {
				if (gf_GetChar(tokeniser) == '.') {

					if (hasFloatingPoint == 0) {
						hasFloatingPoint = 1;
					}

					gf_IncrementLastTokenLength(loader);
					gf_IncrementIndex(tokeniser);
					while (isdigit(gf_GetChar(tokeniser))) {
						gf_IncrementLastTokenLength(loader);
						gf_IncrementIndex(tokeniser);
					}
					break;
				}
				else {
					gf_IncrementLastTokenLength(loader);
					gf_IncrementIndex(tokeniser);
				}
			}

			if (!hasFloatingPoint) {
				loader->lastToken->type = GF_TOKEN_TYPE_INTEGER;
			}
			// Prevents just having a + or - as a valid number
			if (hasPlusOrMinus && loader->lastToken->length == 1) {
				GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "There is a + or - without a number after it.");
				return 0;
			}
		}
		else {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, loader->lastToken, "Unrecognised character %c", gf_GetChar(tokeniser));
			return 0;
		}
	}

	return 1;
}

int gf_Tokenise(gf_Loader *loader, const char *buffer, gf_u64 count) {
	assert(loader);
	assert(buffer);

	gf_Tokeniser tokeniser;
	gf_InitTokeniser(&tokeniser, buffer, count);

	return gf_TokeniseInternal(loader, &tokeniser);
}

gf_Token *gf_ConsumeToken(gf_Loader *loader) {
	gf_Token *token = loader->curToken;
	if (!token) {
		return NULL;
	}
	loader->curToken = loader->curToken->next;
	return token;
}

gf_Token *gf_PeekToken(gf_Loader *loader) {
	return loader->curToken;
}

gf_LoaderNode *gf_AddNode(gf_Loader *loader, gf_Token *token) {
	gf_LoaderNode *node = (gf_LoaderNode *)loader->Allocate(sizeof(gf_LoaderNode));
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "Out of memory. Failed to allocate loader node");
		return NULL;
	}

	if (loader->lastNode) {
		loader->lastNode->nextAllocated = node;
	}
	loader->lastNode = node;

	node->token = token;
	node->parent = NULL;
	node->next = NULL;
	node->prev = NULL;
	node->childrenHead = NULL;
	node->childrenTail = NULL;
	node->nextAllocated = NULL;
	return node;
}

void gf_AddChild(gf_LoaderNode *parent, gf_LoaderNode *child) {
	if (parent == NULL) return;
	if (parent->childrenHead == NULL) {
		parent->childrenHead = child;
		parent->childrenTail = child;
		child->next = NULL;
		child->prev = NULL;
	}
	else {
		parent->childrenTail->next = child;
		child->prev = parent->childrenTail;
		child->next = NULL;
		parent->childrenTail = child;
	}
	child->parent = parent;
}

int gf_Parse(gf_Loader *loader, gf_LoaderNode *parentNode) {
	assert(loader);
	assert(loader->curToken);
	assert(loader->lastToken);
	assert(parentNode);

	int result = 0;
	gf_Token *token = NULL;
	while ((token = gf_ConsumeToken(loader))) {

		if (token->type == GF_TOKEN_TYPE_NAME) {

			if (parentNode) {
				parentNode->token->type = GF_TOKEN_TYPE_COMPOSITE_TYPE;
			}

			gf_Token *peek = gf_PeekToken(loader);
			if (!peek) {
				return 0;
			}

			if (peek->type == GF_TOKEN_TYPE_VALUE_ASSIGN) {

				gf_LoaderNode *node = gf_AddNode(loader, token);
				if (!node) {
					return 0;
				}

				gf_AddChild(parentNode, node);
				gf_ConsumeToken(loader);

				loader->nestLevel++;
				result = gf_Parse(loader, node);
				if (!result) {
					return 0;
				}
			}
			else {
				gf_LoaderNode *node = gf_AddNode(loader, token);
				if (!node) {
					return 0;
				}

				gf_AddChild(parentNode, node);
			}
		}
		else if (token->type == GF_TOKEN_TYPE_STRING || token->type == GF_TOKEN_TYPE_FLOAT || token->type == GF_TOKEN_TYPE_INTEGER) {

			gf_LoaderNode *node = gf_AddNode(loader, token);
			if (!node) {
				return 0;
			}

			gf_AddChild(parentNode, node);
		}
		else if (token->type == GF_TOKEN_TYPE_CURLY_CLOSE) {
			loader->nestLevel--;
			break;
		}
		else if (token->type == GF_TOKEN_TYPE_END_FILE) {
			break;
		}
		else if (token->type == GF_TOKEN_TYPE_VALUE_ASSIGN)  {
			GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, token, "unexpected value assign at token. It is likely because the token before it is not an identifier node.");
			return 0;
		}
	}

	return 1;
}


int gf_LoadInternal(gf_Loader *loader, const char *buffer, gf_u64 bufferCount) {
	assert(loader);
	assert(buffer);

	int result = gf_Tokenise(loader, buffer, bufferCount);
	if (!result) {
		GF_LOG(loader, GF_LOG_ERROR, "Failed to tokenise");
		return 0;
	}

	loader->rootNode = gf_AddNode(loader, &loader->rootToken);
	if (!loader->rootNode) {
		return 0;
	}
	loader->curToken = &loader->rootToken;

	loader->nestLevel = 0;
	result = gf_Parse(loader, loader->rootNode);

	if (loader->nestLevel != 0) {
		GF_LOG(loader, GF_LOG_ERROR, "There is a missing closing brace }. A brace has been opened { without a matching close.");
		result = 0;
	}

	if (!result) {
		GF_LOG(loader, GF_LOG_ERROR, "Failed to parse");
		return 0;
	}

	return 1;
}

int gf_LoadFromBuffer(gf_Loader *loader, const char *buffer, gf_u64 bufferCount, gf_LogAllocateFreeFunctions *funcs) {
	assert(loader);
	assert(buffer);

	gf_InitLoader(loader, funcs);

	return gf_LoadInternal(loader, buffer, bufferCount);
}

int gf_LoadFromFile(gf_Loader *loader, const char *filename, gf_LogAllocateFreeFunctions *funcs) {
	assert(loader);
	assert(filename);

	gf_InitLoader(loader, funcs);

	gf_u64 bufferCount = 0;
	loader->fileContentsBuffer = gf_AllocateNullTerminatedBufferFromFile(loader, filename, &bufferCount);
	if (!loader->fileContentsBuffer) {
		return 0;
	}

	return gf_LoadInternal(loader, loader->fileContentsBuffer, bufferCount);
}

void gf_Unload(gf_Loader *loader) {
	assert(loader);

	gf_Token *token = loader->firstToken;
	gf_Token *nextToken;
	while (token) {
		nextToken = token->next;
		loader->Free(token);
		token = nextToken;
	}

	gf_LoaderNode *node = loader->rootNode;
	gf_LoaderNode *nextNode;
	while (node) {
		nextNode = node->nextAllocated;
		loader->Free(node);
		node = nextNode;
	}

	loader->Free(loader->fileContentsBuffer);
	loader->fileContentsBuffer = NULL;
}

int gf_LoaderNodeToU32(gf_Loader *loader, gf_LoaderNode *node, gf_u32 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null in gf_LoaderNodeToU32");
		return 0;
	}
	if (node->token->type != GF_TOKEN_TYPE_INTEGER) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "node type is not integer");
		return 0;
	}
	if (!gf_StringSpanToU32(node->token->start, node->token->length, value)) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "unable to convert token to u32 in gf_LoaderNodeToU32");
		return 0;
	}
	return 1;
}

int gf_LoaderNodeToU64(gf_Loader *loader, gf_LoaderNode *node, gf_u64 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null in gf_LoaderNodeToU64");
		return 0;
	}
	if (node->token->type != GF_TOKEN_TYPE_INTEGER) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "node type is not integer");
		return 0;
	}
	if (!gf_StringSpanToU64(node->token->start, node->token->length, value)) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "unable to convert token to u64 in gf_LoaderNodeToU64");
		return 0;
	}
	return 1;
}

int gf_LoaderNodeToS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null in gf_LoaderNodeToS32");
		return 0;
	}
	if (node->token->type != GF_TOKEN_TYPE_INTEGER) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "node type is not integer");
		return 0;
	}
	if (!gf_StringSpanToS32(node->token->start, node->token->length, value)) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "unable to convert token to S64 in gf_LoaderNodeToS32");
		return 0;
	}
	return 1;
}


int gf_LoaderNodeToS64(gf_Loader *loader, gf_LoaderNode *node, gf_s64 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null in gf_LoaderNodeToS64");
		return 0;
	}
	if (node->token->type != GF_TOKEN_TYPE_INTEGER) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "node type is not integer");
		return 0;
	}
	if (!gf_StringSpanToS64(node->token->start, node->token->length, value)) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "unable to convert token to S64 in gf_LoaderNodeToS64");
		return 0;
	}
	return 1;
}

int gf_LoaderNodeToF64(gf_Loader *loader, gf_LoaderNode *node, gf_f64 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null in gf_LoaderNodeToF64");
		return 0;
	}
	if (node->token->type != GF_TOKEN_TYPE_FLOAT) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "node type is not float");
		return 0;
	}
	if (!gf_StringSpanToF64(node->token->start, node->token->length, value)) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "unable to convert token to S64 in gf_LoaderNodeToF64");
		return 0;
	}
	return 1;
}

int gf_LoaderNodeToF32(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null in gf_LoaderNodeToF32");
		return 0;
	}
	if (node->token->type != GF_TOKEN_TYPE_FLOAT) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "node type is not float");
		return 0;
	}
	if (!gf_StringSpanToF32(node->token->start, node->token->length, value)) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "unable to convert token to S64 in gf_LoaderNodeToF32");
		return 0;
	}
	return 1;
}

int gf_LoaderNodeToString(gf_Loader *loader, gf_LoaderNode *node, char *src, gf_u64 srcCapacityIncludesNullTerminator) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null in gf_LoaderNodeToString");
		return 0;
	}
	if (node->token->type != GF_TOKEN_TYPE_STRING) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "node type is not a string");
		return 0;
	}
	if (node->token->length >= srcCapacityIncludesNullTerminator) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "unable to convert token to string in gf_LoaderNodeToString");
		return 0;
	}
	memcpy(src, node->token->start, node->token->length);
	src[node->token->length] = '\0';

	return 1;
}


gf_LoaderNode *gf_GetRoot(gf_Loader *loader) {
	assert(loader);

	if (!loader->rootNode) {
		GF_LOG(loader, GF_LOG_WARNING, "loader has no root node");
		return NULL;
	}
	return loader->rootNode;
}

gf_LoaderNode *gf_GetNext(gf_Loader *loader, gf_LoaderNode *node) {
	assert(loader);

	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return NULL;
	}
	else if (!node->next) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_WARNING, node->token, "node's next is null");
		return NULL;
	}
	else {
		return node->next;
	}
}

gf_LoaderNode *gf_GetChild(gf_Loader *loader, gf_LoaderNode *node) {
	assert(loader);

	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return NULL;
	}
	else if (!node->childrenHead) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_WARNING, node->token, "node's child is null");
		return NULL;
	}
	else {
		return node->childrenHead;
	}
}

gf_TokenType gf_GetType(gf_Loader *loader, gf_LoaderNode *node) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return GF_TOKEN_TYPE_END_FILE;
	}
	return node->token->type;
}

gf_LoaderNode *gf_FindFirstChild(gf_Loader *loader, gf_LoaderNode *node, const char *str) {
	// TODO: change to hash map?
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return NULL;
	}

	gf_u64 len = gf_StringLength(str);

	gf_LoaderNode *child = node->childrenHead;
	while (child) {
		if (child->token->type == GF_TOKEN_TYPE_COMPOSITE_TYPE || child->token->type == GF_TOKEN_TYPE_NAME) {
			if (gf_AreStringSpansEqual(child->token->start, child->token->length, str, len)) {
				return child;
			}
		}
		child = child->next;
	}

	return NULL;
}

gf_LoaderNode *gf_FindFirstNext(gf_Loader *loader, gf_LoaderNode *node, const char *str) {
	// TODO: change to hash map?
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return NULL;
	}

	gf_u64 len = gf_StringLength(str);

	gf_LoaderNode *next = node->next;
	while (next) {
		if (next->token->type == GF_TOKEN_TYPE_COMPOSITE_TYPE || next->token->type == GF_TOKEN_TYPE_NAME) {
			if (gf_AreStringSpansEqual(next->token->start, next->token->length, str, len)) {
				return next;
			}
		}
		next = next->next;
	}

	return NULL;
}

int gf_LoadVariableU32(gf_Loader *loader, gf_LoaderNode *node, gf_u32 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *child = gf_GetChild(loader, node);
	if (!child) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}
	return gf_LoaderNodeToU32(loader, child, value);
}


int gf_LoadVariableU64(gf_Loader *loader, gf_LoaderNode *node, gf_u64 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *child = gf_GetChild(loader, node);
	if (!child) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}
	return gf_LoaderNodeToU64(loader, child, value);
}

int gf_LoadVariableS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *child = gf_GetChild(loader, node);
	if (!child) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}
	return gf_LoaderNodeToS32(loader, child, value);
}

int gf_LoadVariableS64(gf_Loader *loader, gf_LoaderNode *node, gf_s64 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *child = gf_GetChild(loader, node);
	if (!child) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}
	return gf_LoaderNodeToS64(loader, child, value);
}

int gf_LoadVariableF64(gf_Loader *loader, gf_LoaderNode *node, gf_f64 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *child = gf_GetChild(loader, node);
	if (!child) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}
	return gf_LoaderNodeToF64(loader, child, value);
}


int gf_LoadVariableF32(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *value) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *child = gf_GetChild(loader, node);
	if (!child) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}
	return gf_LoaderNodeToF32(loader, child, value);
}

int gf_LoadVariableVec3(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *x, gf_f32 *y, gf_f32 *z) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *nodeX = gf_GetChild(loader, node);
	if (!nodeX) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}

	gf_LoaderNode *nodeY = gf_GetNext(loader, nodeX);
	if (!nodeY) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, nodeX->token, "y value is null");
		return 0;
	}

	gf_LoaderNode *nodeZ = gf_GetNext(loader, nodeY);
	if (!nodeZ) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, nodeY->token, "z value is null");
		return 0;
	}

	if (!gf_LoaderNodeToF32(loader, nodeX, x)) {
		return 0;
	}
	if (!gf_LoaderNodeToF32(loader, nodeY, y)) {
		return 0;
	}
	if (!gf_LoaderNodeToF32(loader, nodeZ, z)) {
		return 0;
	}

	return 1;
}

int gf_LoadVariableString(gf_Loader *loader, gf_LoaderNode *node, char *str, gf_u64 lenWithNullTerminator) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *child = gf_GetChild(loader, node);
	if (!child) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}

	gf_u64 index = 0;
	for (index = 0; index < lenWithNullTerminator - 1 && index < child->token->length; index++) {
		str[index] = child->token->start[index];
	}
	str[index] = '\0';

	return 1;
}

int gf_LoadArrayS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value, gf_u64 count) {
	if (!node) {
		GF_LOG(loader, GF_LOG_ERROR, "node is null");
		return 0;
	}

	gf_LoaderNode *child = gf_GetChild(loader, node);
	if (!child) {
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "child is null");
		return 0;
	}

	gf_u64 index = 0;
	while (child && index < count) {
		if (!gf_LoaderNodeToS32(loader, child, &value[index])) {
			return 0;
		}
		child = child->next;
		index++;
	}

	return 1;
}

/*-----------------------------------------------------------------------------------*/

#endif

#ifdef GF_IMPLEMENTATION_WITH_TESTS

/*-------------------------------------TESTS-----------------------------------------*/

int gf_Test(void) {

	{
		const char *str = "a {";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "{{{{{{{{{{{{{}}";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "{{{{{{{{{{{{{a}}}}}}}}}}}}}";
		gf_Loader loader;
		int result = gf_LoadFromBuffer(&loader, str, gf_StringLength(str), NULL);
		GF_TEST_ASSERT(result == 0, str);
		gf_Unload(&loader);
	}
	{
		const char *str = "   ";
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

/*-----------------------------------------------------------------------------------*/

#endif

#endif
