#ifndef QLIGHT_STRING_COMMON_H
#define QLIGHT_STRING_COMMON_H

#include "common.h"

#define StringFormat "%.*s"

#define StringArgumentValue(string) string.size, string.data
#define StringArgumentPointer(string) string->size, string->data

struct UnicodeChar {
	s32 size_in_bytes;
	char8_t bytes[ 4 ];
};

#endif /* QLIGHT_STRING_COMMON_H */