#ifndef QLIGHT_STRING_ASCII_H
#define QLIGHT_STRING_ASCII_H

#include "string_common.h"
#include "array.h"

struct String_ASCII : Array< char > { };
struct StringView_ASCII : ArrayView< char > {
	StringView_ASCII( const StringView_ASCII &view ) = default;
	StringView_ASCII( StringView_ASCII &view ) = default;
	StringView_ASCII() = default;

	StringView_ASCII( u32 size, char *data );
	StringView_ASCII( const char *c_string );
};

/*
struct String_UTF8 : Array< char8_t > {
	u32 size_in_bytes;
};

struct StringView_UTF8 : ArrayView< char8_t > {
	u32 size_in_bytes;
};
*/

// Homemade `strlen`.
// Returns the length of the `c_string`.
u32 string_length( const char *c_string );

// Returns a read-only `view` inside of the other `view`, `string`, or `c_string`.
StringView_ASCII string_view( StringView_ASCII view, u32 offset = 0, u32 length = 0 );
StringView_ASCII string_view( String_ASCII *string, u32 offset = 0, u32 length = 0 );
StringView_ASCII string_view( const char *c_string, u32 offset = 0, u32 length = 0 );

// Creates an ASCII String with `initial_capacity_in_bytes` and
// memory managed by `allocator`.
String_ASCII string_new( Allocator *allocator, u32 initial_capacity_in_bytes );
String_ASCII string_new( Allocator *allocator, StringView_ASCII view );

inline String_ASCII string_new( Allocator *allocator, String_ASCII *string ) {
	return string_new( allocator, string_view( string ) );
}

inline String_ASCII string_new( Allocator *allocator, const char *c_string ) {
	return string_new( allocator, string_view( c_string ) );
}

// Appends an ASCII (1 byte long) character `ascii_char` to the `string`.
// Returns the index of the `ascii_char`'s first byte inside the `string`.
u32 string_add( String_ASCII *string, char ascii_char );

// Appends an ASCII `source` string to the `destination` string.
// Returns the index of the `source` string's first byte inside the `destination` string.
u32 string_add( String_ASCII *destination, StringView_ASCII source );

inline u32 string_add( String_ASCII *destination, String_ASCII *source ) {
	return string_add( destination, string_view( source ) );
}

inline u32 string_add( String_ASCII *destination, const char *source ) {
	return string_add( destination, string_view( source ) );
}

// Pops the last character from the `string`.
// The `do_pop` argument determines whether to actually pop the character.
// In the case of `false` value, the behaviour is similar to the `string_peek` from the end.
// Returns either a single `char` or a `UnicodeChar` structure.
// In the seconds case, the 1st byte is the popped character followed by zeroes.
char string_pop( String_ASCII *string, bool do_pop = true );
UnicodeChar string_pop_unicode( String_ASCII *string, bool do_pop = true );

// Peeks a character from either the `view`, `string`, or `c_string` with an `offset_in_bytes`.
// Returns either a single `char` or the `UnicodeChar` structure.
char string_peek( StringView_ASCII view, u32 offset_in_bytes );
char string_peek( String_ASCII *string, u32 offset_in_bytes );
char string_peek( const char *c_string, u32 offset_in_bytes );
UnicodeChar string_peek_unicode( String_ASCII *string, u32 offset_in_bytes );
UnicodeChar string_peek_unicode( StringView_ASCII view, u32 offset_in_bytes );
UnicodeChar string_peek_unicode( const char *c_string, u32 offset_in_bytes );

// Returns whether two `StringView`s, `String`s, or C strings are equal.
bool string_equals( StringView_ASCII a, StringView_ASCII b );

inline bool string_equals( String_ASCII *a, String_ASCII *b ) {
	return string_equals( string_view( a ), string_view( b ) );
}

inline bool string_equals( const char *a, const char *b ) {
	return string_equals( string_view( a ), string_view( b ) );
}

// Returns whether the `search` string was found at the beginning of the `source` string.
bool string_starts_with( StringView_ASCII source, StringView_ASCII search );

// Returns whether the `search` string was found at the end of the `source` string.
bool string_ends_with( StringView_ASCII source, StringView_ASCII search );

// Returns the offset to the beginning of the `search` string inside of `source` string, in bytes.
// If `search` string was not found, -1 is returned.
s64 string_contains( StringView_ASCII source, StringView_ASCII search );

// Clears the `string`'s contents.
// The `zero_memory` argument determines whether to fill left out memory with zeroes.
void string_clear( String_ASCII *string, bool zero_memory = false );

// Destroys the `string` by clearing its contents and freeing the allocated memory.
// The `zero_memory` argument determines whether to fill the memory with zeroes before deallocation.
bool string_free( String_ASCII *string, bool zero_memory = false );


/*
String_UTF8 string_new(Allocator *allocator, u32 initial_capacity_in_bytes);
String_UTF8 string_new(Allocator *allocator, const char8_t *c_utf8_string, u32 c_utf8_string_size = 0);


StringView_UTF8 string_view(String_UTF8 *string, u32 offset = 0, u32 length = 0);

u32 string_add(String_UTF8 *string, char ascii_char);

u32 string_add(String_UTF8 *string, StringChar utf8_char);

// Append UTF-8 (1-4 bytes long) character to a string.
// Returns the index of character's first byte inside a string.
u32 string_add_multibyte_char(String_UTF8 *string, char8_t *utf8_character, u32 utf8_character_size_in_bytes);

// Append UTF-8 (1-4 bytes long) character sequence (one or more characters) to a string.
// Returns the index of last added character's first byte inside a string.
u32 string_add(String *string, char8_t *utf8_char_array, u32 array_size_in_bytes, u32 *utf8_characters_added = NULL);

// Pop last character from `string`.
// Returns `StringChar` stucture which contains character data.
StringChar string_pop(String *string, bool do_pop = true);

// Peek a character from `string`
StringChar string_peek(String *string, u32 offset_in_bytes);
StringChar string_peek(String *string, u32 offset_in_bytes, mbstate_t *multibyte_shift_state);

void string_clear(String *string, bool zero_memory = false);
bool string_free(String *string, bool zero_memory = false);

bool string_equals(String *a, String *b);
*/

/* TODO */


#endif /* QLIGHT_STRING_ASCII_H */
