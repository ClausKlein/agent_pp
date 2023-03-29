/*_############################################################################
  _##
  _##  AGENT++ 4.5 - threads.cpp
  _##
  _##  Copyright (C) 2000-2021  Frank Fock and Jochen Katz (agentpp.com)
  _##
  _##  Licensed under the Apache License, Version 2.0 (the "License");
  _##  you may not use this file except in compliance with the License.
  _##  You may obtain a copy of the License at
  _##
  _##      http://www.apache.org/licenses/LICENSE-2.0
  _##
  _##  Unless required by applicable law or agreed to in writing, software
  _##  distributed under the License is distributed on an "AS IS" BASIS,
  _##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  _##  See the License for the specific language governing permissions and
  _##  limitations under the License.
  _##
  _##########################################################################*/

#include <agent_pp/mib.h>
#include <agent_pp/mib_entry.h>
#include <agent_pp/threads.h>
#include <libagent.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp
{
#endif

#ifndef _NO_LOGGING
static const char* loggerModuleName = "agent++.threads";
#endif

#ifdef _THREADS
Synchronized ThreadManager::global_lock;
#endif

/**
 * Default constructor
 */
ThreadManager::ThreadManager() { }

/**
 * Destructor
 */
ThreadManager::~ThreadManager()
{
#if defined(_THREADS) && !defined(NO_FAST_MUTEXES)
    if (trylock() == LOCKED) { unlock(); }
#endif
}

/**
 * Start synchronized execution.
 */
void ThreadManager::start_synch()
{
#ifdef _THREADS
    lock();
#endif
}

/**
 * End synchronized execution.
 */
void ThreadManager::end_synch()
{
#ifdef _THREADS
    unlock();
#endif
}

/**
 * Start global synchronized execution.
 */
void ThreadManager::start_global_synch()
{
#ifdef _THREADS
    global_lock.lock();
#endif
}

/**
 * End global synchronized execution.
 */
void ThreadManager::end_global_synch()
{
#ifdef _THREADS
    global_lock.unlock();
#endif
}

ThreadSynchronize::ThreadSynchronize(ThreadManager& sync) : s(sync)
{
#ifdef _THREADS
    s.start_synch();
#endif
}

ThreadSynchronize::~ThreadSynchronize()
{
#ifdef _THREADS
    s.end_synch();
#endif
}

SingleThreadObject::SingleThreadObject() : ThreadManager() { start_synch(); }

SingleThreadObject::~SingleThreadObject() { end_synch(); }

#ifdef _THREADS

/*--------------------- class Synchronized -------------------------*/

#    ifndef _NO_LOGGING
unsigned int Synchronized::next_id = 0;
#    endif

#    define ERR_CHK_WITHOUT_EXCEPTIONS(x)                                          \
        do {                                                                       \
            int result = (x);                                                      \
            if (result)                                                            \
            {                                                                      \
                LOG_BEGIN(loggerModuleName, ERROR_LOG | 0);                        \
                LOG("Constructing Synchronized failed at '" #x "' with (result)"); \
                LOG(result);                                                       \
                LOG_END;                                                           \
            }                                                                      \
        } while (0)

Synchronized::Synchronized()
{
#    ifndef _NO_LOGGING
    id = next_id++;
    if (id > 1) // static initialization order fiasco: Do not log on first calls
    {
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
        LOG("Synchronized created (id)(ptr)");
        LOG(id);
        LOG((unsigned long)this);
        LOG_END;
    }
#    endif
#    ifdef POSIX_THREADS
    pthread_mutexattr_t attr;
    ERR_CHK_WITHOUT_EXCEPTIONS(pthread_mutexattr_init(&attr));
#        ifdef AGENTPP_PTHREAD_RECURSIVE
    ERR_CHK_WITHOUT_EXCEPTIONS(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE));
#        else
    ERR_CHK_WITHOUT_EXCEPTIONS(pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK));
#        endif

    memset(&monitor, 0, sizeof(monitor));
    ERR_CHK_WITHOUT_EXCEPTIONS(pthread_mutex_init(&monitor, &attr));
    ERR_CHK_WITHOUT_EXCEPTIONS(pthread_mutexattr_destroy(&attr));

    memset(&cond, 0, sizeof(cond));
    ERR_CHK_WITHOUT_EXCEPTIONS(pthread_cond_init(&cond, nullptr));
#    else
#        ifdef WIN32
    // Semaphore initially auto signaled, auto reset mode, unnamed
    semEvent = CreateEvent(0, false, false, 0);
    // Semaphore initially unowned, unnamed
    semMutex = CreateMutex(0, false, 0);
#        endif
#    endif
    isLocked = false;
}

