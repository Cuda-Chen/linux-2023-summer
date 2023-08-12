#include <stdint.h>
#include <stdio.h>

#define HHHH ((sz + mask) & ~mask)

static inline uintptr_t align_up(uintptr_t sz, size_t alignment)
{
    uintptr_t mask = alignment - 1;
    if ((alignment & mask) == 0) {  // mask is power of two
        return HHHH;
    }
    return (((sz + mask) / alignment) * alignment);
}

#define TEST_CASES_ITER \
    TEST_IMPL(120, 4)   \
    TEST_IMPL(121, 4)   \
    TEST_IMPL(122, 4)   \
    TEST_IMPL(123, 4)   \
    TEST_IMPL(64, 4)    \
    TEST_IMPL(512, 4)

int main()
{
#define TEST_IMPL(SZ, ALIGN)                 \
    {                                        \
        uintptr_t res = align_up(SZ, ALIGN); \
        printf("ans: %d\n", res);            \
        puts("---");                         \
    }

    TEST_CASES_ITER
#undef TEST_IMPL
}
