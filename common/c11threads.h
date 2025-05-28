/*
c11threads

Authors:
  John Tsiombikas <nuclear@member.fsf.org> - original POSIX threads wrapper
  Oliver Old <oliver.old@outlook.com> - win32 implementation

I place this piece of code in the public domain. Feel free to use as you see
fit. I'd appreciate it if you keep my name at the top of the code somewhere, but
whatever.

Main project site: https://github.com/jtsiomb/c11threads
*/
#ifndef C11THREADS_H_
#define C11THREADS_H_

/* If you wish to use this with pthread-win32 (i.e. use the POSIX threads wrapper
 * instead of the native win32 API implementation of C11 threads), then just
 * define C11THREADS_PTHREAD_WIN32 before including this header file.
 */
#if defined(_WIN32) && !defined(C11THREADS_PTHREAD_WIN32)
#define C11THREADS_WIN32
#endif

/* If your compiler does not support the inline keyword, or supports it with
 * some different variation of prefix or suffix underscores, you can define
 * C11THREADS_INLINE before including this header file.
 */
#ifndef C11THREADS_INLINE
/* C99 compilers will have inline */
#if __STDC_VERSION__ >= 199901L
#define C11THREADS_INLINE inline
/* C++ has inline */
#elif defined(__cplusplus)
#define C11THREADS_INLINE inline
/* MSVC has inline from VS 2015 but supports __inline in older versions */
#elif defined(_MSC_VER)
#if _MSC_VER >= 1900
#define C11THREADS_INLINE inline
#else
#define C11THREADS_INLINE __inline
#endif
/* for every other case, just gamble on having __inline__, and let the user
 * define C11THREADS_INLINE if it breaks
 */
#else
#define C11THREADS_INLINE __inline__
#endif
#endif	/* !defined C11THREADS_INLINE */

#include <time.h>

#ifndef TIME_UTC
#define TIME_UTC 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*thrd_start_t)(void*);
typedef void (*tss_dtor_t)(void*);

enum {
	mtx_plain		= 0,
	mtx_recursive	= 1,
	mtx_timed		= 2,
};

enum {
	thrd_success,
	thrd_timedout,
	thrd_busy,
	thrd_error,
	thrd_nomem
};

#ifndef C11THREADS_WIN32
/* C11 threads over POSIX threads as thin static inline wrapper functions */
#include <stdint.h>
#include <errno.h>
#include <pthread.h>
#include <sched.h>	/* for sched_yield */
#include <sys/time.h>

#ifndef thread_local
#define thread_local		_Thread_local
#endif

#define ONCE_FLAG_INIT		PTHREAD_ONCE_INIT
#define TSS_DTOR_ITERATIONS	PTHREAD_DESTRUCTOR_ITERATIONS

#ifdef __APPLE__
/* Darwin doesn't implement timed mutexes currently */
#define C11THREADS_NO_TIMED_MUTEX
#include <Availability.h>
#ifndef __MAC_10_15
#define C11THREADS_NO_TIMESPEC_GET
#endif
#elif __STDC_VERSION__ < 201112L
#define C11THREADS_NO_TIMESPEC_GET
#endif

#ifdef C11THREADS_NO_TIMED_MUTEX
#define C11THREADS_TIMEDLOCK_POLL_INTERVAL 5000000	/* 5 ms */
#endif

/* types */
typedef pthread_t thrd_t;
typedef pthread_mutex_t mtx_t;
typedef pthread_cond_t cnd_t;
typedef pthread_key_t tss_t;
typedef pthread_once_t once_flag;

/* ---- thread management ---- */

static C11THREADS_INLINE int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	int res = pthread_create(thr, 0, (void*(*)(void*))func, arg);
	if(res == 0) {
		return thrd_success;
	}
	return res == ENOMEM ? thrd_nomem : thrd_error;
}

static C11THREADS_INLINE void thrd_exit(int res)
{
	pthread_exit((void*)(intptr_t)res);
}

static C11THREADS_INLINE int thrd_join(thrd_t thr, int *res)
{
	void *retval;

	if(pthread_join(thr, &retval) != 0) {
		return thrd_error;
	}
	if(res) {
		*res = (int)(intptr_t)retval;
	}
	return thrd_success;
}

