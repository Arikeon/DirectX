#pragma once

#define ENABLE_LEAK_DETECTION 1
#define ENABLE_CONSOLE_DEBUGGING 1
#define ENABLE_INLINE 1
#define ENABLE_CHECK 1

#if ENABLE_CHECK
	#include <assert.h>
	#include <comdef.h>
	#define check(exp) assert((exp))
	#define checkhr(hr) check(!FAILED(hr))
#else
	#define check(exp)
	#define checkhr(hr)
#endif

#if ENABLE_INLINE
	#define INLINE inline
	#define NOINLINE __declspec(noinline)
#else
	#define INLINE
	#define NOINLINE
#endif

#if ENABLE_LEAK_DETECTION
	#include <crtdbg.h>
	#define MEMORY_LEAK_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
	#define new MEMORY_LEAK_NEW
	#define MEMORY_LEAK_REPORT(line) _CrtSetBreakAlloc(line); _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG)
	#define MEMORY_LEAK_START(line) _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); MEMORY_LEAK_REPORT(line)
	#define MEMORY_LEAK_END _CrtDumpMemoryLeaks();
#else
	#define MEMORY_LEAK_NEW
	#define MEMORY_LEAK_REPORT(line)
	#define MEMORY_LEAK_START(line)
	#define MEMORY_LEAK_END
#endif

#if ENABLE_CONSOLE_DEBUGGING
	#define CONSOLE_LOG(exp) 
	#define CONSOLE_REPORT_PROGRESS(index, cap, strtoappend)
#else
	#define CONSOLE_LOG(exp)
	#define CONSOLE_REPORT_PROGRESS(index, cap, strtoappend)
#endif