Synchronized::~Synchronized()
{
#    ifdef POSIX_THREADS
    int result = 0;

    result = pthread_cond_destroy(&cond);
    if (result)
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
        LOG("Synchronized cond_destroy failed with (result)(ptr)");
        LOG(result);
        LOG((unsigned long)this);
        LOG_END;
    }

#        ifdef NO_FAST_MUTEXES
    {
        // if another thread owns the mutex, let's wait ...
        if (lock(1))
        {
            if (pthread_mutex_unlock(&monitor) == 0)
            {
                result = pthread_mutex_destroy(&monitor);
                assert(result == 0);
                return;
            }
        }
        assert(false);
        return; // NOTE: We give it up! CK
    }
#        endif

    result = pthread_mutex_destroy(&monitor);
    if (result)
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
        LOG("Synchronized mutex_destroy failed with (result)(ptr)");
        LOG(result);
        LOG((unsigned long)this);
        LOG_END;
    }
#    else
#        ifdef WIN32
    CloseHandle(semEvent);
    CloseHandle(semMutex);
#        endif
#    endif

    isLocked = false;
}

void Synchronized::wait()
{
#    ifdef POSIX_THREADS
    cond_timed_wait(nullptr);
#    else
#        ifdef WIN32
    wait(INFINITE);
#        endif
#    endif
}

#    ifdef POSIX_THREADS
int Synchronized::cond_timed_wait(const struct timespec* ts)
{
    int result = 0;
    isLocked   = false;
    if (ts)
        result = pthread_cond_timedwait(&cond, &monitor, ts);
    else
        result = pthread_cond_wait(&cond, &monitor);
    isLocked = true;
    return result;
}
#    endif

bool Synchronized::wait(long timeout)
{
    bool timeoutOccurred = false;
#    ifdef POSIX_THREADS
    struct timespec ts = {};
#        ifdef HAVE_CLOCK_GETTIME
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += (time_t)timeout / 1000;
    int const millis = ts.tv_nsec / 1000000 + (timeout % 1000);
    if (millis >= 1000) { ts.tv_sec += 1; }
    ts.tv_nsec = (millis % 1000) * 1000000;
#        else
    struct timeval tv = {};
    gettimeofday(&tv, 0);
    ts.tv_sec  = tv.tv_sec + (time_t)timeout / 1000;
    int millis = tv.tv_usec / 1000 + (timeout % 1000);
    if (millis >= 1000) { ts.tv_sec += 1; }
    ts.tv_nsec = (millis % 1000) * 1000000;
#        endif

    int err  = 0;
    isLocked = false;
    if ((err = cond_timed_wait(&ts)) > 0)
    {
        switch (err)
        {
        case EINVAL:
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
            LOG("Synchronized: wait with timeout returned (error)");
            LOG(err);
            LOG_END;
        case ETIMEDOUT: timeoutOccurred = true; break;
        default:
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
            LOG("Synchronized: wait with timeout returned (error)");
            LOG(err);
            LOG_END;
            break;
        }
    }
#    else
#        ifdef WIN32
    isLocked = false;
    if (!ReleaseMutex(semMutex))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
        LOG("Synchronized: releasing mutex failed");
        LOG_END;
    }
    int err;
    err = WaitForSingleObject(semEvent, timeout);
    switch (err)
    {
    case WAIT_TIMEOUT:
        LOG_BEGIN(loggerModuleName, EVENT_LOG | 8);
        LOG("Synchronized: timeout on wait");
        LOG_END;
        timeoutOccurred = true;
        break;
    case WAIT_ABANDONED:
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 2);
        LOG("Synchronized: waiting for event failed");
        LOG_END;
    }
    if (WaitForSingleObject(semMutex, INFINITE) != WAIT_OBJECT_0)
    {
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 8);
        LOG("Synchronized: waiting for mutex failed");
        LOG_END;
    }
#        endif
#    endif
    isLocked = true;
    return timeoutOccurred;
}

