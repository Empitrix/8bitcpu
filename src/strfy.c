#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* str_insert: insert 'm' into 'src' that starts at given 'idx' */
void str_insert(char *src, char *m, int idx) {
	int len1 = strlen(src);
	int len2 = strlen(m);
	memmove(src + idx + len2, src + idx, len1 - idx + 1);
	memcpy(src + idx, m, len2);
}

/* str_replace: replaces all of the 'a' with 'b' in the given 'src' */
void str_replace(char *src, char *a, char *b) {
	int len_a = strlen(a);
	int len_b = strlen(b);

	char *p = src;

	while ((p = strstr(p, a)) != NULL) {
		int offset = p - src;
		memmove(p + len_b, p + len_a, strlen(p + len_a) + 1);
		memcpy(p, b, len_b);
		p += len_b;
	}
}


void shift_over(char *src, int shift, int size){
	for(int j = 0; j < size; ++j){
		src[j] = src[shift + j];
	}
}


char *str_slice(char *src, int start, int end) {
	int length = end - start;
	char *result = (char *)malloc(length + 1); // Allocate memory for the new string
	strncpy(result, src + start, length);
	result[length] = '\0'; // Null-terminate the string
	return result;
}

void str_lower(char *src){
	int len, i; len = (int)strlen(src);
	for(i = 0; i < len; ++i){
		src[i] = tolower(src[i]);
	}
}
