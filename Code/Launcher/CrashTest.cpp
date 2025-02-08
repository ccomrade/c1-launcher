#include <assert.h>
#include <intrin.h>
#include <stdlib.h>
#include <string.h>

#include "CryCommon/CrySystem/IConsole.h"
#include "CryCommon/CrySystem/ISystem.h"

#include "CrashTest.h"

static void SegFault()
{
	volatile int* p = NULL;
	*p = 0xabcd;
}

static void IntDivisionByZero()
{
	volatile int i = 0;
	i /= i;
}

static void BadAllocBig()
{
	volatile bool b = true;
	while (b)
	{
		new char[16 * 1024 * 1024];
	}
}

static void BadAllocSmall()
{
	volatile bool b = true;
	while (b)
	{
		new char[128];
	}
}

static void InvalidCrtArgument()
{
	strcpy_s(NULL, 4, "abc");
}

static void PureVirtualCall()
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

static void StackOverflow()
{
	volatile int i = 0;
	volatile int* p = &i;
	volatile bool b = true;
	while (b)
	{
		// stack grows downwards
		*(--p) = 666;
	}
}

static void CrashTestHandler(IConsoleCmdArgs* pArgs)
{
	if (pArgs->GetArgCount() != 2)
	{
		CryLogWarningAlways("'%s' requires one argument", pArgs->GetCommandLine());
		return;
	}

	const int value = atoi(pArgs->GetArg(1));

	switch (value)
	{
		case 1:
		{
			SegFault();
			break;
		}
		case 2:
		{
			IntDivisionByZero();
			break;
		}
		case 3:
		{
			BadAllocBig();
			break;
		}
		case 4:
		{
			gEnv->pSystem->Error("sys_crashtest %d", value);
			break;
		}
		case 5:
		{
			BadAllocSmall();
			break;
		}
		case 6:
		{
			assert(0);
			break;
		}
		case 7:
		{
			__debugbreak();
			break;
		}
		case 8:
		{
			abort();
			break;
		}
		case 9:
		{
			InvalidCrtArgument();
			break;
		}
		case 10:
		{
			PureVirtualCall();
			break;
		}
		case 11:
		{
			throw 666;
			break;
		}
		case 12:
		{
			StackOverflow();
			break;
		}
		default:
		{
			CryLogWarningAlways("sys_crashtest %d is not supported", value);
			break;
		}
	}
}

void CrashTest::Register()
{
	IConsole* pConsole = gEnv->pConsole;

	// disable the original cvar without removing it completely because CrySystem holds a pointer to it
	pConsole->UnregisterVariable("sys_crashtest", false);

	pConsole->AddCommand("sys_crashtest", &CrashTestHandler, 0, "Crash the game");
}
