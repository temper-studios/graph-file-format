#pragma once
typedef uint64_t graph_u64;
typedef int64_t graph_s64;

bool graph_utility_strings_equal(const char *a, graph_u64 alength, const char *b, graph_u64 blength);
graph_u64 graph_utility_string_length(const char *str);

// TODO: change these functions so they work on string spans. they work in the context of our tokens
// because our tokens end with delimiters. not the case for regularly spanning strings.
bool graph_utility_spanning_str_to_float(const char *start, graph_u64 length, float *value);
bool graph_utility_spanning_str_to_double(const char *start, graph_u64 length, double *value);
bool graph_utility_spanning_str_to_u64(const char *start, graph_u64 length, graph_u64 *value);
bool graph_utility_spanning_str_to_s64(const char *start, graph_u64 length, graph_s64 *value);