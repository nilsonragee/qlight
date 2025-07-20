#ifndef QLIGHT_PLATFORM_H
#define QLIGHT_PLATFORM_H

#include "types.h"

/*
	Platform deduction.

	Based on: https://github.com/cpredef/predef/blob/master/OperatingSystems.md
	Platforms to be supported: Windows-x64, Linux-x64. -nilsoncore, 12 March 2024
*/

#if defined(_WIN32)
	#define QLIGHT_PLATFORM_WINDOWS 1
#elif defined(__linux__) || defined(__gnu_linux)
	//     __linux__: Linux kernel (GNU/Linux and Android)
	// __gnu_linux__: GNU/Linux
	#define QLIGHT_PLATFORM_LINUX 1
#else
	#error Unsupported platform.
#endif

/*
	Debug build functionality.
*/

#if defined(QLIGHT_DEBUG)
	// Assert in Debug build.
	#define Assert(expression) \
		if (!(expression)) { platform_assert_fail(#expression, "-", __FILE__, __LINE__); }

	// Assert with message in Debug build.
	#define AssertMessage(expression, message) \
		if (!(expression)) { platform_assert_fail(#expression, message, __FILE__, __LINE__); }
#else
	// Do nothing in Release build.
	#define Assert(expression)
	#define AssertMessage(expression, message)
#endif

extern "C" void platform_assert_fail(const char *expression, const char *message, const char *file, long line);

#endif /* QLIGHT_PLATFORM_H */
