#include "platform.h"
#include <stdio.h>
#include <stdlib.h>

void platform_assert_fail(const char *expression, const char *message, const char *file, long line) {
	const char title[] = "Assertion failed";
	const size_t title_length_minus_1 = sizeof(title) - 1;

	const size_t buffer_size = 2048;
	char text[buffer_size];
	size_t cursor = 0;
	cursor += snprintf(
		text, buffer_size,
		"%s!\n"
		"Expression: %s\n"
		"Message: %s\n"
		"File: %s:%ld\n",
		title, expression, message, file, line
	);

	fwrite(text, sizeof(char), cursor, stderr);
	exit(EXIT_FAILURE); // TODO(nilsoncore): Cause a debug breakpoint instead.
}