void Synchronized::notify()
{
#    ifdef POSIX_THREADS
    int result = 0;
    result     = pthread_cond_signal(&cond);
    if (result)
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("Synchronized: notify failed (result)");
        LOG(result);
        LOG_END;
    }
#    else
#        ifdef WIN32
    numNotifies = 1;
    if (!SetEvent(semEvent))
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("Synchronized: notify failed");
        LOG_END;
    }
#        endif
#    endif
}

void Synchronized::notify_all()
{
#    ifdef POSIX_THREADS
    int result = 0;
    result     = pthread_cond_broadcast(&cond);
    if (result)
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("Synchronized: notify_all failed (result)");
        LOG(result);
        LOG_END;
    }
#    else
#        ifdef WIN32
    numNotifies = (char)0x80;
    while (numNotifies--)
        if (!SetEvent(semEvent))
        {
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
            LOG("Synchronized: notify failed");
            LOG_END;
        }
#        endif
#    endif
}

bool Synchronized::lock()
{
#    ifdef POSIX_THREADS
    int const err = pthread_mutex_lock(&monitor);
#        ifndef AGENTPP_PTHREAD_RECURSIVE
    if (!err)
    {
        isLocked = true;
        return true;
    }
    else if (err == EDEADLK)
    {
        // This thread owns already the lock, but
        // we do not like recursive locking and print a warning!
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
        LOG("Synchronized: recursive locking detected (id)!");
        LOG(id);
        LOG_END;
        return true;
    }
#        else
    if (!err)
    {

        if (isLocked)
        {

#            if 0
            // This thread owns already the lock, but
            // we do not like recursive locking. Thus
            // release it immediately and print a warning!
            if (pthread_mutex_unlock(&monitor) != 0)
            {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 0);
                LOG("Synchronized: unlock failed on recursive lock (id)");
                LOG(id);
                LOG_END;
            }
            else
            {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
                LOG("Synchronized: recursive locking detected (id)!");
                LOG(id);
                LOG_END;
            }
#            endif
        }
        else
        {
            isLocked = true;
            // no logging because otherwise deep (virtual endless) recursion
        }

        return true;
    }
#        endif
    else
    {
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
        LOG("Synchronized: lock failed (id)(err)");
        LOG(id);
        LOG(err);
        LOG_END;
        return false;
    }
#    else
#        ifdef WIN32
    if (WaitForSingleObject(semMutex, INFINITE) != WAIT_OBJECT_0)
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("Synchronized: lock failed");
        LOG_END;
        return false;
    }
    if (isLocked)
    {
        // This thread owns already the lock, but
        // we do not like recursive locking. Thus
        // release it immediately and print a warning!
        if (!ReleaseMutex(semMutex))
        {
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
            LOG("Synchronized: unlock failed (id)");
            LOG(id);
            LOG_END;
            return false;
        }
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
        LOG("Synchronized: recursive locking detected (id)!");
        LOG(id);
        LOG_END;
    }
    isLocked = true;
    return true;
#        endif
#    endif
}

