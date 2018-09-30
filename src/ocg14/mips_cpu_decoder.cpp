#include "mips_cpu_decoder.h"

uint32_t decode_opcode(uint32_t instr)
{
	return (instr>>26) & 0x0000003F;
}

uint32_t decode_rs(uint32_t instr)
{
	return (instr>>21) & 0x0000001F;
}

uint32_t decode_rt(uint32_t instr)
{
	return (instr>>16) & 0x0000001F;
}

uint32_t decode_rd(uint32_t instr)
{
	return (instr>>11) & 0x0000001F;
}

uint32_t decode_shift(uint32_t instr)
{
	return (instr>>6) & 0x0000001F;
}

uint32_t decode_func(uint32_t instr)
{
	return (instr>>0) & 0x0000003F;
}

uint32_t decode_branch_func(uint32_t instr)
{
	return (instr>>16) & 0x0000001F;
}

uint32_t decode_data(uint32_t instr)
{
	return (instr>>0) & 0x0000FFFF;
}

uint32_t decode_addr(uint32_t instr)
{
	return (instr>>0) & 0x03FFFFFF;
}