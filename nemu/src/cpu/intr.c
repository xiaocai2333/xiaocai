#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
	  rtl_push(&cpu.flags);
	  rtl_push(&cpu.cs);
	  rtl_push(&ret_addr);
	  cpu.eflags.IF = 0;
	  uint32_t idtr_addr = cpu.idtr.base + (NO << 3);
		uint32_t offset_low, offset_high;
		offset_low = vaddr_read(idtr_addr, 4) & 0xffff;
		offset_high = vaddr_read(idtr_addr + 4, 4) & 0xffff0000;
	  rtl_j(offset_high | offset_low);
}

void dev_raise_intr() {
	cpu.INTR = true;
}
