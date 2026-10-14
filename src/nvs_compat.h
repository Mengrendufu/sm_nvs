/* The handful of Zephyr macros nvs.c/nvs_priv.h use, spelled plainly. */
#ifndef NVS_COMPAT_H_
#define NVS_COMPAT_H_
#include <stdint.h>
#include <string.h>

#define __ASSERT_NO_MSG(x) do { (void)(x); } while (0)
#define __ASSERT(x, ...) do { (void)(x); } while (0)
#define BUILD_ASSERT(cond, ...) \
    extern char nvs_build_assert_##__LINE__[(cond) ? 1 : -1]
#define ZTESTABLE_STATIC static
#define ZTESTABLE
#define LOG_MODULE_DECLARE(...)
#define __aligned(x) __attribute__((aligned(x)))
#define __unused __attribute__((unused))
#define likely(x) (x)
#define unlikely(x) (x)
/* nvs_priv.h uses KB(64) for NVS_MAX_SECTOR_SIZE: without this the bound is
 * zero and every mount with a 4096-byte sector would be rejected. */
#ifndef KB
#define KB(x) ((x) * 1024)
#endif
#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
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
