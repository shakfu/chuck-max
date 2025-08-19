#pragma once
#include "core/macros.h"

void* reallocate(void* pointer, i64 oldSize, i64 newSize);

#define ALLOCATE_TYPE(type) (type*)reallocate(NULL, 0, sizeof(type))

#define ALLOCATE_COUNT(type, count) (type*)reallocate(NULL, 0, sizeof(type) * (count))

#define ALLOCATE_BYTES(type, bytes) (type*)reallocate(NULL, 0, (bytes))

#define GROW_CAPACITY(cap) ((cap) < 8 ? 8 : (cap) * 2)
#define GROW_ARRAY(type, arrayPtr, oldCap, newCap)                                     \
    (type*)reallocate(arrayPtr, (oldCap) * sizeof(type), (newCap) * sizeof(type))

#define FREE_ARRAY(type, arrayPtr, oldCap)                                             \
    reallocate(arrayPtr, (oldCap) * sizeof(type), 0);                                  \
    arrayPtr = NULL;

#define FREE_TYPE(type, ptr)                                                           \
    do {                                                                               \
        reallocate(ptr, sizeof(type), 0);                                              \
        ptr = NULL;                                                                    \
    } while (0)

#define FREE(ptr)                                                                      \
    do {                                                                               \
        reallocate((void*)ptr, 0, 0);                                                  \
        ptr = NULL;                                                                    \
    } while (0)

// ============================================================================
// Arena Allocator
// ============================================================================

struct Arena {
    u8* base;
    u64 curr; // current pointer offset
    u64 cap;  // capacity in bytes

    static void init(Arena* a, u64 cap);
    static void* top(Arena* a);
    static void* push(Arena* a, u64 size);
    static void* pushZero(Arena* a, u64 size);
    static void pop(Arena* a, u64 size);
    static void popZero(Arena* a, u64 size);
    static void* get(Arena* a, u64 offset);
    static void clear(Arena* a);
    static void clearZero(Arena* a);
    static void free(Arena* a);
    static bool containsItem(Arena* a, void* ptr, size_t size);
    static u64 offsetOf(Arena* a, void* ptr);
    static void* findItem(Arena* a, void* ptr, size_t size);
};

#define ARENA_PUSH_TYPE(a, type) (type*)Arena::push(a, sizeof(type))
#define ARENA_PUSH_ZERO_TYPE(a, type) (type*)Arena::pushZero(a, sizeof(type))
#define ARENA_PUSH_COUNT(a, type, count) (type*)Arena::push(a, sizeof(type) * (count))
#define ARENA_PUSH_ZERO_COUNT(a, type, count)                                          \
    (type*)Arena::pushZero(a, sizeof(type) * (count))

// assuming arena all contiguous elements of type
#define ARENA_LENGTH(a, type) ((a)->curr / sizeof(type))
#define ARENA_CAP(a, type) ((a)->cap / sizeof(type))

#define ARENA_GET_TYPE(a, type, index) ((type*)Arena::get(a, (index) * sizeof(type)))

#define ARENA_GET_LAST_TYPE(a, type) ARENA_GET_TYPE(a, type, ARENA_LENGTH(a, type) - 1)

#define ARENA_POP_TYPE(a, type) Arena::pop(a, sizeof(type))
#define ARENA_POP_COUNT(a, type, count) Arena::pop(a, sizeof(type) * (count))

#define ARENA_SWAP_DELETE_DEC(a, type, i)                                              \
    {                                                                                  \
        *ARENA_GET_TYPE(a, type, i) = *ARENA_GET_LAST_TYPE(a, type);                   \
        ARENA_POP_TYPE(a, type);                                                       \
        i--;                                                                           \
    }

#define ARENA_SWAP_DELETE(a, type, i)                                                  \
    {                                                                                  \
        *ARENA_GET_TYPE(a, type, i) = *ARENA_GET_LAST_TYPE(a, type);                   \
        ARENA_POP_TYPE(a, type);                                                       \
    }

#define ARENA_CONTAINS(a, item) Arena::containsItem(a, &item, sizeof(item))
#define ARENA_FIND(a, item) Arena::findItem(a, &item, sizeof(item))
