#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "console.h"

constexpr const size_t CONSOLE_BUFFER_SIZE = 4096;

struct G_Console {
	HANDLE   input = NULL;
	HANDLE   output = NULL;
	  bool   allocated = false;
} g_console;

bool console_init( UINT code_page_id, const wchar_t *title_wide ) {
	if ( g_console.allocated )
		return false;

	HANDLE input = GetStdHandle( STD_INPUT_HANDLE );
	HANDLE output = GetStdHandle( STD_OUTPUT_HANDLE );
	bool input_present = ( input != NULL && input != INVALID_HANDLE_VALUE );
	bool output_present = ( output != NULL && output != INVALID_HANDLE_VALUE );
	if ( !input_present || !output_present ) {
		g_console.allocated = AllocConsole();
		if ( !g_console.allocated )
			return false;
	}

	// Do not reuse old ones since AllocConsole() updates handles.
	g_console.input = GetStdHandle( STD_INPUT_HANDLE );
	g_console.output = GetStdHandle( STD_OUTPUT_HANDLE );
	g_console.allocated = true;

	// Make sure Unicode characters will be displayed properly.
	SetConsoleCP( code_page_id );
	SetConsoleOutputCP( code_page_id );

	// Enable processing of ANSI escape codes (changing foreground color, etc.).
	DWORD console_mode;
	GetConsoleMode( g_console.output, &console_mode );
	console_mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode( g_console.output, console_mode );

	if ( title_wide )
		SetConsoleTitleW( title_wide );

	return g_console.allocated;
}

void console_free() {
	if ( !g_console.allocated )
		return;

	g_console.input = NULL;
	g_console.output = NULL;
	g_console.allocated = false;
}

void console_print_to( HANDLE output_handle, const char *format, ... ) {
	va_list var_args;
	va_start( var_args, format );

	char buffer[ CONSOLE_BUFFER_SIZE ];
	int written = vsnprintf( buffer, CONSOLE_BUFFER_SIZE, format, var_args );
	WriteConsoleA( output_handle, buffer, static_cast< size_t >( written ), NULL, NULL );
}

void console_print( const char *format, ... ) {
	if ( !g_console.allocated )
		return;

	va_list var_args;
	va_start( var_args, format );

	char buffer[ CONSOLE_BUFFER_SIZE ];
	int written = vsnprintf( buffer, CONSOLE_BUFFER_SIZE, format, var_args );
	WriteConsoleA( g_console.output, buffer, static_cast< size_t >( written ), NULL, NULL );
}

void console_print_unformatted( StringView_ASCII message ) {
	if ( !g_console.allocated )
		return;

	WriteConsoleA( g_console.output, message.data, ( DWORD )message.size, NULL, NULL );
}

void console_print_to_wide( HANDLE output_handle, const wchar_t *format_wide, ... ) {
	va_list var_args;
	va_start( var_args, format_wide );

	wchar_t buffer[ CONSOLE_BUFFER_SIZE ];
	int written = _vsnwprintf( buffer, CONSOLE_BUFFER_SIZE, format_wide, var_args );
	WriteConsoleW( output_handle, buffer, static_cast< size_t >( written ), NULL, NULL );
}

void console_print_wide( const wchar_t *format_wide, ... ) {
	if ( !g_console.allocated )
		return;

	va_list var_args;
	va_start( var_args, format_wide );

	wchar_t buffer[ CONSOLE_BUFFER_SIZE ];
	int written = _vsnwprintf( buffer, CONSOLE_BUFFER_SIZE, format_wide, var_args );
	WriteConsoleW( g_console.output, buffer, static_cast< size_t >( written ), NULL, NULL );
}