static C11THREADS_INLINE int thrd_detach(thrd_t thr)
{
	return pthread_detach(thr) == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE thrd_t thrd_current(void)
{
	return pthread_self();
}

static C11THREADS_INLINE int thrd_equal(thrd_t a, thrd_t b)
{
	return pthread_equal(a, b);
}

static C11THREADS_INLINE int thrd_sleep(const struct timespec *ts_in, struct timespec *rem_out)
{
	if(nanosleep(ts_in, rem_out) < 0) {
		if(errno == EINTR) return -1;
		return -2;
	}
	return 0;
}

static C11THREADS_INLINE void thrd_yield(void)
{
	sched_yield();
}

/* ---- mutexes ---- */

static C11THREADS_INLINE int mtx_init(mtx_t *mtx, int type)
{
	int res;
	pthread_mutexattr_t attr;

	pthread_mutexattr_init(&attr);

	if(type & mtx_timed) {
#ifdef PTHREAD_MUTEX_TIMED_NP
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_TIMED_NP);
#else
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
#endif
	}
	if(type & mtx_recursive) {
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	}

	res = pthread_mutex_init(mtx, &attr) == 0 ? thrd_success : thrd_error;
	pthread_mutexattr_destroy(&attr);
	return res;
}

static C11THREADS_INLINE void mtx_destroy(mtx_t *mtx)
{
	pthread_mutex_destroy(mtx);
}