bool Synchronized::lock(long timeout)
{

#    ifdef POSIX_THREADS
    struct timespec ts = {};
#        ifdef HAVE_CLOCK_GETTIME
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += (time_t)timeout / 1000;
    int const millis = ts.tv_nsec / 1000000 + (timeout % 1000);
    if (millis >= 1000) { ts.tv_sec += 1; }
    ts.tv_nsec = (millis % 1000) * 1000000;
#        else
    struct timeval tv = {};
    gettimeofday(&tv, 0);
    ts.tv_sec  = tv.tv_sec + (time_t)timeout / 1000;
    int millis = tv.tv_usec / 1000 + (timeout % 1000);
    if (millis >= 1000) { ts.tv_sec += 1; }
    ts.tv_nsec            = (millis % 1000) * 1000000;
#        endif

    int error = 0;
#        ifdef HAVE_PTHREAD_MUTEX_TIMEDLOCK
    if ((error = pthread_mutex_timedlock(&monitor, &ts)) == 0)
    {
#        else
    long remaining_millis = timeout;
    do {
        error = pthread_mutex_trylock(&monitor);
        if (error == EBUSY)
        {
            Thread::sleep(10);
            remaining_millis -= 10;
        }
    } while (error == EBUSY && (remaining_millis > 0));
    if (error == 0)
    {
#        endif
#        ifndef AGENTPP_PTHREAD_RECURSIVE
        isLocked = true;
        return true;
#        else
        if (isLocked)
        {

#            if 0
            // This thread owns already the lock, but
            // we do not like recursive locking. Thus
            // release it immediately and print a warning!
            if (pthread_mutex_unlock(&monitor) != 0)
            {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 0);
                LOG("Synchronized: unlock failed on recursive lock (id)");
                LOG(id);
                LOG_END;
            }
            else
            {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
                LOG("Synchronized: recursive locking detected (id)!");
                LOG(id);
                LOG_END;
            }
#            endif
        }
        else
        {
            isLocked = true;
            // no logging because otherwise deep (virtual endless) recursion
        }
        return true;
#        endif
    }
    else
    {
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
        LOG("Synchronized: lock failed (id)(error)");
        LOG(id);
        LOG(error);
        LOG_END;
        return false;
    }
#    else
#        ifdef WIN32
    if (WaitForSingleObject(semMutex, timeout) != WAIT_OBJECT_0)
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("Synchronized: lock failed");
        LOG_END;
        return false;
    }
    if (isLocked)
    {
        // This thread owns already the lock, but
        // we do not like recursive locking. Thus
        // release it immediately and print a warning!
        if (!ReleaseMutex(semMutex))
        {
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
            LOG("Synchronized: unlock failed (id)");
            LOG(id);
            LOG_END;
            return false;
        }
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
        LOG("Synchronized: recursive locking detected (id)!");
        LOG(id);
        LOG_END;
    }
    isLocked = true;
    return true;
#        endif
#    endif
}

bool Synchronized::unlock()
{
    bool const wasLocked = isLocked;
    isLocked             = false;
#    ifdef POSIX_THREADS
    int const err = pthread_mutex_unlock(&monitor);
    if (err != 0)
    {
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
        LOG("Synchronized: unlock failed (id)(error)(wasLocked)");
        LOG(id);
        LOG(err);
        LOG(wasLocked);
        LOG_END;
        isLocked = wasLocked;
        return false;
    }
#    else
#        ifdef WIN32
    if (!ReleaseMutex(semMutex))
    {
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
        LOG("Synchronized: unlock failed (id)(isLocked)(wasLocked)");
        LOG(id);
        LOG(isLocked);
        LOG(wasLocked);
        LOG_END;
        isLocked = wasLocked;
        return false;
    }
#        endif
#    endif
    return true;
}

Synchronized::TryLockResult Synchronized::trylock()
{
#    ifdef POSIX_THREADS
#        ifndef AGENTPP_PTHREAD_RECURSIVE
    int const err = pthread_mutex_trylock(&monitor);
    if (!err)
    {
        isLocked = true;
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
        LOG("Synchronized: try lock success (id)(ptr)");
        LOG(id);
        LOG((long)this);
        LOG_END;
        return LOCKED;
    }
    else if (err == EDEADLK)
    {
        // This thread owns already the lock, but
        // we do not like recursive locking and print a warning!
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
        LOG("Synchronized: recursive try locking detected (id)(ptr)!");
        LOG(id);
        LOG((long)this);
        LOG_END;
        return OWNED;
    }
#        else
    if (pthread_mutex_trylock(&monitor) == 0)
    {
        if (isLocked)
        {

#            if 0
            // This thread owns already the lock, but
            // we do not like true recursive locking. Thus
            // release it immediately and print a warning!
            if (pthread_mutex_unlock(&monitor) != 0)
            {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 0);
                LOG("Synchronized: unlock failed on recursive try lock "
                    "(id)(ptr)");
                LOG(id);
                LOG((long)this);
                LOG_END;
            }
            else
            {
                LOG_BEGIN(loggerModuleName, WARNING_LOG | 5);
                LOG("Synchronized: recursive try locking detected (id)(ptr)!");
                LOG(id);
                LOG((long)this);
                LOG_END;
            }
#            endif

            return OWNED;
        }
        else
        {
            isLocked = true;
            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
            LOG("Synchronized: try lock success (id)(ptr)");
            LOG(id);
            LOG((long)this);
            LOG_END;
        }
        return LOCKED;
    }
#        endif
    else
    {
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
        LOG("Synchronized: try lock busy (id)(ptr)");
        LOG(id);
        LOG((long)this);
        LOG_END;
        return BUSY;
    }
