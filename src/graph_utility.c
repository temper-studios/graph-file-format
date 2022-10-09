bool graph_utility_strings_equal(const char *a, graph_u64 alength, const char *b, graph_u64 blength) {
	if (alength != blength) return false;
	graph_u64 i = 0;
	while (i < alength) {
		if (a[i] == '\0' && b[i] != '\0') return false;
		if (a[i] != '\0' && b[i] == '\0') return false;
		if (a[i] != b[i]) return false;
		i++;
	}
	return true;
}

graph_u64 graph_utility_string_length(const char *str) {
	assert(str);
	graph_u64 len = 0;
	while (str[len] != '\0') len++;
	return len;
}

bool graph_utility_spanning_str_to_float(const char *start, graph_u64 length, float *value) {
	assert(value);
	assert(start);
	float f = strtof(start, NULL);
	// 0.0 could mean a failure, or it could just mean the string is actually 0. some fantastic api design there
	if (f == 0.0) {
		graph_u64 index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return false;
		}
	}
	else if (f == HUGE_VALF || f == -HUGE_VALF) {
		return false;
	}
	*value = f;
	return true;
}

bool graph_utility_spanning_str_to_double(const char *start, graph_u64 length, double *value) {
	assert(value);
	assert(start);
	double d = strtod(start, NULL);
	// 0.0 could mean a failure, or it could just mean the string is actually 0. some fantastic api design there
	if (d == 0.0) {
		graph_u64 index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return false;
		}
	}
	else if (d == HUGE_VAL || d == -HUGE_VAL) {
		return false;
	}
	*value = d;
	return true;
}

bool graph_utility_spanning_str_to_u64(const char *start, graph_u64 length, graph_u64 *value) {
	assert(value);
	assert(start);

	if (*start == '-') {
		return false;
	}

	graph_u64 i = strtoull(start, NULL, 10);
	// 0 could mean a failure, or it could just mean the string is actually 0. some fantastic api design there
	if (i == 0) {
		graph_u64 index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return false;
		}
	}
	if (i == ULLONG_MAX) {
		return false;
	}
	*value = i;
	return true;
}

bool graph_utility_spanning_str_to_s64(const char *start, graph_u64 length, graph_s64 *value) {
	assert(value);
	assert(start);

	graph_s64 i = strtoll(start, NULL, 10);
	// 0 could mean a failure, or it could just mean the string is actually 0. some fantastic api design there
	if (i == 0) {
		graph_u64 index = 0;
		while (!isdigit(start[index]) && index < length) {
			index++;
		}
		if (start[index] != '0') {
			return false;
		}
	}
	if (i == LLONG_MIN || i == LLONG_MAX) {
		return false;
	}
	*value = i;
	return true;
}