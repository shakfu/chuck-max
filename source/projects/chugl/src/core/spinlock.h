#pragma once

#include <atomic>
#include <thread>

// include immintrin.h for _mm_pause() on macos x64_64
#if defined(__x86_64__)
#include <immintrin.h>
#endif

// Spinlock implementation adapted from from
// https://rigtorp.se/spinlock/
// https://github.com/open-telemetry/opentelemetry-cpp/blob/main/api/include/opentelemetry/common/spin_lock_mutex.h
// https://github.com/open-telemetry/opentelemetry-cpp/pull/443

/*
Spinlock with NO backoff strategy. No yields or sleeps.

Can have decent performance in scenarios where there is low lock contention and
lock-holders achieve their work quickly.  It degrades in scenarios where locked
tasks take a long time.
*/

#define SPINLOCK_FAST_ITERATIONS 200

struct spinlock {
    std::atomic<bool> lock_ = { 0 };

    static void fast_yield() noexcept
    {
// Issue a Pause/Yield instruction while spinning.
#if defined(_MSC_VER)
        // YieldProcessor(); //  Microsoft dropped YieldProcessor with Windows 8.
        // Replace with _mm_pause()
        _mm_pause();
#elif defined(__i386__) || defined(__x86_64__)
#if defined(__clang__) || defined(__INTEL_COMPILER)
        _mm_pause();
#else
        __builtin_ia32_pause();
#endif
#elif defined(__arm__)
        __asm__ volatile("yield" ::: "memory");
#else
        // TODO: Issue PAGE/YIELD on other architectures.
#endif
    }

    static void lock(spinlock* spin) noexcept
    {
        // method 1
        for (;;) {
            // Optimistically assume the lock is free on the first try
            if (!spin->lock_.exchange(true, std::memory_order_acquire)) {
                return;
            }
            // Wait for lock to be released without generating cache misses
            while (spin->lock_.load(std::memory_order_relaxed)) {
                // Issue X86 PAUSE or ARM YIELD instruction to reduce
                // contention between hyper-threads
                fast_yield();
            }
        }

        // method 2 (with yield)
        // for (;;) {
        //     // Try once
        //     if (!spin->lock_.exchange(true, std::memory_order_acquire)) {
        //         return;
        //     }
        //     // Spin-Fast (goal ~20ns)
        //     for (std::size_t i = 0; i < SPINLOCK_FAST_ITERATIONS; ++i) {
        //         if (try_lock(spin)) return;
        //         fast_yield();
        //     }
        //     // Yield then try again (goal ~100ns)
        //     std::this_thread::yield();
        //     if (try_lock(spin)) return;
        // }
    }

    static bool try_lock(spinlock* spin) noexcept
    {
        // First do a relaxed load to check if lock is free in order to prevent
        // unnecessary cache misses if someone does while(!try_lock())
        return !spin->lock_.load(std::memory_order_relaxed)
               && !spin->lock_.exchange(true, std::memory_order_acquire);
    }

    static void unlock(spinlock* spin) noexcept
    {
        spin->lock_.store(false, std::memory_order_release);
    }
};
