#include <stdlib.h> // MB_CUR_MAX
#include <wchar.h> // mbrlen()

#include "string.h"

String string_new(Allocator *allocator, u32 initial_capacity_in_bytes) {
	String string = static_cast<String>(array_new<char8_t>(allocator, initial_capacity_in_bytes));
	string.size_in_bytes = 0;
	return string;
}

/*
String string_new(Allocator *allocator, const char8_t *c_utf8_string, u32 c_utf8_string_size) {
	if (c_utf8_string_size < 1) {
		// The length is not given, calculate it.
		c_utf8_string_size = strlen(c_utf8_string);
	}

	// NOTE(nilsoncore): Maybe a good idea. Needs real life application experience.
	// const u32 initial_capacity_in_bytes = QL_max2(c_utf8_string_size, ARRAY_RESIZE_MIN_CAPACITY);
	String string = string_new(allocator, c_utf8_string_size);

}
*/

StringView get_string_view(String *string, u32 offset, u32 length) {
	StringView view = { };
	view.data = string->data + offset
	return view;
}

u32 string_add(String *string, char8_t ascii_char) {
	if (string->size_in_bytes + 1 > string->capacity) {
		array_resize(string, string->capacity * 2);
	}

	string->data[string->size_in_bytes] = ascii_char;
	const u32 current_index = string->size_in_bytes;
	string->size += 1;
	string->size_in_bytes += 1;

	return current_index;
}

u32 string_add_multibyte_char(String *string, char8_t *utf8_character_array, u32 character_size_in_bytes) {
	if (string->size_in_bytes + character_size_in_bytes > string->capacity) {
		array_resize(string, string->capacity * 2);
	}

	const u32 utf8_character_index = string->size_in_bytes;
	for (u32 utf8_char_byte_index = 0;
		utf8_char_byte_index < character_size_in_bytes;
		utf8_char_byte_index += 1)
	{
		string->data[string->size_in_bytes] = utf8_character_array[utf8_char_byte_index];
		string->size_in_bytes += 1;
	}

	string->size += 1;

	return utf8_character_index;
}

u32 string_add(String *string, char8_t *utf8_bytes_array, u32 array_size_in_bytes, u32 *utf8_characters_added) {
	// Reset shift state. (Reentrant function variant of `mblen`)
	// https://www.gnu.org/software/libc/manual/html_node/Shift-State.html
	mbstate_t multibyte_shift_state = { };
	// mbrlen(NULL, 0, &multibyte_shift_state);
	u32 array_byte_index = 0;
	u32 chars_added = 0;
	u32 last_added_utf8_character_index = 0;
	while (array_byte_index < array_size_in_bytes) {
		const char *c_utf8_character_pointer = (const char *) &utf8_bytes_array[array_byte_index];
		// MB_CUR_MAX - maximum length of a multibyte character in the current locale.
		// https://www.man7.org/linux/man-pages/man3/MB_CUR_MAX.3.html
		//
		// // Don't care about locale, look up to 4 bytes. (basically just Unicode)
		// const size_t c_utf8_character_max_size = MB_CUR_MAX;
		const size_t c_utf8_character_max_size = QLIGHT_UNICODE_STRING_MAX_BYTES;
		int utf8_char_size_in_bytes = mbrlen(c_utf8_character_pointer, c_utf8_character_max_size, &multibyte_shift_state);
		if (utf8_char_size_in_bytes < 0) {
			AssertMessage(utf8_char_size_in_bytes < 0, "Trying to add an invalid UTF-8 character to string.");
			if (utf8_characters_added)
				*utf8_characters_added = chars_added;

			return last_added_utf8_character_index;
		}

		last_added_utf8_character_index = string_add_multibyte_char(string, &utf8_bytes_array[array_byte_index], utf8_char_size_in_bytes);
		array_byte_index += utf8_char_size_in_bytes;
		chars_added += 1;
	}

	if (utf8_characters_added)
		*utf8_characters_added = chars_added;

	return last_added_utf8_character_index;
}

