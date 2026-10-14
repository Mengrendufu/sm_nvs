#ifndef NVS_CRC_H_
#define NVS_CRC_H_

#include <stddef.h>
#include <stdint.h>

/* Bit-exact to Zephyr's subsys/crc (see the porting guide, section 8):
 * crc8_ccitt : poly 0x07, MSB-first, seed from the caller (NVS passes 0xFF)
 * crc32_ieee : poly 0xEDB88320 reflected, seed 0xFFFFFFFF, final xor. */
uint8_t crc8_ccitt(uint8_t seed, const void *buf, size_t cnt);
uint32_t crc32_ieee(const void *buf, size_t len);

#endif /* NVS_CRC_H_ */
