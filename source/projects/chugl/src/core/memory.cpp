#include <stdlib.h>
#include <string.h>

#include "core/log.h"
#include "core/memory.h"
#include "memory.h"

void* reallocate(void* pointer, i64 oldSize, i64 newSize)
{
    // passing size = 0 to realloc will allocate a "minimum-sized"
    // object
    if (newSize == 0) {
        free(pointer); // no-op if pointer is NULL
        return NULL;
    }

    void* result = realloc(pointer, newSize);
    if (result == NULL) {
        log_error("Memory allocation failure. Unable to allocate %ld bytes", newSize);
#ifdef CHUGL_DEBUG
        exit(1); // out of memory
#endif
    }

    // zero out the new memory
    if (newSize > oldSize) {
        memset((u8*)result + oldSize, 0, newSize - oldSize);
    }

    return result;
}

// =================================================================================================
// Arena Allocator Definitions
// =================================================================================================

void Arena::init(Arena* a, u64 cap)
{
    ASSERT(a->base == NULL); // must not already be initialized

    a->base = ALLOCATE_BYTES(u8, cap);
    a->curr = 0;
    a->cap  = cap;
}

void* Arena::top(Arena* a)
{
    return a->base + a->curr;
}

void* Arena::push(Arena* a, u64 size)
{
    // reallocate more memory if needed
    if (a->curr + size > a->cap) {
        u64 oldCap = a->cap;
        a->cap     = MAX(GROW_CAPACITY(oldCap), a->curr + size);
        a->base    = GROW_ARRAY(u8, a->base, oldCap, a->cap);
    }

    void* result = a->base + a->curr;

    // advance the current offset
    a->curr += size;

    ASSERT(a->curr <= a->cap);

    return result;
}

void* Arena::pushZero(Arena* a, u64 size)
{
    void* result = push(a, size);
    memset(result, 0, size);
    return result;
}

void Arena::pop(Arena* a, u64 size)
{
    ASSERT(a->base != NULL); // must be initialized
    ASSERT(size <= a->curr); // cannot pop more than allocated

    size = MIN(size, a->curr);

    // move the current pointer back
    a->curr -= size;
}

void Arena::popZero(Arena* a, u64 size)
{
    Arena::pop(a, size);
    // zero out the memory
    memset(a->base + a->curr, 0, size);
}

void* Arena::get(Arena* a, u64 offset)
{
    ASSERT(offset <= a->curr); // cannot access beyond current pointer
    ASSERT(offset >= 0);       // cannot access before the base pointer
    ASSERT(a->curr <= a->cap); // current pointer must be within capacity
    return a->base + offset;
}

void Arena::clear(Arena* a)
{
    a->curr = 0;
}

void Arena::clearZero(Arena* a)
{
    u64 size = a->curr;
    memset(a->base, 0, size);
    a->curr = 0;
}

void Arena::free(Arena* a)
{
    FREE_ARRAY(u8, a->base, a->cap);
    a->base = NULL;
    a->curr = 0;
    a->cap  = 0;
}

u64 Arena::offsetOf(Arena* a, void* ptr)
{
    ASSERT(a->base != NULL);          // must be initialized
    ASSERT(ptr >= a->base);           // must be within the arena
    ASSERT(ptr <= a->base + a->curr); // must be within the arena

    return (u64)((u8*)ptr - a->base);
}

// Assumes arena is contiguous array of identical elements.
// Returns nullptr if not found. else returns the pointer to the item
void* Arena::findItem(Arena* a, void* ptr, size_t size)
{
    u8* comp = a->base;
    while (comp < a->base + a->curr) {
        if (memcmp(comp, ptr, size) == 0) {
            return comp;
        }
        comp += size;
    }
    return nullptr;
}

bool Arena::containsItem(Arena* a, void* ptr, size_t size)
{
    u8* comp = a->base;
    while (comp < a->base + a->curr) {
        if (memcmp(comp, ptr, size) == 0) {
            return true;
        }
        comp += size;
    }
    return false;
}