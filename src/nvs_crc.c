#include "nvs_crc.h"

static const uint8_t crc8_tab[16] = {
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15,
    0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
};

uint8_t crc8_ccitt(uint8_t seed, const void *buf, size_t cnt) {
    const uint8_t *p = (const uint8_t *)buf;
    uint8_t val = seed;
    size_t i;

    for (i = 0; i < cnt; i++) {
        val ^= p[i];
        val = (uint8_t)((val << 4) ^ crc8_tab[val >> 4]);
        val = (uint8_t)((val << 4) ^ crc8_tab[val >> 4]);
    }
    return val;
}

static const uint32_t crc32_tab[16] = {
    0x00000000U, 0x1db71064U, 0x3b6e20c8U, 0x26d930acU,
    0x76dc4190U, 0x6b6b51f4U, 0x4db26158U, 0x5005713cU,
    0xedb88320U, 0xf00f9344U, 0xd6d6a3e8U, 0xcb61b38cU,
    0x9b64c2b0U, 0x86d3d2d4U, 0xa00ae278U, 0xbdbdf21cU,
};

uint32_t crc32_ieee(const void *buf, size_t len) {
    const uint8_t *data = (const uint8_t *)buf;
    uint32_t crc = 0xFFFFFFFFU;
    size_t i;

    for (i = 0; i < len; i++) {
        uint8_t byte = data[i];
        crc = (crc >> 4) ^ crc32_tab[(crc ^ byte) & 0x0fU];
        crc = (crc >> 4) ^ crc32_tab[(crc ^ (byte >> 4)) & 0x0fU];
    }
    return ~crc;
}
