/*
 *  Debug_GetCallstack.cpp
 *  OpenLieroX
 *
 *  Created by Albert Zeyer on 06.04.12.
 *  code under LGPL
 *
 */

/*
 About the POSIX solution:
 
 Initially, I wanted to implement something similar as suggested
 here <http://stackoverflow.com/a/4778874/133374>, i.e. getting
 somehow the top frame pointer of the thread and unwinding it
 manually (the linked source is derived from Apples `backtrace`
 implementation, thus might be Apple-specific, but the idea is
 generic).
 
 However, to have that safe (and the source above is not and
 may even be broken anyway), you must suspend the thread while
 you access its stack. I searched around for different ways to
 suspend a thread and found:
  - http://stackoverflow.com/questions/2208833/how-do-i-suspend-another-thread-not-the-current-one
  - http://stackoverflow.com/questions/6367308/sigstop-and-sigcont-equivalent-in-threads
  - http://stackoverflow.com/questions/2666059/nptl-sigcont-and-thread-scheduling
 Basically, there is no really good way. The common hack, also
 used by the Hotspot JAVA VM (<http://stackoverflow.com/a/2221906/133374>),
 is to use signals and sending a custom signal to your thread via
 `pthread_kill` (<http://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_kill.html>).
 
 So, as I would need such signal-hack anyway, I can have it a bit
 simpler and just use `backtrace` inside the called signal handler
 which is executed in the target thread (as also suggested here:
 <http://stackoverflow.com/a/6407683/133374>). This is basically
 what this implementation is doing.
 
 If you are also interested in printing the backtrace, see:
 - backtrace_symbols_str() in Debug_extended_backtrace.cpp
 - DumpCallstack() in Debug_DumpCallstack.cpp
 */


#include "Debug.h"
#include "Mutex.h"
#include "util/StaticVar.h"

// When implementing iterating over threads on Mac, this might be useful:
// http://llvm.org/viewvc/llvm-project/lldb/trunk/tools/darwin-threads/examine-threads.c?view=markup

// For getting the callback, maybe libunwind can be useful: http://www.nongnu.org/libunwind/


#ifndef HAVE_EXECINFO
#	if defined(__linux__)
#		define HAVE_EXECINFO 1
#	elif defined(__DARWIN_VERS_1050)
#		define HAVE_EXECINFO 1
#	else
#		define HAVE_EXECINFO 0
#	endif
#endif

#if HAVE_EXECINFO
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#endif

#if HAVE_EXECINFO

#include <signal.h>
#include <pthread.h>

static StaticVar<Mutex> callstackMutex;
static ThreadId callingThread = 0;
static ThreadId targetThread = 0;
static void** threadCallstackBuffer = NULL;
static int threadCallstackBufferSize = 0;
static int threadCallstackCount = 0;

#define CALLSTACK_SIG SIGUSR2

void* GetPCFromUContext(void* ucontext);

__attribute__((noinline))
static void _callstack_signal_handler(int signr, siginfo_t *info, void *secret) {
	ThreadId myThread = (ThreadId)pthread_self();
	//notes << "_callstack_signal_handler, self: " << myThread << ", target: " << targetThread << ", caller: " << callingThread << endl;
	if(myThread != targetThread) return;
	
	threadCallstackCount = backtrace(threadCallstackBuffer, threadCallstackBufferSize);
	
	// Search for the frame origin.
	for(int i = 1; i < threadCallstackCount; ++i) {
		if(threadCallstackBuffer[i] != NULL) continue;
		
		// Found it at stack[i]. Thus remove the first i.
		const int IgnoreTopFramesNum = i;
		threadCallstackCount -= IgnoreTopFramesNum;
		memmove(threadCallstackBuffer, threadCallstackBuffer + IgnoreTopFramesNum, threadCallstackCount * sizeof(void*));
		threadCallstackBuffer[0] = GetPCFromUContext(secret); // replace by real PC ptr
		break;
	}
	
	// continue calling thread
	pthread_kill((pthread_t)callingThread, CALLSTACK_SIG);
}

static void _setup_callstack_signal_handler() {
	struct sigaction sa;
	sigfillset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = _callstack_signal_handler;
	sigaction(CALLSTACK_SIG, &sa, NULL);	
}

__attribute__((noinline))
int GetCallstack(ThreadId threadId, void **buffer, int size) {
	if(threadId == 0 || threadId == (ThreadId)pthread_self()) {
		int count = backtrace(buffer, size);
		static const int IgnoreTopFramesNum = 1; // remove this `GetCallstack` frame
		if(count > IgnoreTopFramesNum) {
			count -= IgnoreTopFramesNum;
			memmove(buffer, buffer + IgnoreTopFramesNum, count * sizeof(void*));
		}
		return count;
	}
	
	Mutex::ScopedLock lock(callstackMutex.get());
	callingThread = (ThreadId)pthread_self();
	targetThread = threadId;
	threadCallstackBuffer = buffer;
	threadCallstackBufferSize = size;
	
	_setup_callstack_signal_handler();

	// call _callstack_signal_handler in target thread
	if(pthread_kill((pthread_t)threadId, CALLSTACK_SIG) != 0)
		// something failed ...
		return 0;

	{
		sigset_t mask;
		sigfillset(&mask);
		sigdelset(&mask, CALLSTACK_SIG);

		// wait for CALLSTACK_SIG on this thread
		sigsuspend(&mask);
	}
	
	threadCallstackBuffer = NULL;
	threadCallstackBufferSize = 0;
	return threadCallstackCount;
}

#else // !HAVE_EXECINFO

// TODO: win32 implementation
// This might be useful: http://stackwalker.codeplex.com/SourceControl/changeset/view/66907#604665
// Esp, SuspendThread, ResumeThread, GetThreadContext, STACKFRAME64, ...

int GetCallstack(ThreadId threadId, void **buffer, int size) {
	return 0;
}

#endif