#    else
#        ifdef WIN32
    int status = WaitForSingleObject(semMutex, 0);
    if (status != WAIT_OBJECT_0)
    {
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
        LOG("Synchronized: try lock failed (id)");
        LOG(id);
        LOG_END;
        return BUSY;
    }
    if (isLocked)
    {
        if (!ReleaseMutex(semMutex))
        {
            LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
            LOG("Synchronized: unlock failed (id)");
            LOG(id);
            LOG_END;
        }
        return OWNED;
    }
    else
    {
        isLocked = true;
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
        LOG("Synchronized: try lock success (id)");
        LOG(id);
        LOG_END;
    }
    return LOCKED;
#        endif
#    endif
}

/*------------------------ class Thread ----------------------------*/

ThreadList Thread::threadList;

#    ifdef POSIX_THREADS
void* thread_starter(void* t)
{
    auto* thread = (Thread*)t;
    Thread::threadList.add(thread);

#        ifndef NO_FAST_MUTEXES
    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
    LOG("Thread: started (tid)");
    // XXX LOG((AGENTPP_OPAQUE_PTHREAD_T)(thread->tid));
    LOG_END;
#        endif

    thread->get_runnable().run();

#        ifndef NO_FAST_MUTEXES
    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
    LOG("Thread: ended (tid)");
    // XXX LOG((AGENTPP_OPAQUE_PTHREAD_T)(thread->tid));
    LOG_END;
#        endif
    Thread::threadList.remove(thread);
    thread->status = Thread::FINISHED;

    return t;
}
#    else
#        ifdef WIN32
DWORD thread_starter(LPDWORD lpdwParam)
{
    Thread* thread = (Thread*)lpdwParam;
    Thread::threadList.add(thread);

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
    LOG("Thread: started (tid)");
    LOG(thread->tid);
    LOG_END;

    thread->get_runnable().run();

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
    LOG("Thread: ended (tid)");
    LOG(thread->tid);
    LOG_END;

    Thread::threadList.remove(thread);
    thread->status = Thread::FINISHED;

    ::SetEvent(thread->threadEndEvent);

    return 0;
}
#        endif
#    endif

Thread::Thread()
{
    stackSize = AGENTPP_DEFAULT_STACKSIZE;
    runnable  = (Runnable*)this;
    status    = IDLE;
#    ifdef WIN32
    threadHandle   = INVALID_HANDLE_VALUE;
    threadEndEvent = ::CreateEvent(NULL, true, false, NULL);
#    endif // WIN32
}

Thread::Thread(Runnable& r)
{
    stackSize = AGENTPP_DEFAULT_STACKSIZE;
    runnable  = &r;
    status    = IDLE;
#    ifdef WIN32
    threadHandle   = INVALID_HANDLE_VALUE;
    threadEndEvent = ::CreateEvent(NULL, true, false, NULL);
#    endif // WIN32
}

void Thread::run()
{
    LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
    LOG("Thread: empty run method!");
    LOG_END;
}

Thread::~Thread()
{
    if (status != IDLE) { join(); }
#    ifdef WIN32
    ::CloseHandle(threadEndEvent);
    if (threadHandle != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(threadHandle);
        threadHandle = INVALID_HANDLE_VALUE;
    }
#    endif
}

Runnable& Thread::get_runnable() { return *runnable; }

void Thread::join()
{
#    ifdef POSIX_THREADS
    if (status)
    {
        void*     retstat = nullptr;
        int const err     = pthread_join(tid, &retstat);
        if (err)
        {
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
            LOG("Thread: join failed (error)");
            LOG(err);
            LOG_END;
        }
        status = IDLE;
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
        LOG("Thread: joined thread successfully (tid)");
        // XXX LOG((AGENTPP_OPAQUE_PTHREAD_T)tid); // FIXME: OSX error: cast from pointer to smaller
        // type
        //  'int' loses information
        LOG_END;
    }
    else
    {
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
        LOG("Thread: thread not running (tid)");
        // XXX LOG((AGENTPP_OPAQUE_PTHREAD_T)tid); // FIXME: OSX error: cast from pointer to smaller
        // type
        //  'int' loses information
        LOG_END;
    }
#    else
#        ifdef WIN32
    if (status)
    {
        if (WaitForSingleObject(threadEndEvent, INFINITE) != WAIT_OBJECT_0)
        {
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
            LOG("Thread: join failed");
            LOG_END;
        }
        status = IDLE;
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 4);
        LOG("Thread: joined thread successfully");
        LOG_END;
    }
    else
    {
        LOG_BEGIN(loggerModuleName, WARNING_LOG | 1);
        LOG("Thread: thread not running");
        LOG_END;
    }
