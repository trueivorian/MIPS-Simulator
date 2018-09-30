#include "mips_test_encoder.h"

uint32_t opcode(uint32_t instr)
{
	return (instr<<26);
}

uint32_t rs(uint32_t instr)
{
	return (instr<<21);
}

uint32_t rt(uint32_t instr)
{
	return (instr<<16);
}

uint32_t rd(uint32_t instr)
{
	return (instr<<11);
}

uint32_t shift(uint32_t instr)
{
	return (instr<<6);
}

uint32_t func(uint32_t instr)
{
	return (instr<<0);
}

uint32_t branch_func(uint32_t instr)
{
	return (instr<<16);
}

uint32_t data(uint32_t instr)
{
	return (instr<<0);
}

uint32_t addr(uint32_t instr)
{
	return (instr<<0);
}