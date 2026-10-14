/* Public API of sm_nvs: the same calls as Zephyr's kvss/nvs.h, with the
 * device model replaced by the one pointer nvs_port.h hides. */
#ifndef NVS_H_
#define NVS_H_

#include <sys/types.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "nvs_port.h"

struct nvs_fs {
    off_t offset;                 /* file system offset in flash */
    uint32_t ate_wra;                 /* allocation table write address */
    uint32_t data_wra;                /* data write address */
    uint32_t sector_size;             /* multiple of the erase granularity */
    uint16_t sector_count;
    bool ready;
    void *flash_device;
    const struct flash_parameters *flash_parameters;
#if CONFIG_NVS_LOOKUP_CACHE
    uint32_t lookup_cache[CONFIG_NVS_LOOKUP_CACHE_SIZE];
#endif
};

int nvs_mount(struct nvs_fs *fs);
int nvs_clear(struct nvs_fs *fs);
ssize_t nvs_write(struct nvs_fs *fs, uint16_t id, const void *data, size_t len);
int nvs_delete(struct nvs_fs *fs, uint16_t id);
ssize_t nvs_read(struct nvs_fs *fs, uint16_t id, void *data, size_t len);

#endif /* NVS_H_ */
