/* The six functions of nvs_port.h, over whatever the platform gives us.
 *
 * On the host they sit on tests/nor_sim.c; on the board the very same file
 * sits on the flash task's RPC (Flash_readBlock / Flash_writeBlock /
 * Flash_eraseSector). Nothing above this file knows which one it is.
 *
 * The one thing that must never be dropped is the split loop in
 * flash_write(): NVS hands over arbitrary-length streams, and a W25Q128 page
 * program that crosses a page boundary wraps around and overwrites the start
 * of the page - silently. nor_sim.c aborts if that ever happens here.
 */
#include "nvs_port.h"
#include "nor_sim.h"

#define NOR_BASE        0U
#define NOR_ERASE_SIZE  NOR_SIM_SECTOR
#define NOR_PAGE_SIZE   NOR_SIM_PAGE
#define NOR_WBS         4U          /* see the porting guide, section 9 */

static const struct flash_parameters nvs_flash_par_ = {
    NOR_WBS,
    NOR_SIM_ERASE_VAL,
};

void *nvs_port_device(void) {
    return (void *)0;
}

int flash_read(void *dev, off_t off, void *buf, size_t len) {
    (void)dev;
    return NorSim_read(NOR_BASE + (uint32_t)off, buf, len);
}

int flash_write(void *dev, off_t off, const void *buf, size_t len) {
    const uint8_t *p = (const uint8_t *)buf;
    uint32_t addr = NOR_BASE + (uint32_t)off;

    (void)dev;

    if ((len == 0U) || ((len % NOR_WBS) != 0U)) {
        return -22;             /* -EINVAL: contract 2 */
    }

    while (len > 0U) {
        size_t chunk = NOR_PAGE_SIZE - (size_t)(addr % NOR_PAGE_SIZE);

        if (chunk > len) {
            chunk = len;
        }
        if (NorSim_pageProgram(addr, p, chunk) != 0) {
            return -5;          /* -EIO: contract 6 */
        }
        addr += (uint32_t)chunk;
        p += chunk;
        len -= chunk;
    }

    return 0;
}

int flash_flatten(void *dev, off_t off, size_t len) {
    size_t done;

    (void)dev;

    for (done = 0U; done < len; done += NOR_ERASE_SIZE) {
        if (NorSim_sectorErase(NOR_BASE + (uint32_t)off + (uint32_t)done) != 0) {
            return -5;
        }
    }

    return 0;
}

const struct flash_parameters *flash_get_parameters(void *dev) {
    (void)dev;
    return &nvs_flash_par_;
}

size_t flash_get_write_block_size(void *dev) {
    (void)dev;
    return NOR_WBS;
}

int flash_get_page_info_by_offs(void *dev, off_t off,
                                struct flash_pages_info *info) {
    (void)dev;

    info->start_offset = (off_t)((uint32_t)off & ~(NOR_ERASE_SIZE - 1U));
    info->size = NOR_ERASE_SIZE;
    info->index = (uint32_t)(info->start_offset / (off_t)NOR_ERASE_SIZE);
    return 0;
}
