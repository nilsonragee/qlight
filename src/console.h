#ifndef QLIGHT_CONSOLE_H
#define QLIGHT_CONSOLE_H

#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#define NOMCX
#define NOIME
#include <Windows.h>
#undef NOSERVICE
#undef NOMCX
#undef NOIME

#include "common.h"
#include "string.h"

// \x1B == Hexadecimal 1B == Decimal 27 == ASCII <ESC>
#define QL_COLOR_RED      "\x1B[31m"
#define QL_COLOR_GREEN    "\x1B[32m"
#define QL_COLOR_YELLOW   "\x1B[33m"
#define QL_COLOR_BLUE     "\x1B[34m"
#define QL_COLOR_MAGENTA  "\x1B[35m"
#define QL_COLOR_CYAN     "\x1B[36m"
#define QL_COLOR_WHITE    "\x1B[37m"
#define QL_COLOR_BLACK    "\x1B[38m"
#define QL_COLOR_RESET    "\x1B[0m"

// 256-bit colors
#define QL_COLOR_GRAY     "\x1b[38;5;244m"

bool console_init( UINT code_page_id, const wchar_t *title_wide = NULL );
void console_free();

// UTF-8:
void console_print_to( HANDLE output_handle, const char *format, ... );
void console_print( const char *format, ... );
void console_print_unformatted( StringView_ASCII message );

// UTF-16:
void console_print_to_wide( HANDLE output_handle, const wchar_t *format, ... );
void console_print_wide( const wchar_t *format_wide, ... );

#endif /* QLIGHT_CONSOLE_H */
