#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <memory.h>
#include <inttypes.h>

//----------------------------------TYPEDEFS----------------------------------------\\

typedef uint32_t gf_u32;
typedef uint64_t gf_u64;
typedef int32_t  gf_s32;
typedef int64_t  gf_s64;
typedef float    gf_f32;
typedef double   gf_f64;

//----------------------------------------------------------------------------------\\

//-------------------------STRING CONVERSIONS---------------------------------------\\

int gf_AreStringSpansEqual(const char *a, gf_u64 alength, const char *b, gf_u64 blength);
gf_u64 gf_StringLength(const char *str);

int gf_StringSpanToU32(const char *start, uint64_t length, gf_u32 *value);
int gf_StringSpanToU64(const char *start, uint64_t length, gf_u64 *value);
int gf_StringSpanToS32(const char *start, uint64_t length, gf_s32 *value);
int gf_StringSpanToS64(const char *start, uint64_t length, gf_s64 *value);
int gf_StringSpanToF64(const char *start, uint64_t length, gf_f64 *value);
int gf_StringSpanToF32(const char *start, uint64_t length, gf_f32 *value);

//-----------------------------------------------------------------------------------\\

//-------------------------------------TOKENS----------------------------------------\\

typedef enum gf_TokenType {
	GF_TOKEN_TYPE_ROOT,
	GF_TOKEN_TYPE_NAME,
	GF_TOKEN_TYPE_CURLY_CLOSE,
	GF_TOKEN_TYPE_COMMENT,
	GF_TOKEN_TYPE_COMPOSITE_TYPE,
	GF_TOKEN_TYPE_STRING,
	GF_TOKEN_TYPE_FLOAT,
	GF_TOKEN_TYPE_INTEGER,
	GF_TOKEN_TYPE_END_FILE,
	GF_TOKEN_TYPE_VALUE_ASSIGN
} gf_TokenType;

typedef struct gf_Token {
	const char *start;
	gf_u64 length;
	gf_TokenType type;
	gf_u64 lineno;
	struct gf_Token *next;
} gf_Token;

void gf_PrintToken(gf_Token *token);
void gf_PrintLineToken(gf_Token *token);
const char *gf_TokenTypeToString(gf_TokenType type);

//-----------------------------------------------------------------------------------\\

//-------------------------LOG AND FUNCTION POINTERS--------------------------\\

typedef enum gf_LogLevel {
	GF_LOG_ERROR = 0,
	GF_LOG_WARNING,
	GF_LOG_INFO
} gf_LogLevel;

typedef void (*gf_LogFunctionPtr)(gf_LogLevel, int, gf_Token *, const char *, va_list);
typedef void *(*gf_AllocatorFunctionPtr)(size_t);
typedef void (*gf_FreeFunctionPtr)(void *);

typedef struct gf_LogAllocateFreeFunctions {
	gf_LogFunctionPtr Log;
	gf_AllocatorFunctionPtr Allocate;
	gf_FreeFunctionPtr Free;
} gf_LogAllocateFreeFunctions;

void gf_DefaultLog(gf_LogLevel level, int lineno, gf_Token *, const char *format, va_list vlist);
void gf_Log(gf_LogFunctionPtr Log, gf_LogLevel level, int lineno, gf_Token *, const char *format, ...);

#define GF_LOG(loaderOrSaver, level, ...) gf_Log(loaderOrSaver->Log, level, __LINE__, NULL, __VA_ARGS__)
#define GF_LOG_WITH_TOKEN(loaderOrSaver, level, token, ...) gf_Log(loaderOrSaver->Log, level, __LINE__, token, __VA_ARGS__)

//-----------------------------------------------------------------------------------\\

//--------------------------------------SAVER----------------------------------------\\

typedef struct gf_Saver {
	gf_LogFunctionPtr Log;
	unsigned int indent;
} gf_Saver;

void gf_InitSaver(gf_Saver *saver, gf_LogFunctionPtr logfunction);
int  gf_PrintIndent(gf_Saver *saver, FILE *file);
int  gf_SaveVariableS64(gf_Saver *saver, FILE *file, const char *identifier, gf_s64 *value);
int  gf_SaveVariableS32(gf_Saver *saver, FILE *file, const char *identifier, gf_s32 *value);
int  gf_SaveVariableU32(gf_Saver *saver, FILE *file, const char *identifier, gf_u32 *value);
int  gf_SaveVariableU64(gf_Saver *saver, FILE *file, const char *identifier, gf_u64 *value);
int  gf_SaveVariableString(gf_Saver *saver, FILE *file, const char *identifier, const char *str);
int  gf_SaveVariableF32(gf_Saver *saver, FILE *file, const char *identifier, gf_f32 *value);
int  gf_SaveVariableF64(gf_Saver *saver, FILE *file, const char *identifier, gf_f64 *value);
int  gf_SaveVariableStringSpan(gf_Saver *saver, FILE *file, const char *identifier, const char *str, int strLen);
int  gf_SaveVariableVec3(gf_Saver *saver, FILE *file, const char *identifier, gf_f32 *x, gf_f32 *y, gf_f32 *z);
int  gf_SaveArrayU64(gf_Saver *saver, FILE *file, const char *identifier, gf_u64 *value, int count);
int  gf_SaveArrayS64(gf_Saver *saver, FILE *file, const char *identifier, gf_s64 *value, int count);
int  gf_SaveArrayS32(gf_Saver *saver, FILE *file, const char *identifier, gf_s32 *value, int count);
int  gf_SaveStartList(gf_Saver *saver, FILE *file, const char *identifier);
int  gf_SaveEndList(gf_Saver *saver, FILE *file);

