#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>


char* strconcat(char* dst, const char* append, size_t dstLen, size_t appendLen) {
	void *dstNew = realloc(dst, dstLen + appendLen + 1);
	if (dstNew == NULL) {
		fprintf(stderr, "Couldn't resize [%.20s] from %lu to %lu characters", dst, dstLen, dstLen+appendLen);
		return NULL;
	}
	strncpy(dstNew+dstLen, append, appendLen);
	*((char*)dstNew+dstLen+appendLen+1) = '\0';
	return dstNew;
}