static C11THREADS_INLINE int mtx_lock(mtx_t *mtx)
{
	int res = pthread_mutex_lock(mtx);
	return res == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE int mtx_trylock(mtx_t *mtx)
{
	int res = pthread_mutex_trylock(mtx);
	if(res == EBUSY) {
		return thrd_busy;
	}
	return res == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE int mtx_timedlock(mtx_t *mtx, const struct timespec *ts)
{
	int res = 0;
#ifdef C11THREADS_NO_TIMED_MUTEX
	/* fake a timedlock by polling trylock in a loop and waiting for a bit */
	struct timeval now;
	struct timespec sleeptime;

	sleeptime.tv_sec = 0;
	sleeptime.tv_nsec = C11THREADS_TIMEDLOCK_POLL_INTERVAL;

	while((res = pthread_mutex_trylock(mtx)) == EBUSY) {
		gettimeofday(&now, NULL);

		if(now.tv_sec > ts->tv_sec || (now.tv_sec == ts->tv_sec &&
					(now.tv_usec * 1000) >= ts->tv_nsec)) {
			return thrd_timedout;
		}

		nanosleep(&sleeptime, NULL);
	}
#else
	if((res = pthread_mutex_timedlock(mtx, ts)) == ETIMEDOUT) {
		return thrd_timedout;
	}
#endif
	return res == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE int mtx_unlock(mtx_t *mtx)
{
	return pthread_mutex_unlock(mtx) == 0 ? thrd_success : thrd_error;
}

/* ---- condition variables ---- */

static C11THREADS_INLINE int cnd_init(cnd_t *cond)
{
	return pthread_cond_init(cond, 0) == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE void cnd_destroy(cnd_t *cond)
{
	pthread_cond_destroy(cond);
}

static C11THREADS_INLINE int cnd_signal(cnd_t *cond)
{
	return pthread_cond_signal(cond) == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE int cnd_broadcast(cnd_t *cond)
{
	return pthread_cond_broadcast(cond) == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE int cnd_wait(cnd_t *cond, mtx_t *mtx)
{
	return pthread_cond_wait(cond, mtx) == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE int cnd_timedwait(cnd_t *cond, mtx_t *mtx, const struct timespec *ts)
{
	int res;

	if((res = pthread_cond_timedwait(cond, mtx, ts)) != 0) {
		return res == ETIMEDOUT ? thrd_timedout : thrd_error;
	}
	return thrd_success;
}

/* ---- thread-specific data ---- */

static C11THREADS_INLINE int tss_create(tss_t *key, tss_dtor_t dtor)
{
	return pthread_key_create(key, dtor) == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE void tss_delete(tss_t key)
{
	pthread_key_delete(key);
}

static C11THREADS_INLINE int tss_set(tss_t key, void *val)
{
	return pthread_setspecific(key, val) == 0 ? thrd_success : thrd_error;
}

static C11THREADS_INLINE void *tss_get(tss_t key)
{
	return pthread_getspecific(key);
}

/* ---- misc ---- */

static C11THREADS_INLINE void call_once(once_flag *flag, void (*func)(void))
{
	pthread_once(flag, func);
}

#ifdef C11THREADS_NO_TIMESPEC_GET
static C11THREADS_INLINE int timespec_get(struct timespec *ts, int base)
{
	struct timeval tv;

	if(base != TIME_UTC) {
		return 0;
	}

	if(gettimeofday(&tv, 0) == -1) {
		return 0;
	}

	ts->tv_sec = tv.tv_sec;
	ts->tv_nsec = tv.tv_usec * 1000;
	return base;
}
#endif


#else	/* C11THREADS_WIN32 */

/* C11 threads implementation using native Win32 API calls (see c11threads_win32.c) */

#ifndef thread_local
#ifdef _MSC_VER
#define thread_local		__declspec(thread)
#else
#define thread_local		_Thread_local
#endif
#endif

#define ONCE_FLAG_INIT		{0}
#define TSS_DTOR_ITERATIONS	4

#ifndef _UCRT
#define C11THREADS_NO_TIMESPEC_GET
#endif

#ifdef _MSC_VER
#define C11THREADS_MSVC_NORETURN __declspec(noreturn)
#define C11THREADS_GNUC_NORETURN
#elif defined(__GNUC__)
#define C11THREADS_MSVC_NORETURN
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 5)
#define C11THREADS_GNUC_NORETURN __attribute__((noreturn))
#else
#define C11THREADS_GNUC_NORETURN
#endif
#else
#define C11THREADS_MSVC_NORETURN
#define C11THREADS_GNUC_NORETURN
#endif

/* types */
typedef unsigned long thrd_t;
typedef struct {
	void *debug_info;
	long lock_count;
	long recursion_count;
	void *owning_thread;
	void *lock_semaphore;
	void *spin_count;
} mtx_t;
typedef void *cnd_t;
typedef unsigned long tss_t;
typedef void *once_flag;
struct _c11threads_win32_timespec32_t {
	long tv_sec;
	long tv_nsec;
};
struct _c11threads_win32_timespec64_t {
#ifdef _MSC_VER
	__int64 tv_sec;
#else
	long long tv_sec;
#endif
	long tv_nsec;
};
#if !defined(_UCRT) && !defined(_TIMESPEC_DEFINED)
#ifdef _USE_32BIT_TIME_T
struct timespec {
	long tv_sec;
	long tv_nsec;
};
#elif !defined(_USE_32BIT_TIME_T)
struct timespec {
	__int64 tv_sec;
	long tv_nsec;
};
#endif	/* !defined(_USE_32BIT_TIME_T) */
#endif	/* !defined(_UCRT) && !defined(_TIMESPEC_DEFINED) */

/* Thread functions. */

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg);
/* Win32: Threads not created with thrd_create() need to call this to clean up TSS. */
C11THREADS_MSVC_NORETURN void thrd_exit(int res) C11THREADS_GNUC_NORETURN;
int thrd_join(thrd_t thr, int *res);
int thrd_detach(thrd_t thr);
thrd_t thrd_current(void);
int thrd_equal(thrd_t a, thrd_t b);
static C11THREADS_INLINE int thrd_sleep(const struct timespec *ts_in, struct timespec *rem_out);
void thrd_yield(void);

/* Mutex functions. */

int mtx_init(mtx_t *mtx, int type);
void mtx_destroy(mtx_t *mtx);
int mtx_lock(mtx_t *mtx);
int mtx_trylock(mtx_t *mtx);
static C11THREADS_INLINE int mtx_timedlock(mtx_t *mtx, const struct timespec *ts);
int mtx_unlock(mtx_t *mtx);

/* Condition variable functions. */

int cnd_init(cnd_t *cond);
void cnd_destroy(cnd_t *cond);
int cnd_signal(cnd_t *cond);
int cnd_broadcast(cnd_t *cond);
int cnd_wait(cnd_t *cond, mtx_t *mtx);
static C11THREADS_INLINE int cnd_timedwait(cnd_t *cond, mtx_t *mtx, const struct timespec *ts);

/* Thread-specific storage functions. */

int tss_create(tss_t *key, tss_dtor_t dtor);
void tss_delete(tss_t key);
int tss_set(tss_t key, void *val);
void *tss_get(tss_t key);

/* One-time callable function. */

void call_once(once_flag *flag, void (*func)(void));

#ifdef C11THREADS_NO_TIMESPEC_GET
static C11THREADS_INLINE int timespec_get(struct timespec *ts, int base);
#endif

/* Special Win32 functions. */
/* Win32: Free resources associated with this library. */
void c11threads_win32_destroy(void);
/* Win32: Register current Win32 thread in c11threads to allow for proper thrd_join(). */
int c11threads_win32_thrd_self_register(void);
/* Win32: Register Win32 thread by ID in c11threads to allow for proper thrd_join(). */
int c11threads_win32_thrd_register(unsigned long win32_thread_id);

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4127) /* Warning C4127: conditional expression is constant */
#endif

/* ---- thread management ---- */

int _c11threads_win32_thrd_sleep32(const struct _c11threads_win32_timespec32_t *ts_in, struct _c11threads_win32_timespec32_t *rem_out);
int _c11threads_win32_thrd_sleep64(const struct _c11threads_win32_timespec64_t *ts_in, struct _c11threads_win32_timespec64_t *rem_out);
static C11THREADS_INLINE int thrd_sleep(const struct timespec *ts_in, struct timespec *rem_out)
{
	if (sizeof(ts_in->tv_sec) == 4) {
		return _c11threads_win32_thrd_sleep32((const struct _c11threads_win32_timespec32_t*)ts_in, (struct _c11threads_win32_timespec32_t*)rem_out);
	} else {
		return _c11threads_win32_thrd_sleep64((const struct _c11threads_win32_timespec64_t*)ts_in, (struct _c11threads_win32_timespec64_t*)rem_out);
	}
}

/* ---- mutexes ---- */

int _c11threads_win32_mtx_timedlock32(mtx_t *mtx, const struct _c11threads_win32_timespec32_t *ts);
int _c11threads_win32_mtx_timedlock64(mtx_t *mtx, const struct _c11threads_win32_timespec64_t *ts);
static C11THREADS_INLINE int mtx_timedlock(mtx_t *mtx, const struct timespec *ts)
{
	if (sizeof(ts->tv_sec) == 4) {
		return _c11threads_win32_mtx_timedlock32(mtx, (const struct _c11threads_win32_timespec32_t*)ts);
	} else {
		return _c11threads_win32_mtx_timedlock64(mtx, (const struct _c11threads_win32_timespec64_t*)ts);
	}
}

/* ---- condition variables ---- */

int _c11threads_win32_cnd_timedwait32(cnd_t *cond, mtx_t *mtx, const struct _c11threads_win32_timespec32_t *ts);
int _c11threads_win32_cnd_timedwait64(cnd_t *cond, mtx_t *mtx, const struct _c11threads_win32_timespec64_t *ts);
static C11THREADS_INLINE int cnd_timedwait(cnd_t *cond, mtx_t *mtx, const struct timespec *ts)
{
	if (sizeof(ts->tv_sec) == 4) {
		return _c11threads_win32_cnd_timedwait32(cond, mtx, (const struct _c11threads_win32_timespec32_t*)ts);
	} else {
		return _c11threads_win32_cnd_timedwait64(cond, mtx, (const struct _c11threads_win32_timespec64_t*)ts);
	}
}

/* ---- misc ---- */

#ifdef C11THREADS_NO_TIMESPEC_GET
int _c11threads_win32_timespec32_get(struct _c11threads_win32_timespec32_t *ts, int base);
int _c11threads_win32_timespec64_get(struct _c11threads_win32_timespec64_t *ts, int base);
static C11THREADS_INLINE int timespec_get(struct timespec *ts, int base)
{
	if (sizeof(ts->tv_sec) == 4) {
		return _c11threads_win32_timespec32_get((struct _c11threads_win32_timespec32_t*)ts, base);
	} else {
		return _c11threads_win32_timespec64_get((struct _c11threads_win32_timespec64_t*)ts, base);
	}
}
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif	/* C11THREADS_WIN32 */

#ifdef __cplusplus
}
#endif

#endif	/* C11THREADS_H_ */
