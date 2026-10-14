/* Host tests for the ported NVS, over the simulated NOR.
 * What is proven here: the core builds and runs without Zephyr, the flash
 * semantics the core assumes are actually enforced (nor_sim.c aborts on a
 * page-crossing program or a 0->1 bit), the ATE CRCs are bit-exact, and the
 * log-structured store survives a full sector wrap with its live keys intact.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "nvs.h"
#include "nvs_crc.h"
#include "nor_sim.h"

static struct nvs_fs Fs;
static struct nvs_fs GcFs;
static uint8_t GcBuf[512];

static void crc_vectors(void) {
    static const uint8_t ate[7] = {0x01, 0x00, 0x00, 0x00,
                                   0x44, 0x00, 0xFF};
    assert(crc8_ccitt(0x00, "123456789", 9) == 0xF4);
    assert(crc8_ccitt(0xFF, "123456789", 9) == 0xFB);
    assert(crc32_ieee("123456789", 9) == 0xCBF43926U);
    assert(crc32_ieee("", 0) == 0U);
    assert(crc8_ccitt(0xFF, ate, 7) == 0x45);   /* a real ATE, per the guide */
    printf("  crc vectors .................. pass\n");
}

static void page_split(void) {
    /* The guide's section 7.3 table, split into what the contract allows
     * (lengths a multiple of write_block_size) and what it must refuse. */
    /* 10 cases of the guide's table: seven the contract accepts (with the
     * number of page programs each must become) and three it must refuse,
     * because the length is not a multiple of write_block_size. */
    static const struct { uint32_t off; size_t len; unsigned chunks; } ok[] = {
        {0, 256, 1}, {64, 64, 1}, {128, 128, 1}, {200, 112, 2}, {200, 300, 2},
        {0, 300, 2}, {1, 256, 2},
    };
    static const struct { uint32_t off; size_t len; } bad[] = {
        {1, 255}, {250, 1}, {255, 2},
    };
    uint8_t buf[300];
    unsigned i;

    memset(buf, 0x00, sizeof(buf));
    for (i = 0; i < sizeof(ok) / sizeof(ok[0]); i++) {
        unsigned const before = NorSim_programs();
        assert(flash_write(0, (off_t)(0x1000U + ok[i].off), buf,
                           ok[i].len) == 0);
        assert(NorSim_programs() - before == ok[i].chunks);
    }
    for (i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        assert(flash_write(0, (off_t)(0x1000U + bad[i].off), buf,
                           bad[i].len) != 0);
    }
    printf("  page split (10 cases) ........ pass (nor_sim never aborted)\n");
}

static void round_trip(void) {
    char rd[32];

    Fs.offset = 0;
    Fs.sector_size = NOR_SIM_SECTOR;
    Fs.sector_count = 8;
    Fs.flash_device = nvs_port_device();
    assert(nvs_mount(&Fs) == 0);

    assert(nvs_write(&Fs, 1, "hello", 5) == 5);
    memset(rd, 0, sizeof(rd));
    assert(nvs_read(&Fs, 1, rd, sizeof(rd)) == 5);
    assert(strcmp(rd, "hello") == 0);

    /* Writing the same bytes again must not touch flash at all. */
    {
        unsigned const before = NorSim_programs();
        assert(nvs_write(&Fs, 1, "hello", 5) == 0);
        assert(NorSim_programs() == before);
    }

    assert(nvs_write(&Fs, 1, "world!", 6) == 6);
    memset(rd, 0, sizeof(rd));
    assert(nvs_read(&Fs, 1, rd, sizeof(rd)) == 6);
    assert(strcmp(rd, "world!") == 0);

    assert(nvs_delete(&Fs, 1) == 0);
    assert(nvs_read(&Fs, 1, rd, sizeof(rd)) < 0);
    printf("  round trip / dedupe / delete . pass\n");
}

static void wrap_and_gc(void) {
    unsigned const erases_before = NorSim_erases();
    int i;

    GcFs.offset = 0;
    GcFs.sector_size = NOR_SIM_SECTOR;
    GcFs.sector_count = 4;
    GcFs.flash_device = nvs_port_device();
    assert(nvs_mount(&GcFs) == 0);

    /* 512-byte entries in a 16 KB file system: about eight per sector, so a
     * few dozen writes force the first GC and the wrap. */
    for (i = 0; i < 40; i++) {
        memset(GcBuf, (int)(0x40 + (i % 32)), sizeof(GcBuf));
        GcBuf[0] = (uint8_t)i;
        assert(nvs_write(&GcFs, (uint16_t)(i % 8), GcBuf, sizeof(GcBuf))
               == (ssize_t)sizeof(GcBuf));
    }

    /* Every id must hold exactly the bytes of its own last write, which is
     * the whole point of surviving a GC: the newest copy wins. */
    for (i = 0; i < 8; i++) {
        uint8_t rd[sizeof(GcBuf)];
        int const last = 32 + i;             /* 32..39: the newest run */
        ssize_t const n = nvs_read(&GcFs, (uint16_t)i, rd, sizeof(rd));

        assert(n == (ssize_t)sizeof(GcBuf));
        assert(rd[0] == (uint8_t)last);
        assert(rd[1] == (uint8_t)(0x40 + (last % 32)));
    }

    printf("  wrap + gc ................... pass (%u erases, %u programs, "
           "%u bytes)\n", NorSim_erases() - erases_before, NorSim_programs(),
           NorSim_bytesProgrammed());
}

int main(void) {
    printf("sm_nvs host tests\n");
    NorSim_init();
    crc_vectors();
    page_split();
    NorSim_init();
    round_trip();
    NorSim_init();
    wrap_and_gc();
    printf("all pass\n");
    return 0;
}
