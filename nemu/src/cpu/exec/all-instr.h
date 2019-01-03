#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
//dummy
make_EHelper(call);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(ret);
//add
make_EHelper(lea);
make_EHelper(and);
make_EHelper(add);
make_EHelper(cmp);
make_EHelper(setcc);
make_EHelper(movzx);
make_EHelper(test);
make_EHelper(jcc);
make_EHelper(leave);
make_EHelper(inc);
//add-longlong
make_EHelper(nop);
make_EHelper(adc);
make_EHelper(or);
//bit
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(dec);
make_EHelper(not);
//div
make_EHelper(imul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(cltd);
make_EHelper(idiv);
//fact
make_EHelper(jmp);
//load-store
make_EHelper(movsx);
//recursion
make_EHelper(call_rm);
make_EHelper(jmp_rm);
//shift
make_EHelper(shr);
//shuixianhua
make_EHelper(mul);
//sub-longlong
make_EHelper(sbb);

make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(neg);
make_EHelper(cwtl);
make_EHelper(div);
make_EHelper(rol);