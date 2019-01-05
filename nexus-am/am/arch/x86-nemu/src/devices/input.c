#include <am.h>
#include <x86.h>
#include <amdev.h>

size_t input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _KbdReg *kbd = (_KbdReg *)buf;
      uint32_t data = inl(0x60);
      kbd->keydown = data & 0x8000 ? 1:0;
      kbd->keycode = data & ~0x8000;
      return sizeof(_KbdReg);
    }
  }
  return 0;
}