//-----------------------------------------------------------------------------------\\

//------------------------------------LOADER-----------------------------------------\\

typedef struct gf_Tokeniser {
	const char *buffer;
	gf_u64 index;
	gf_u64 count;
} gf_Tokeniser;

void gf_InitTokeniser(gf_Tokeniser *tokeniser, const char *buffer, gf_u64 count);
char gf_GetChar(gf_Tokeniser *tokeniser);
void gf_IncrementIndex(gf_Tokeniser *tokeniser);
const char *gf_Ptr(gf_Tokeniser *tokeniser);

typedef struct gf_LoaderNode {
	gf_Token *token;
	struct gf_LoaderNode *parent;
	struct gf_LoaderNode *next;
	struct gf_LoaderNode *prev;
	struct gf_LoaderNode *childrenHead;
	struct gf_LoaderNode *childrenTail;
	struct gf_LoaderNode *nextAllocated;
} gf_LoaderNode;

typedef struct gf_Loader {
	gf_LogFunctionPtr Log;
	gf_AllocatorFunctionPtr Allocate;
	gf_FreeFunctionPtr Free;
	gf_Token rootToken;
	gf_Token *lastToken;
	gf_Token *firstToken;
	gf_Token *curToken;
	gf_LoaderNode *rootNode;
	gf_LoaderNode *lastNode;
} gf_Loader;

void gf_InitLoader(gf_Loader *loader, gf_LogAllocateFreeFunctions *helperfunctions);
void gf_IncrementLastTokenLength(gf_Loader *loader);
int gf_AddToken(gf_Loader *loader, const char *start, gf_TokenType type, gf_u64 lineno);
char *gf_AllocateNullTerminatedBufferFromFile(gf_Loader *loader, const char *filename);
int gf_TokeniseInternal(gf_Loader *loader, gf_Tokeniser *tokeniser);
int gf_Tokenise(gf_Loader *loader, const char *buffer, gf_u64 count);
gf_Token *gf_ConsumeToken(gf_Loader *loader);
gf_Token *gf_PeekToken(gf_Loader *loader);
gf_LoaderNode *gf_AddNode(gf_Loader *loader, gf_Token *token);
void gf_AddChild(gf_LoaderNode *parent, gf_LoaderNode *child);
int gf_Parse(gf_Loader *loader, gf_LoaderNode *parentNode);
int gf_LoadInternal(gf_Loader *loader, const char *buffer, gf_u64 bufferCount);
int gf_LoadFromBuffer(gf_Loader *loader, const char *buffer, gf_u64 bufferCount, gf_LogAllocateFreeFunctions *funcs);
int gf_LoadFromFile(gf_Loader *loader, const char *filename, gf_LogAllocateFreeFunctions *funcs);
void gf_Unload(gf_Loader *loader);
int gf_LoaderNodeToU32(gf_Loader *loader, gf_LoaderNode *node, gf_u32 *value);
int gf_LoaderNodeToU64(gf_Loader *loader, gf_LoaderNode *node, gf_u64 *value);
int gf_LoaderNodeToS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value);
int gf_LoaderNodeToS64(gf_Loader *loader, gf_LoaderNode *node, gf_s64 *value);
int gf_LoaderNodeToF64(gf_Loader *loader, gf_LoaderNode *node, gf_f64 *value);
int gf_LoaderNodeToF32(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *value);
int gf_LoaderNodeToString(gf_Loader *loader, gf_LoaderNode *node, char *src, gf_u64 srcCapacityIncludesNullTerminator);
gf_LoaderNode *gf_GetRoot(gf_Loader *loader);
gf_LoaderNode *gf_GetNext(gf_Loader *loader, gf_LoaderNode *node);
gf_LoaderNode *gf_GetChild(gf_Loader *loader, gf_LoaderNode *node);
gf_TokenType gf_GetType(gf_Loader *loader, gf_LoaderNode *node);
gf_LoaderNode *gf_FindFirstChild(gf_Loader *loader, gf_LoaderNode *node, const char *str);
gf_LoaderNode *gf_FindFirstNext(gf_Loader *loader, gf_LoaderNode *node, const char *str);
int gf_LoadVariableU32(gf_Loader *loader, gf_LoaderNode *node, gf_u32 *value);
int gf_LoadVariableU64(gf_Loader *loader, gf_LoaderNode *node, gf_u64 *value);
int gf_LoadVariableS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value);
int gf_LoadVariableS64(gf_Loader *loader, gf_LoaderNode *node, gf_s64 *value);
int gf_LoadVariableF64(gf_Loader *loader, gf_LoaderNode *node, gf_f64 *value);
int gf_LoadVariableF32(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *value);
int gf_LoadVariableVec3(gf_Loader *loader, gf_LoaderNode *node, gf_f32 *x, gf_f32 *y, gf_f32 *z);
int gf_LoadVariableString(gf_Loader *loader, gf_LoaderNode *node, char *str, gf_u64 lenWithNullTerminator);
int gf_LoadArrayS32(gf_Loader *loader, gf_LoaderNode *node, gf_s32 *value, gf_u64 count);


