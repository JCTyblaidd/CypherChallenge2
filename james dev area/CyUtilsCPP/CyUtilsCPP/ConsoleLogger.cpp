#include "ConsoleLogger.hpp"
#ifdef _WIN32
#include <Windows.h>
#endif

namespace logging {
	ConsoleLogHandler::ConsoleLogHandler() {
#ifdef _WIN32
		consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	}

	void ConsoleLogHandler::_newLine() {
		std::cout << std::endl;
	}

	void ConsoleLogHandler::_setBlue() {
#ifdef _WIN32
		SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else

#endif
	}
	void ConsoleLogHandler::_setYellow() {
#ifdef _WIN32
		SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else

#endif
	}
	void ConsoleLogHandler::_setRed() {
#ifdef _WIN32
		SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
#else

#endif
	}
	void ConsoleLogHandler::_setWhite() {
#ifdef _WIN32
		SetConsoleTextAttribute(consoleHandle, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
#else

#endif
	}
	void ConsoleLogHandler::_text(String str) {
		std::cout << str;
	}
}