#        endif
#    endif
}

void Thread::start()
{
#    ifdef POSIX_THREADS
    if (status == IDLE)
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, stackSize);
        int const err = pthread_create(&tid, &attr, thread_starter, this);
        if (err)
        {
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
            LOG("Thread: cannot start thread (error)");
            LOG(err);
            LOG_END;
            status = IDLE;
        }
        else
            status = RUNNING;
        pthread_attr_destroy(&attr);
    }
    else
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("Thread: thread already running!");
        LOG_END;
    }
#    else
#        ifdef WIN32
    DWORD* targ = (DWORD*)this;

    if (status == IDLE)
    {

        if (threadHandle != INVALID_HANDLE_VALUE)
        {
            ::CloseHandle(threadHandle);
            threadHandle = INVALID_HANDLE_VALUE;
        }

        threadHandle = CreateThread(0, // no security attributes
            stackSize, (LPTHREAD_START_ROUTINE)thread_starter, targ, 0, &tid);

        if (threadHandle == 0)
        {
            LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
            LOG("Thread: cannot start thread");
            LOG_END;
            status = IDLE;
        }
        else
        {
            status = RUNNING;
        }
    }
    else
    {
        LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
        LOG("Thread: thread already running!");
        LOG_END;
    }
#        endif
#    endif
}

void Thread::sleep(long millis)
{
#    ifdef WIN32
    Sleep(millis);
#    else
    nsleep((int)(millis / 1000), (millis % 1000) * 1000000);
#    endif
}

void Thread::sleep(long millis, int nanos)
{
#    ifdef WIN32
    sleep(millis);
#    else
    nsleep((int)(millis / 1000), (millis % 1000) * 1000000 + nanos);
#    endif
}

void Thread::nsleep(int secs, long nanos)
{
#    ifdef WIN32
    DWORD millis = secs * 1000 + nanos / 1000000;
    Sleep(millis);
#    else
    long const s = secs + nanos / 1000000000;
    long const n = nanos % 1000000000;

#        ifdef _POSIX_TIMERS
    struct timespec interval = {}, remainder = {};
    interval.tv_sec = s;
    interval.tv_nsec = n;
    if (nanosleep(&interval, &remainder) == -1)
    {
        if (errno == EINTR)
        {
            LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
            LOG("Thread: sleep interrupted");
            LOG_END;
        }
    }
#        else
    struct timeval interval = {};
    interval.tv_sec         = s;
    interval.tv_usec        = n / 1000;
    fd_set writefds, readfds, exceptfds;
    FD_ZERO(&writefds);  // NOLINT(clang-analyzer-security.insecureAPI.bzero)
    FD_ZERO(&readfds);   // NOLINT(clang-analyzer-security.insecureAPI.bzero)
    FD_ZERO(&exceptfds); // NOLINT(clang-analyzer-security.insecureAPI.bzero)
    if (select(0, &writefds, &readfds, &exceptfds, &interval) == -1)
    {
        if (errno == EINTR)
        {
            LOG_BEGIN(loggerModuleName, EVENT_LOG | 3);
            LOG("Thread: sleep interrupted");
            LOG_END;
        }
    }
#        endif
#    endif
}

#    ifdef AGENTPP_USE_THREAD_POOL

/*--------------------- class TaskManager --------------------------*/

TaskManager::TaskManager(ThreadPool* tp, int stackSize) : thread(*this)
{
    threadPool = tp;
    task       = nullptr;
    go         = true;
    thread.set_stack_size(stackSize);
    thread.start();
    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
    LOG("TaskManager: thread started");
    LOG_END;
}

TaskManager::~TaskManager()
{
    lock();
    go = false;
    notify();
    unlock();
    thread.join();
    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
    LOG("TaskManager: thread stopped");
    LOG_END;
}

void TaskManager::run()
{
    lock();
    while (go)
    {
        if (task)
        {
            task->run();
            delete task;
            task = nullptr;
            unlock();
            if (threadPool->is_one_time_execution()) { return; }
            threadPool->idle_notification();
            lock();
        }
        else
        {
            wait();
        }
    }
    if (task)
    {
        delete task;
        task = nullptr;
    }
    unlock();
}