//-----------------------------------------------------------------------------------\\

// IMPLEMENTATION
#ifdef GF_IMPLEMENTATION

//-------------------------STRING CONVERSIONS---------------------------------------\\

int gf_AreStringSpansEqual(const char *a, gf_u64 alength, const char *b, gf_u64 blength) {
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

	gf_s32 i = strtol(start, NULL, 10);
	// 0 could mean a failure, or it could just mean the string is actually 0. some fantastic api design there
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

	gf_s64 i = strtoll(start, NULL, 10);
	// 0 could mean a failure, or it could just mean the string is actually 0. some fantastic api design there
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

	double d = strtod(start, NULL);
	// 0.0 could mean a failure, or it could just mean the string is actually 0. some fantastic api design there
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
	float f = strtof(start, NULL);
	// 0.0 could mean a failure, or it could just mean the string is actually 0. some fantastic api design there
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

//-----------------------------------------------------------------------------------\\

//-------------------------------------TOKENS----------------------------------------\\

void gf_PrintToken(gf_Token *token) {
	for (gf_u64 i = 0; i < token->length; i++) {
		printf("%c", token->start[i]);
	}
}

void gf_PrintLineToken(gf_Token *token) {
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

//-----------------------------------------------------------------------------------\\

//-------------------------------------LOGGING----------------------------------------\\

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

//-----------------------------------------------------------------------------------\\

//-------------------------------------SAVER----------------------------------------\\

void gf_InitSaver(gf_Saver *saver, gf_LogFunctionPtr logfunction) {
	saver->indent = 0;
	if (logfunction == NULL) {
		saver->Log = gf_DefaultLog;
	}
}

int gf_PrintIndent(gf_Saver *saver, FILE *file) {
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

//-----------------------------------------------------------------------------------\\

//-------------------------------------LOADER----------------------------------------\\

// if the function pointer is null the default logging, malloc and free functions will be used respectivel.
// The allocate function must return pointers that are not invalidated with subsequent allocations
// the free function must allow null pointers
void gf_InitLoader(gf_Loader *loader, gf_LogAllocateFreeFunctions *helperfunctions) {
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
}

void gf_IncrementLastTokenLength(gf_Loader *loader) {
	assert(loader->lastToken);
	loader->lastToken->length++;
}

int gf_AddToken(gf_Loader *loader, const char *start, gf_TokenType type, gf_u64 lineno) {
	assert(loader->lastToken);

	if (!start) {
		GF_LOG(loader, GF_LOG_ERROR, "Expecting a token but at the end of the file/buffer");
		return 0;
	}

	gf_Token *token = (gf_Token *)loader->Allocate(sizeof(gf_Token));
	if (!token) {
		GF_LOG(loader, GF_LOG_ERROR, "Out of memory in gf_AddToken");
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
	token->next = NULL;

	return 1;
}


// Helper function that opens a file, and allocates a null terminated buffer that stores all its contents.
// gf_Loader must be initialised.
// Returns NULL if this fails. Error is logged
char *gf_AllocateNullTerminatedBufferFromFile(gf_Loader *loader, const char *filename) {

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

	return buffer;
}

void gf_InitTokeniser(gf_Tokeniser *tokeniser, const char *buffer, gf_u64 count) {
	tokeniser->buffer = buffer;
	tokeniser->count = count;
	tokeniser->index = 0;
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
	}
}

const char *gf_Ptr(gf_Tokeniser *tokeniser) {
	if (tokeniser->index < tokeniser->count) {
		return &tokeniser->buffer[tokeniser->index];
	}
	return NULL;
}

int gf_TokeniseInternal(gf_Loader *loader, gf_Tokeniser *tokeniser) {

	uint64_t lineno = 1;
	int result = 0;

	while (1) {

		if (gf_GetChar(tokeniser) == '\0') {

			result = gf_AddToken(loader, "<end token>", GF_TOKEN_TYPE_END_FILE, lineno);
			if (!result) {
				GF_LOG(loader, GF_LOG_ERROR, "Failed to add end file token");
				return 0;
			}
			break;
		}
		else if (gf_GetChar(tokeniser) == '{') {
			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_VALUE_ASSIGN, lineno);
			if (!result) {
				GF_LOG(loader, GF_LOG_ERROR, "Failed to add value assign token");
				return 0;
			}
			gf_IncrementIndex(tokeniser);
		}
		else if (gf_GetChar(tokeniser) == '}') {
			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_CURLY_CLOSE, lineno);
			if (!result) {
				GF_LOG(loader, GF_LOG_ERROR, "Failed to add curly close token");
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
						GF_LOG(loader, GF_LOG_ERROR, "Comment does not end before the file ends");
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
						lineno++;
						gf_IncrementIndex(tokeniser);
					}
					else if (gf_GetChar(tokeniser) == '\r') {
						lineno++;
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
			lineno++;
			gf_IncrementIndex(tokeniser);
		}
		else if (gf_GetChar(tokeniser) == '\r') {
			lineno++;
			gf_IncrementIndex(tokeniser);
			if (gf_GetChar(tokeniser) == '\n') {
				gf_IncrementIndex(tokeniser);
			}
		}
		else if (gf_GetChar(tokeniser) == '\"') {

			gf_IncrementIndex(tokeniser);

			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_STRING, lineno);
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

			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_NAME, lineno);
			if (!result) {
				GF_LOG(loader, GF_LOG_ERROR, "Failed to add name token");
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

			result = gf_AddToken(loader, gf_Ptr(tokeniser), GF_TOKEN_TYPE_FLOAT, lineno);
			if (!result) {
				GF_LOG(loader, GF_LOG_ERROR, "Failed to add float token");
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
				GF_LOG(loader, GF_LOG_ERROR, "There is a + or - without a number");
				return 0;
			}
		}
		else {
			GF_LOG(loader, GF_LOG_ERROR, "Unrecognised character %c", gf_GetChar(tokeniser));
			return 0;
		}
	}

	return 1;
}

// The buffer doesn't have to be null terminated. But make sure the size is accurate
int gf_Tokenise(gf_Loader *loader, const char *buffer, gf_u64 count) {

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

// assumes that loader has valid tokens that aren't NULL
int gf_Parse(gf_Loader *loader, gf_LoaderNode *parentNode) {
	assert(loader->curToken);
	assert(loader->lastToken);

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
			break;
		}
		else if (token->type == GF_TOKEN_TYPE_END_FILE) {
			break;
		}
	}

	return 1;
}


