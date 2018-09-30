#ifndef mips_cpu_decoder_header
#define mips_cpu_decoder_header

#include "mips_core.h"

uint32_t decode_opcode(uint32_t instr);
uint32_t decode_rs(uint32_t instr);
uint32_t decode_rt(uint32_t instr);
uint32_t decode_rd(uint32_t instr);
uint32_t decode_shift(uint32_t instr);
uint32_t decode_func(uint32_t instr);
uint32_t decode_branch_func(uint32_t instr);
uint32_t decode_data(uint32_t instr);
uint32_t decode_addr(uint32_t instr);

#endif