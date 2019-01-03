#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  // TODO();

  // XXX: DONE
  if (id_dest->width == 1)
  {
    uint8_t u_dest_val = id_dest->val;
    id_dest->val = (int8_t)u_dest_val;
  }
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  // TODO();

  // XXX: DONE
  rtl_pop(&t0);

  // if (id_dest->width == 1)
  // {
  //   uint8_t u_t0 = t0;
  //   id_dest->val = (int8_t)u_t0;
  // }
  // else
  // {
  //   id_dest->val = t0;
  // }
  id_dest->val = id_dest->width == 1 ? (int8_t)((uint8_t)t0) : t0;
  operand_write(id_dest, &id_dest->val);

  print_asm_template1(pop);
}

/**
 * Push All General-Purpose Registers
 */
make_EHelper(pusha) {
  // TODO();

  // XXX: DONE
  t0 = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);

  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  // TODO();

  // XXX: DONE
  rtl_push(&cpu.edi);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.ebp);

  rtl_push(&t0);
  rtl_push(&cpu.ebx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.eax);

  print_asm("popa");
}

make_EHelper(leave) {
  // TODO();

  // XXX: DONE
  reg_l(4) = reg_l(5);
  rtl_pop(&t0);
  reg_w(5) = t0;

  print_asm("leave");
}

/**
 * Convert Signed Long to Signed Double Long (cltd)
 * See https://docs.oracle.com/cd/E19455-01/806-3773/instructionset-49/index.html
 * cltd
 * Operation
 *    sign-extend EAX -> EDX:EAX
 * Description
 *    cltd converts the signed long in EAX to a signed double long in EDX:EAX by
 *    extending the most-significant bit (sign bit) of EAX into all bits of EDX.
 * Example
 *    cltd
 */
make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    // TODO();
    // XXX: DONE
    rtl_sext(&t0, &reg_l(R_EAX), 2);
    rtl_shri(&reg_l(R_EDX), &t0, 16);
  }
  else {
    // TODO();
    // XXX: DONE
    rtl_sari(&reg_l(R_EDX), &reg_l(R_EAX), 31);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

/**
 * Convert Word to Long (cwtl)
 * cwtl
 * Operation
 *    sign-extend AX -> EAX
 * Description
 *    cwtl converts the signed word in AX to a signed long in EAX by extending the most-significant bit (sign bit) of AX into two most-significant bytes of EAX.
 * Example
 *    cwtl
 */
make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    // TODO();
    // XXX: DONE
    rtl_shli(&reg_l(R_EAX), &reg_l(R_EAX), 24);
    rtl_shri(&reg_l(R_EAX), &reg_l(R_EAX), 8);
    rtl_shri(&reg_l(R_EAX), &reg_l(R_EAX), 16);
  }
  else {
    // TODO();
    // XXX: DONE
    rtl_sext(&reg_l(R_EAX), &reg_l(R_EAX), 2);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

/**
 * Move with Sign-Extension
 */
make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t0, &id_src->val, id_src->width);
  operand_write(id_dest, &t0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
