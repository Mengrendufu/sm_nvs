#include "nor_sim.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint8_t NorSim_mem_[NOR_SIM_SIZE];
static unsigned NorSim_programs_;
static unsigned NorSim_erases_;
static uint32_t NorSim_bytes_;

static void NorSim_fail_(const char *why, uint32_t addr, size_t len) {
    fprintf(stderr, "NOR SIM VIOLATION: %s (addr=0x%05x len=%zu)\n",
            why, (unsigned)addr, len);
    abort();
}

void NorSim_init(void) {
    memset(NorSim_mem_, NOR_SIM_ERASE_VAL, sizeof(NorSim_mem_));
    NorSim_programs_ = 0;
    NorSim_erases_ = 0;
    NorSim_bytes_ = 0;
}

int NorSim_read(uint32_t addr, void *buf, size_t len) {
    if ((size_t)addr + len > sizeof(NorSim_mem_)) {
        return -1;
    }
    memcpy(buf, &NorSim_mem_[addr], len);
    return 0;
}

int NorSim_pageProgram(uint32_t addr, const void *buf, size_t len) {
    const uint8_t *p = (const uint8_t *)buf;
    size_t i;

    if (len == 0U || ((size_t)addr + len > sizeof(NorSim_mem_))) {
        return -1;
    }
    /* Contract 8: one command, one page. */
    if (((addr % NOR_SIM_PAGE) + len) > NOR_SIM_PAGE) {
        NorSim_fail_("page program crossed a page boundary", addr, len);
    }
    /* Contract 5: NOR only clears bits. */
    for (i = 0; i < len; i++) {
        uint8_t const old = NorSim_mem_[addr + i];
        if ((old & p[i]) != p[i]) {
            NorSim_fail_("program tried to set a bit (0 -> 1)", addr + i, 1);
        }
    }
    memcpy(&NorSim_mem_[addr], p, len);
    NorSim_programs_++;
    NorSim_bytes_ += (uint32_t)len;
    return 0;
}

int NorSim_sectorErase(uint32_t addr) {
    if ((addr % NOR_SIM_SECTOR) != 0U || (size_t)addr + NOR_SIM_SECTOR >
        sizeof(NorSim_mem_)) {
        return -1;
    }
    memset(&NorSim_mem_[addr], NOR_SIM_ERASE_VAL, NOR_SIM_SECTOR);
    NorSim_erases_++;
    return 0;
}

unsigned NorSim_programs(void) { return NorSim_programs_; }
unsigned NorSim_erases(void) { return NorSim_erases_; }
uint32_t NorSim_bytesProgrammed(void) { return NorSim_bytes_; }