bool TaskManager::set_task(Runnable* t)
{
    lock();
    if (!task)
    {
        task = t;
        notify();
        unlock();
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
        LOG("TaskManager: after notify");
        LOG_END;
        return true;
    }
    else
    {
        unlock();
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
        LOG("TaskManager: got already a task");
        LOG_END;
        return false;
    }
}

/*--------------------- class ThreadPool --------------------------*/

void ThreadPool::execute(Runnable* t)
{
    lock();
    TaskManager* tm = nullptr;
    while (!tm)
    {
        ArrayCursor<TaskManager> cur;
        for (cur.init(&taskList); cur.get(); cur.next())
        {
            tm = cur.get();
            if (tm->is_idle())
            {
                LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
                LOG("TaskManager: task manager found");
                LOG_END;

                unlock();
                if (tm->set_task(t)) { return; }
                else
                {
                    // task could not be assigned
                    tm = nullptr;
                    lock();
                }
            }
            tm = nullptr;
        }
        if (!tm) wait(1000);
    }
    unlock();
}

bool ThreadPool::is_idle()
{
    lock();
    ArrayCursor<TaskManager> cur;
    for (cur.init(&taskList); cur.get(); cur.next())
    {
        if (!cur.get()->is_idle())
        {
            unlock();
            return false;
        }
    }
    unlock();
    return true;
}

bool ThreadPool::is_busy()
{
    lock();
    ArrayCursor<TaskManager> cur;
    for (cur.init(&taskList); cur.get(); cur.next())
    {
        if (cur.get()->is_busy())
        {
            unlock();
            return true;
        }
    }
    unlock();
    return false;
}

void ThreadPool::terminate()
{
    lock();
    ArrayCursor<TaskManager> cur;
    for (cur.init(&taskList); cur.get(); cur.next())
    {
        cur.get()->stop();
        cur.get()->notify();
    }
    notify();
    unlock();
}

void ThreadPool::join()
{
    Array<TaskManager> joined;
    lock();
    ArrayCursor<TaskManager> cur;
    for (cur.init(&taskList); cur.get(); cur.next())
    {
        TaskManager* joining = cur.get();
        if (joined.index(joining) < 0)
        {
            unlock();
            joining->join();
            joined.add(joining);
            lock();
        }
    }
    unlock();
    joined.clear();
}

ThreadPool::ThreadPool(int size) : oneTimeExecution(false)
{
    for (int i = 0; i < size; i++) { taskList.add(new TaskManager(this)); }
}

ThreadPool::ThreadPool(int size, int stack_size) : oneTimeExecution(false)
{
    stackSize = stack_size;
    for (int i = 0; i < size; i++) { taskList.add(new TaskManager(this, stackSize)); }
}

ThreadPool::~ThreadPool()
{
    terminate();
    join();
}

/*--------------------- class QueuedThreadPool --------------------------*/

QueuedThreadPool::QueuedThreadPool(int size) : ThreadPool(size) { }

QueuedThreadPool::QueuedThreadPool(int size, int stack_size) : ThreadPool(size, stack_size) { }

QueuedThreadPool::~QueuedThreadPool()
{
    go = false;
    Thread::lock();
    Thread::notify_all();
    Thread::unlock();
    Thread::join();
}

void QueuedThreadPool::assign(Runnable* t)
{
    TaskManager*             tm = nullptr;
    ArrayCursor<TaskManager> cur;
    for (cur.init(&taskList); cur.get(); cur.next())
    {
        tm = cur.get();
        if (tm->is_idle())
        {
            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
            LOG("TaskManager: task manager found");
            LOG_END;
            Thread::unlock();
            if (!tm->set_task(t))
            {
                tm = nullptr;
                Thread::lock();
            }
            else
            {
                Thread::lock();
                break;
            }
        }
        tm = nullptr;
    }
    if (!tm)
    {
        queue.add(t);
        Thread::notify();
    }
}

void QueuedThreadPool::execute(Runnable* t)
{
    Thread::lock();
    if (queue.empty()) { assign(t); }
    else
    {
        queue.add(t);
    }
    Thread::unlock();
}

