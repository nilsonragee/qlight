#ifndef QLIGHT_LOG_H
#define QLIGHT_LOG_H

#include "common.h"
#include "console.h"

// Define this in the module.
// #define QL_LOG_CHANNEL "Name"

#define log_info( format, ... )     log( LogLevel_Info, QL_LOG_CHANNEL, format, __VA_ARGS__ )
#define log_warning( format, ... )  log( LogLevel_Warning, QL_LOG_CHANNEL, format, __VA_ARGS__ )
#define log_error( format, ... )    log( LogLevel_Error, QL_LOG_CHANNEL, format, __VA_ARGS__ )

#ifdef QLIGHT_DEBUG
#define log_debug( format, ... )    log( LogLevel_Debug, QL_LOG_CHANNEL, format, __VA_ARGS__ )
#else
#define log_debug( format, ... )
#endif

enum Log_Level : u8 {
	LogLevel_Info = 0,
	LogLevel_Warning,
	LogLevel_Error,
	LogLevel_Debug,

	LogLevel_COUNT,

	LogLevel_AllExceptDebug = LogLevel_Info | LogLevel_Warning | LogLevel_Error,
	LogLevel_All = LogLevel_AllExceptDebug | LogLevel_Debug
};

struct Log_Level_Parameters {
	StringView_ASCII prefix;
	StringView_ASCII color_code;
};

enum Log_Level_Bits : u8 {
	LogLevelBit_Info = ( 1 << 0 ),
	LogLevelBit_Warning = ( 1 << 1 ),
	LogLevelBit_Error = ( 1 << 2 ),
	LogLevelBit_Debug = ( 1 << 3 )
};

bool log_init();
void log_shutdown();

Log_Level_Parameters *log_level_parameters( Log_Level level );
void log( Log_Level level, StringView_ASCII channel, const char *format, ... );

#endif /* QLIGHT_LOG_H */
