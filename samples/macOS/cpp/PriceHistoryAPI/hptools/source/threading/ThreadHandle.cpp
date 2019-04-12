/* Copyright 2019 FXCM Global Services, LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use these files except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "stdafx.h"

#include "threading/ThreadHandle.h"

#ifdef ANDROID
#   include <signal.h>

void thread_exit_handler(int sig)
{
    if (sig == SIGUSR1)
        pthread_exit(0);
} 
#endif

using hptools::ThreadHandle;

ThreadHandle::ThreadHandle()
    : mHandle(getCurrentThreadHandle())
{
    #ifdef ANDROID
    /* Android is mission pthread_cancel implementation. This is solution via signals:
            register signal to enable thread killing itself. */
    struct sigaction sa = {0};
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = thread_exit_handler;
    sigaction(SIGUSR1, &sa, NULL);
    #endif
}

ThreadHandle::ThreadHandle(Handle handle)
    : mHandle(handle)
{
}

ThreadHandle::~ThreadHandle()
{
}

ThreadHandle* ThreadHandle::getCurrentThread()
{
    return new ThreadHandle();
}

void ThreadHandle::release()
{
    delete this;
}

bool ThreadHandle::isCurrentThread() const
{
#ifdef PTHREADS
    return pthread_equal(mHandle, pthread_self());
#else
    return mHandle == ::GetCurrentThreadId();
#endif
}

ThreadHandle::Handle ThreadHandle::getCurrentThreadHandle() const
{
#ifdef PTHREADS
    return pthread_self();
#elif WIN32
    return GetCurrentThreadId();
#endif
}
