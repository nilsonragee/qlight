#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

#include "log.h"

constexpr const size_t LOG_BUFFER_SIZE = 4096;

struct G_Log {
	Log_Level_Parameters level_parameters[ LogLevel_COUNT ];
} g_log;

Log_Level_Parameters *log_level_parameters( Log_Level level ) {
	return &g_log.level_parameters[ level ];
}

bool log_init() {
	g_log.level_parameters[ LogLevel_Info ] = {
		.prefix = "INFO",
		.color_code = QL_COLOR_RESET
	};
	g_log.level_parameters[ LogLevel_Warning ] = {
		.prefix = "WARNING",
		.color_code = QL_COLOR_YELLOW
	};
	g_log.level_parameters[ LogLevel_Error ] = {
		.prefix = "ERROR",
		.color_code = QL_COLOR_RED
	};
	g_log.level_parameters[ LogLevel_Debug ] = {
		.prefix = "DEBUG",
		.color_code = QL_COLOR_GRAY
	};

	return true;
}

void log_shutdown() {

}

void log( Log_Level level, StringView_ASCII channel, const char *format, ... ) {
	va_list var_args;
	va_start( var_args, format );

	char buffer[ LOG_BUFFER_SIZE ];

	Log_Level_Parameters *params = log_level_parameters( level );

	int cursor = 0;
	cursor += snprintf(
		buffer + cursor,
		LOG_BUFFER_SIZE,
		StringViewFormat "[" StringViewFormat "][" StringViewFormat "]: ",
		StringViewArgument( params->color_code ),
		StringViewArgument( channel ),
		StringViewArgument( params->prefix )
	);

	/* Insert '\n\0' at the end of message/buffer */

	cursor += vsnprintf( buffer + cursor, LOG_BUFFER_SIZE - cursor, format, var_args );
	// `cursor - 1`: Ignore null-terminator so it points to the last char
	int last_char_idx = ( cursor < LOG_BUFFER_SIZE ) ? cursor - 1 : LOG_BUFFER_SIZE - 3;
	buffer[ last_char_idx + 1 ] = '\n';
	buffer[ last_char_idx + 2 ] = '\0';
	console_print_unformatted( string_view( buffer, 0, cursor + 1 /* new line */ ) );
}
