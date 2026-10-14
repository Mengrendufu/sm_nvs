/* A W25Q128 with the embedded parts kept and the rest thrown away.
 * It enforces the two semantics that break silently in real life:
 *   - a page program never crosses a 256-byte page (the NVS core does not
 *     check; the driver must split - see src/nvs_flash_port.c)
 *   - a program only clears bits (1 -> 0); bytes must be erased to go back
 * Anything that violates either aborts the host test on purpose.
 */
#ifndef NOR_SIM_H_
#define NOR_SIM_H_

#include <stddef.h>
#include <stdint.h>

#define NOR_SIM_SIZE       (64U * 1024U)
#define NOR_SIM_SECTOR     4096U
#define NOR_SIM_PAGE       256U
#define NOR_SIM_ERASE_VAL  0xFFU

void NorSim_init(void);
int NorSim_read(uint32_t addr, void *buf, size_t len);
int NorSim_pageProgram(uint32_t addr, const void *buf, size_t len);
int NorSim_sectorErase(uint32_t addr);

/* Evidence counters: what the core actually asked the driver to do. */
unsigned NorSim_programs(void);
unsigned NorSim_erases(void);
uint32_t NorSim_bytesProgrammed(void);

#endif /* NOR_SIM_H_ */
