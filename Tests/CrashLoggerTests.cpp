#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <intrin.h>

#include "Library/CrashLogger.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

static void Test_SegFault()
{
	volatile int* p = NULL;
	*p = 0xabcd;
}

static void Test_IntDivisionByZero()
{
	volatile int i = 0;
	i /= i;
}

static void Test_InvalidCrtArgument()
{
	strcpy_s(NULL, 4, "abc");
}

static void Test_PureVirtualCall()
{
	struct Base
	{
		Base()
		{
			this->Boom();
		}

		void Boom()
		{
			this->Call();
		}

		virtual void Call() = 0;
	};

	struct Derived : Base
	{
		void Call() override
		{
		}
	};

	Derived d;
}

static void CallEngineError(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	CrashLogger::OnEngineError(format, args);
	va_end(args);
}

static void Test_EngineError()
{
	CallEngineError("engine error test");
}

static void Test_DebugBreak()
{
	__debugbreak();
}

static void Test_Halt()
{
	__halt();
}

static void Test_UnhandledCppException()
{
	throw 666;
}

static void Test_StdAbort()
{
	std::abort();
}

static void Test_StdTerminate()
{
	std::terminate();
}

static const struct { const char* name; void (*func)(); } TESTS[] = {
	{ "SegFault", &Test_SegFault },
	{ "IntDivisionByZero", &Test_IntDivisionByZero },
	{ "InvalidCrtArgument", &Test_InvalidCrtArgument },
	{ "PureVirtualCall", &Test_PureVirtualCall },
	{ "EngineError", &Test_EngineError },
	{ "DebugBreak", &Test_DebugBreak },
	{ "Halt", &Test_Halt },
	{ "UnhandledCppException", &Test_UnhandledCppException },
	{ "StdAbort", &Test_StdAbort },
	{ "StdTerminate", &Test_StdTerminate },
};

static std::FILE* MockLogFileProvider()
{
	// crash successfully caught
	std::exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::fprintf(stderr, "Usage: %s TEST\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char* test = argv[1];

	// initialize crash logger
	CrashLogger::Enable(&MockLogFileProvider, "Test banner");

	for (std::size_t i = 0; i < ARRAY_SIZE(TESTS); i++)
	{
		if (std::strcmp(TESTS[i].name, test) == 0)
		{
			// execute the test
			TESTS[i].func();

			// all tests should trigger a crash and never return
			return EXIT_FAILURE;
		}
	}

	// test not found
	return EXIT_FAILURE;
}
