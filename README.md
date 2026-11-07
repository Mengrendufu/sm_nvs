# sm_nvs — an independent NVS (key-value store) component

Extracted from Zephyr's `subsys/kvss/nvs` and carried as its own component, the
way this project's other third-party pieces are (sm_hsm, sm_sst, common_c).

## Layout

    include/    public API (nvs.h) and the platform contract (nvs_port.h)
    src/        the carried implementation (patched copy of vendor/nvs.c)
    vendor/     the upstream files, byte for byte, as fetched (see NOTICE)
    ports/      a template for the platform side (read/write/erase + crit)
    tests/      host tests with a RAM-backed fake flash

## NVM orchestration contract

The shared NVM layer consists of exactly these files:

    include/nvm_manager.h   object-level Manager contract and backend vtable
    include/nvm_meta.h      caller-owned paired RAM block description
    src/nvm_meta.c          block binding and constructor preconditions

The application supplies `nvm_manager.c`, `nvm_conf.h` and concrete data
instances. This library does not supply a default Manager implementation,
registry, filesystem object, board partition or business data singleton.
The existing function signatures and object layout are unchanged.

Compile `src/nvm_meta.c` when using this layer. The application provides
`nvm_port.h` with `NVM_DEFINE_MODULE` and `NVM_REQUIRE`; RAM synchronization
used by its Manager and data instances also stays in that application port.
The low-level NVS API and its existing host build remain independent.

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