void QueuedThreadPool::run()
{
    go = true;
    Thread::lock();
    while (go)
    {
        Runnable* t = queue.removeFirst();
        if (t) { assign(t); }
        Thread::wait(1000);
    }
    Thread::unlock();
}

unsigned int QueuedThreadPool::queue_length()
{
    Thread::lock();
    int const length = queue.size();
    Thread::unlock();
    return length;
}

void QueuedThreadPool::idle_notification()
{
    Thread::lock();
    Thread::notify();
    Thread::unlock();
    ThreadPool::idle_notification();
}

void MibTask::run() { (task->called_class->*task->method)(task->req); }

#        ifdef NO_FAST_MUTEXES

LockRequest::LockRequest(Synchronized* s)
{
    target        = s;
    waitForLock   = true;
    tryLockResult = BUSY;
    lock();
}

LockRequest::~LockRequest() { unlock(); }

LockQueue::LockQueue()
{
    go = true;
    start();
}

LockQueue::~LockQueue()
{
    go = false;
    lock();
    // wake up queue
    notify();
    unlock();

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
    LOG("LockQueue: end queue");
    LOG_END;

    // join thread here, before pending list is deleted
    if (is_alive()) join();

    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 1);
    LOG("LockQueue: queue stopped");
    LOG_END;

    pendingRelease.clear();
    pendingLock.clear();
}

void LockQueue::run()
{
    lock();
    while ((!pendingLock.empty()) || (!pendingRelease.empty()) || (go))
    {
        while (!pendingRelease.empty())
        {
            LockRequest* r = pendingRelease.removeFirst();
            r->target->unlock();
            r->lock();
            r->notify();
            r->unlock();
            LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
            LOG("LockQueue: unlocked (ptr)");
            LOG((long)r->target);
            LOG_END;
        }
        int const pl      = pendingLock.size();
        int       pending = pl;
        for (int i = 0; i < pl; i++)
        {
            LockRequest* r = pendingLock.removeFirst();
            // Only if target is not locked at all - also not by
            // this lock queue - then inform requester:
            Synchronized::TryLockResult tryLockResult = Synchronized::BUSY;
            if ((tryLockResult = r->target->trylock()) == LOCKED)
            {
                LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
                LOG("LockQueue: lock (ptr)(pending)");
                LOG((long)r->target);
                LOG(pending);
                LOG_END;
                r->tryLockResult = LOCKED;
                r->lock();
                r->notify();
                r->unlock();
                pending--;
            }
            else if (!r->waitForLock)
            {
                LOG_BEGIN(loggerModuleName, DEBUG_LOG | 8);
                LOG("LockQueue: trylock (ptr)(pending)(result)");
                LOG((long)r->target);
                LOG(pending);
                LOG(tryLockResult == LOCKED ? "locked" : tryLockResult == BUSY ? "busy" : "owned");
                LOG_END;
                r->tryLockResult = tryLockResult;
                r->lock();
                r->notify();
                r->unlock();
                pending--;
            }
            else
            {
                pendingLock.addLast(r);
            }
        }
        LOG_BEGIN(loggerModuleName, DEBUG_LOG | 9);
        LOG("LockQueue: waiting for next event (pending)");
        LOG(pending);
        LOG_END;

        // do not wait forever because we cannot
        // be sure that all instrumentation code notifies
        // us correctly.
        wait(5000);
    }
    unlock();
    LOG_BEGIN(loggerModuleName, INFO_LOG | 2);
    LOG("LockQueue: stopped (pending locks)(pending releases)(go)");
    LOG(pendingLock.size());
    LOG(pendingRelease.size());
    LOG(go);
    LOG_END;
}

void LockQueue::acquire(LockRequest* r)
{
    lock();
    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
    LOG("LockQueue: adding lock request (ptr)");
    LOG((long)r->target);
    LOG_END;
    pendingLock.addLast(r);
    notify();
    unlock();
}

void LockQueue::release(LockRequest* r)
{
    lock();
    LOG_BEGIN(loggerModuleName, DEBUG_LOG | 2);
    LOG("LockQueue: adding release request (ptr)");
    LOG((long)r->target);
    LOG_END;
    pendingRelease.addLast(r);
    notify();
    unlock();
}

#        endif // NO_FAST_MUTEXES

#    endif
#endif // _THREADS

#ifdef AGENTPP_NAMESPACE
}
#endif
