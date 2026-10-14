# sm_nvs — an independent NVS (key-value store) component

Extracted from Zephyr's `subsys/kvss/nvs` and carried as its own component, the
way this project's other third-party pieces are (sm_hsm, sm_sst, common_c).

## Layout

    include/    public API (nvs.h) and the platform contract (nvs_port.h)
    src/        the carried implementation (patched copy of vendor/nvs.c)
    vendor/     the upstream files, byte for byte, as fetched (see NOTICE)
    ports/      a template for the platform side (read/write/erase + crit)
    tests/      host tests with a RAM-backed fake flash

## Port contract (the only thing the core needs)

    bool     NvsPort_read (uint32_t off, void *buf, uint32_t len);
    bool     NvsPort_write(uint32_t off, const void *buf, uint32_t len);
    bool     NvsPort_erase(uint32_t off);        // one 4 KB sector
    uint32_t NvsPort_sectorSize(void);           // 4096 on a W25Q128
    void     NvsPort_critEnter(void);            // may be empty with one caller
    void     NvsPort_critExit(void);

The core must not include anything from Zephyr: no kernel.h, no device.h, no
LOG_*. Stripping that out is the job of src/ (see UPSTREAM in NOTICE): vendor/
stays as fetched so `diff -u vendor/nvs.c src/nvs.c` is the whole patch set.

## Target use (sm_blinky)

* W25Q128, 16 MB, 4 KB erasable sectors — sector_size 4096 maps 1:1 onto a
  device sector, which is what NVS wants.
* NVS runs inside the existing flash task, so "only the flash task touches the
  bus" stays true. Reads at mount are fine before the scheduler (the boot path
  already reads directly, see Asset_load).
* Erase is ~167 ms measured (datasheet: 30 ms typ, 200/400 ms max), so calls
  must never come from the UI thread.

## State

* [x] repository created, upstream fetched and pinned (NOTICE)
* [ ] src/: dependencies stripped, builds without Zephyr headers
* [ ] tests/: key-value round trip, GC wrap, and power-loss injection
* [ ] sm_blinky: submodule added, ports/nvs/ written, mount point chosen