int gf_LoadInternal(gf_Loader *loader, const char *buffer, gf_u64 bufferCount) {

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

	result = gf_Parse(loader, loader->rootNode);
	if (!result) {
		GF_LOG(loader, GF_LOG_ERROR, "Failed to parse");
		return 0;
	}

	return 1;
}

int gf_LoadFromBuffer(gf_Loader *loader, const char *buffer, gf_u64 bufferCount, gf_LogAllocateFreeFunctions *funcs) {
	gf_InitLoader(loader, funcs);

	return gf_LoadInternal(loader, buffer, bufferCount);
}

int gf_LoadFromFile(gf_Loader *loader, const char *filename, gf_LogAllocateFreeFunctions *funcs) {
	gf_InitLoader(loader, funcs);

	char *buffer = gf_AllocateNullTerminatedBufferFromFile(loader, filename);
	if (!buffer) {
		return 0;
	}
	gf_u64 bufferCount = gf_StringLength(buffer) + 1; // +1 for null terminator

	return gf_LoadInternal(loader, buffer, bufferCount);
}


// You can replace this with your own function. All this does is free token and node memory
void gf_Unload(gf_Loader *loader) {
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
		GF_LOG_WITH_TOKEN(loader, GF_LOG_ERROR, node->token, "unable to convert token to S64 in gf_LoaderNodeToString");
		return 0;
	}
	memcpy(src, node->token->start, node->token->length);
	src[node->token->length] = '\0';

	return 1;
}


gf_LoaderNode *gf_GetRoot(gf_Loader *loader) {
	if (!loader->rootNode) {
		GF_LOG(loader, GF_LOG_WARNING, "loader has no root node");
		return NULL;
	}
	return loader->rootNode;
}

gf_LoaderNode *gf_GetNext(gf_Loader *loader, gf_LoaderNode *node) {
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

#endif
