# Host build of the core, its flash port and the tests. No Zephyr, no board.
#
# The vendored files keep upstream's spelling, which is not -Wall clean here;
# two suppressions cover exactly that and nothing else. Everything we wrote
# (nvs_flash_port.c, the tests) is built without them.
VENDOR_WARNS := -Wno-sign-compare -Wno-unused-parameter
CFLAGS := -std=c99 -Wall -Wextra -O1 -g -DNVS_HOST_BUILD -I include -I src -I tests

CORE := src/nvs.c src/nvs_crc.c
PORT := src/nvs_flash_port.c tests/nor_sim.c
TEST := tests/test_nvs.c

test_nvs: $(CORE) $(PORT) $(TEST)
	@$(CC) $(CFLAGS) src/nvs.c src/nvs_crc.c $(VENDOR_WARNS) \
	      src/nvs_flash_port.c tests/nor_sim.c $(TEST) -o $@
	@./test_nvs

clean:
	rm -f test_nvs

.PHONY: clean test_nvs
