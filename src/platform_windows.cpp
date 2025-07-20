#define _CRT_SECURE_NO_WARNINGS

#include "platform.h"
#include <stdio.h> // snprintf()
#include <stdlib.h> // exit()

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#include <Windows.h>

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

	// TODO(nilsoncore): Decide whether the assertion failure message box
	// will pop out or not if the debugger is already attached.
	if (IsDebuggerPresent()) {
		DebugBreak();
		// NOTE(nilsoncore):
		// If you are in the debugger (presumably Visual Studio since it's Windows),
		// the green cursor will point at the `return` expression, but this is not
		// the expression on which the assert actually failed. The green cursor
		// points at the next statement that will be executed, but the breakpoint
		// itself is being caused in the `KernelBase.dll`. You can see that in
		// the Call Stack windows, which you should keep open. There will be a
		// yellow cursor that points to the kernel dll, but there's no source code
		// files for the debugger to open for it, so it opens last call.
		//
		// In order to get to the failed assert, you have to navigate through the
		// Call Stack from top to bottom. The function that caused the assert failure
		// is usually one below `platform_assert_fail()` function.
		//
		// Once you get to the assert line, you will see that the green cursor points
		// at line 1 below the actual assert. As it is described above, the green cursor
		// points at the next statement that will be executed, so this is expected,
		// there's nothing wrong with the debugger. (aside from UX design, I guess)
		return;
	}

	const char attach[] =
		"\n"
		"Attach a debugger to the process and press \"Retry\" to start debugging, or press \"Cancel\" to close the program.";
	const size_t attach_length = sizeof(attach);

	// Warning: cursor position is not updated after this!
	strncat(
		/* destination */ text + cursor,
		/*      source */ attach,
		/*       count */ attach_length
	);

	const char *text_skipped_title = text + title_length_minus_1 + 2;
	int pressed_button_id = MessageBoxA(
		/* HWND        hWnd */ NULL,
		/* LPCSTR    lpText */ text_skipped_title,
		/* LPCSTR lpCaption */ title,
		/* UINT       uType */ MB_ICONERROR | MB_RETRYCANCEL | MB_SETFOREGROUND | MB_TASKMODAL
	);

	switch (pressed_button_id) {
		case IDRETRY: {
			DebugBreak();
			// NOTE(nilsoncore):
			// This is not the actual assertion statement line.
			// Read a big comment above.
			break;
		}
		case IDCANCEL:
		default: {
			exit(EXIT_FAILURE);
			break;
		}
	}
}
