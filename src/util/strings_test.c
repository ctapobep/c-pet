#include "test_util.h"
#include "log.h"
#include "../util/signals.c"
#include "strings.h"

#include "asserts.h"

static void concat_returnsNewPointerToCombinedString() {
	char* str = calloc(1, sizeof(char));
	str = strconcat(str, "hello", 0, 5);
	assertEqualsString("hello", str);
	str = strconcat(str, " world", 5, 6);
	assertEqualsString("hello world", str);
	str = strconcat(str, " world", 11, 6);
	assertEqualsString("hello world world", str);
	free(str);
}

int main(void) {
	logInfo("string util tests");
	register_signals();
	RUN_TEST(concat_returnsNewPointerToCombinedString);
}