StringChar string_pop(String *string, bool do_pop) {
	StringChar result = { };

	if (!string) {
		// Return everything zeroed.
		return result;
	}

	if (string->size < 1) {
		// Return everything zeroed.
		return result;
	}

	mbstate_t multibyte_shift_state = { };
	const size_t c_utf8_character_max_size = QLIGHT_UNICODE_STRING_MAX_BYTES;
	u32 byte_backward_index = 1;
	while (result.size_in_bytes <= 0 && byte_backward_index < string->size_in_bytes) {
		// multibyte_shift_state = { };
		const char *c_utf8_character_pointer = (const char *) &string->data[string->size_in_bytes - byte_backward_index];
		result.size_in_bytes = mbrlen(c_utf8_character_pointer, c_utf8_character_max_size, &multibyte_shift_state);
		if (result.size_in_bytes > 0) {
			break;
		}

		byte_backward_index += 1;
	}

	// Copy character bytes from source string.
	for (u32 byte_index = 0; byte_index < result.size_in_bytes; byte_index += 1) {
		result.bytes[byte_index] = string->data[string->size_in_bytes - byte_backward_index];
		byte_backward_index -= 1;
	}

	if (do_pop) {
		string->size -= 1;
		string->size_in_bytes -= result.size_in_bytes;
	}

	return result;
}

StringChar string_peek(String *string, u32 offset_in_bytes) {
	mbstate_t multibyte_shift_state = { };
	return string_peek(string, offset_in_bytes, &multibyte_shift_state);
}

StringChar string_peek(String *string, u32 offset_in_bytes, mbstate_t *multibyte_shift_state) {
	StringChar result = { };

	if (!string) {
		// Return everything zeroed.
		return result;
	}

	if (offset_in_bytes >= string->size_in_bytes) {
		// Return everything zeroed.
		return result;
	}

	const char *c_utf8_character_pointer = (const char *) &string->data[offset_in_bytes];
	const size_t c_utf8_character_max_size = QLIGHT_UNICODE_STRING_MAX_BYTES;

	// Get character size using standard C library.
	result.size_in_bytes = static_cast<s32>(mbrlen(
		c_utf8_character_pointer,
		c_utf8_character_max_size,
		multibyte_shift_state
	));

	// Check whether the character has a valid UTF-8 encoding.
	if (result.size_in_bytes < 0) {
		AssertMessage(result.size_in_bytes < 0, "Trying to add an invalid UTF-8 character to string.");

		// Set `size_in_bytes` to -1 to let the function caller know the character was invalid.
		result.size_in_bytes = -1;
		return result;
	}

	// Copy character bytes from source string.
	for (u32 byte_index = 0; byte_index < result.size_in_bytes; byte_index += 1) {
		result.bytes[byte_index] = string->data[offset_in_bytes + byte_index];
	}

	return result;
}

void string_clear(String *string, bool zero_memory) {
	if (zero_memory)
		memset(string->data, 0, sizeof(char), * string->size_in_bytes);

	string->size = 0;
	string->size_in_bytes = 0;
}

bool string_free(String *string, bool zero_memory) {
	if (!string->data) return false;
	if (string->capacity < 1) return false;

	string_clear(string, zero_memory);
	Deallocate(string->allocator, string->data);
	return true;
}

bool string_equals(StringView *a, StringView *b) {
	u32 a_idx = 0;
	u32 b_idx = 0;
	while (a_idx < a->size && b_idx < b_size) {
		if (a->data[a_idx] != b->data[b_idx])
			return false;

		a_idx += 1;
		b_idx += 1;
	}

	return true;
}

bool string_equals(String *a, String *b) {
	return string_equals(string_view(a), string_view(b));StringView get_string_view(String *string, u32 offset = 0, u32 length = 0);
}
