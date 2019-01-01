#include "nemu.h"
#include "device/mmio.h"
#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
  int mmio_map = is_mmio(addr);       // mmip map num
  return mmio_map != -1 ? mmio_read(addr, len, mmio_map) :
         pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));}

void paddr_write(paddr_t addr, uint32_t data, int len) {
  int mmio_map = is_mmio(addr);
  return mmio_map != -1 ? mmio_write(addr, len, data, mmio_map)
                        : memcpy(guest_to_host(addr), &data, len);
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, uint32_t data, int len) {
  paddr_write(addr, data, len);
}
