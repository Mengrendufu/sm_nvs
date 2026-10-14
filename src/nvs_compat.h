/* The handful of Zephyr macros nvs.c/nvs_priv.h use, spelled plainly. */
#ifndef NVS_COMPAT_H_
#define NVS_COMPAT_H_
#include <stdint.h>
#include <string.h>

#define __ASSERT_NO_MSG(x) do { (void)(x); } while (0)
#define __ASSERT(x, ...) do { (void)(x); } while (0)
/* File scope friendly: a unique tentative object per line. */
#define BUILD_ASSERT(cond, ...) \
    extern char nvs_build_assert_##__LINE__[(cond) ? 1 : -1]
#define __aligned(x) __attribute__((aligned(x)))
#define __unused __attribute__((unused))
#define likely(x) (x)
#define unlikely(x) (x)
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* Unit-test hooks upstream keeps in the source: plain static here. */
#ifndef ZTESTABLE_STATIC
#define ZTESTABLE_STATIC static
#endif
#ifndef ZTESTABLE
#define ZTESTABLE
#endif
#ifndef LOG_MODULE_DECLARE
#define LOG_MODULE_DECLARE(...)
#endif
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#define BIT(n) (1UL << (n))
static inline int nvs_is_pow2_(uint32_t x) { return (x != 0U) && ((x & (x - 1U)) == 0U); }
#ifndef is_power_of_two
#define is_power_of_two(x) nvs_is_pow2_((uint32_t)(x))
#endif
#endif
