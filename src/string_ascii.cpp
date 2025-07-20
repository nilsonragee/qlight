#include "string_ascii.h"

StringView_ASCII::StringView_ASCII( u32 size, char *data ) {
	this->size = size;
	this->data = data;
}

StringView_ASCII::StringView_ASCII( const char *c_string ) {
	StringView_ASCII view = string_view( c_string );
	this->size = view.size;
	this->data = view.data;
}

u32 string_length( const char *c_string ) {
	u32 size = 0;
	if ( !c_string )
		return size;

	char c = *c_string;
	while ( c != '\0' ) {
		c_string += sizeof( char );
		size += sizeof( char );
		c = *c_string;
	}

	return size;
}

String_ASCII string_new( Allocator *allocator, u32 initial_capacity_in_bytes ) {
	Array< char > array = array_new< char >( allocator, initial_capacity_in_bytes );
	String_ASCII string = static_cast< String_ASCII >( array );
	return string;
}

String_ASCII string_new( Allocator *allocator, StringView_ASCII view ) {
	Array< char > array = array_new< char >( allocator, view.size );
	String_ASCII string = static_cast< String_ASCII >( array );
	string_add( &string, view );
	return string;
}

u32 string_add( String_ASCII *string, char ascii_char ) {
	const u32 size = string->size;
	if ( size + 1 * sizeof( char ) > string->capacity ) {
		array_resize( string, string->capacity * 2 );
	}

	string->data[ size ] = ascii_char;
	string->size += 1;

	return size;
}

u32 string_add( String_ASCII *destination, StringView_ASCII source ) {
	const u32 size = destination->size;
	if ( size + source.size * sizeof( char ) > destination->capacity ) {
		array_resize( destination, destination->capacity * 2 );
	}

	u32 dst_idx = size;
	u32 src_idx = 0;
	while ( src_idx < source.size ) {
		destination->data[ dst_idx ] = source.data[ src_idx ];
		destination->size += 1;
		dst_idx += 1;
		src_idx += 1;
	}

	return size;
}

static StringView_ASCII string_view_impl( u32 size, const char *data, u32 offset, u32 length ) {
	if ( length != 0 )
		size = length;

	// StringView_ASCII view;
	// view.size = length;
	// view.data = ( char * )data + offset;
	StringView_ASCII view = {
		/* .size */ size,
		/* .data */ const_cast< char * >( data + offset )
	};

	return view;
}

StringView_ASCII string_view( StringView_ASCII view, u32 offset, u32 length ) {
	return string_view_impl( view.size, view.data, offset, length );
}

StringView_ASCII string_view( String_ASCII *string, u32 offset, u32 length ) {
	return string_view_impl( string->size, string->data, offset, length );
}

StringView_ASCII string_view( const char *c_string, u32 offset, u32 length ) {
	return string_view_impl( string_length( c_string ), c_string, offset, length );
}

char string_pop( String_ASCII *string, bool do_pop ) {
	char popped = string->data[ string->size - 1 ];
	if ( do_pop )
		string->size -= 1;

	return popped;
}

UnicodeChar string_pop_unicode( String_ASCII *string, bool do_pop ) {
	char popped = string_pop( string, do_pop );
	UnicodeChar variable = {
		.size_in_bytes = 1,
		.bytes = { static_cast< char8_t >( popped ), 0x00, 0x00, 0x00 }
	};

	return variable;
}

static char string_peek_impl( u32 size, const char *data, u32 offset_in_bytes ) {
	if ( offset_in_bytes > size )
		return NULL;

	return data[ offset_in_bytes ];
}

char string_peek( StringView_ASCII view, u32 offset_in_bytes ) {
	return string_peek_impl( view.size, view.data, offset_in_bytes );
}

char string_peek( String_ASCII *string, u32 offset_in_bytes ) {
	return string_peek_impl( string->size, string->data, offset_in_bytes );
}

char string_peek( const char *c_string, u32 offset_in_bytes ) {
	return string_peek_impl( string_length( c_string ), c_string, offset_in_bytes );
}

static UnicodeChar string_peek_unicode_impl( u32 size, const char *data, u32 offset_in_bytes ) {
	char peeked = string_peek_impl( size, data, offset_in_bytes );
	UnicodeChar unicode = {
		.size_in_bytes = 1,
		.bytes = { static_cast< char8_t >( peeked ), 0x00, 0x00, 0x00 }
	};

	return unicode;
};

UnicodeChar string_peek_unicode( StringView_ASCII view, u32 offset_in_bytes ) {
	return string_peek_unicode_impl( view.size, view.data, offset_in_bytes );
}

UnicodeChar string_peek_unicode( String_ASCII *string, u32 offset_in_bytes ) {
	return string_peek_unicode_impl( string->size, string->data, offset_in_bytes );
}

UnicodeChar string_peek_unicode( const char *c_string, u32 offset_in_bytes ) {
	return string_peek_unicode_impl( string_length( c_string ), c_string, offset_in_bytes );
}

bool string_equals( StringView_ASCII a, StringView_ASCII b ) {
	const u32 size = a.size;
	if ( size != b.size )
		return false;

	for ( u32 char_idx = 0; char_idx < size; char_idx += 1 ) {
		if ( a.data[ char_idx ] != b.data[ char_idx ] )
			return false;
	}

	return true;
}

bool string_starts_with( StringView_ASCII source, StringView_ASCII search ) {
	if ( search.size > source.size )
		return false;

	u32 char_idx = 0;
	while ( source.data[ char_idx ] == search.data[ char_idx ] && char_idx < search.size ) {
		char_idx += 1;
	}

	bool starts = char_idx + 1 == search.size;
	return starts;
}

bool string_ends_with( StringView_ASCII source, StringView_ASCII search ) {
	if ( search.size > source.size )
		return false;

	u32 char_idx = source.size - search.size;
	while ( source.data[ char_idx ] == search.data[ char_idx ] && char_idx < search.size ) {
		char_idx += 1;
	}

	bool ends = char_idx + 1 == search.size;
	return ends;
}

// Ok for short strings, terrible for long ones.
// TODO: Use sophisticated search method for strings with size bigger than some constant.
s64 string_contains( StringView_ASCII source, StringView_ASCII search ) {
	if ( search.size > source.size )
		return -1;

	u32 consecutive_matches = 0;
	for ( u32 char_idx = 0; char_idx < search.size; char_idx += 1 ) {
		if ( source.data[ char_idx ] == search.data[ char_idx ] ) {
			consecutive_matches += 1;
		} else {
			consecutive_matches = 0;
			continue;
		}

		if ( consecutive_matches == search.size )
			return char_idx - consecutive_matches;
	}

	return -1;
}

void string_clear( String_ASCII *string, bool zero_memory ) {
	array_clear( string, zero_memory );
}

bool string_free( String_ASCII *string, bool zero_memory ) {
	return array_free( string, zero_memory );
}

/*
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
*/

/*
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
*/

/*
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
*/

/*
StringChar string_peek(String *string, u32 offset_in_bytes) {
	mbstate_t multibyte_shift_state = { };
	return string_peek(string, offset_in_bytes, &multibyte_shift_state);
}
*/

/*
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
*/
