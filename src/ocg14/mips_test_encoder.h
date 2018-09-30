#ifndef mips_test_encoder_header
#define mips_test_encoder_header

#include "mips_core.h"

uint32_t opcode(uint32_t instr);
uint32_t rs(uint32_t instr);
uint32_t rt(uint32_t instr);
uint32_t rd(uint32_t instr);
uint32_t shift(uint32_t instr);
uint32_t func(uint32_t instr);
uint32_t branch_func(uint32_t instr);
uint32_t data(uint32_t instr);
uint32_t addr(uint32_t instr);

#endif