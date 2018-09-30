#include "mips_cpu_alu.h"
#include "mips_mem.h"
#include "math.h"

//Sign Extension

uint32_t sign_extend(uint8_t n)
{
	uint32_t n32 = (uint32_t)n;

	if(n & 0x80)
	{
		n32 = 0xFFFF0000|n32;
	}

	return n32;
}

uint32_t sign_extend(uint16_t n)
{
	uint32_t n32 = (uint32_t)n;

	if(n & 0x8000)
	{
		n32 = 0xFFFF0000|n32;
	}

	return n32;
}

uint64_t sign_extend(uint32_t n)
{
	uint64_t n64 = (uint64_t)n;

	if(n & 0x80000000)
	{
		n64 = 0xFFFFFFFF00000000|n64;
	}

	return n64;
}

//Test for Overflow
bool arithmetic_overflow_32_bit(uint32_t rs, uint32_t rt)
{
	uint32_t test = rs + rt;

	if((rs & 0x80000000) && (rt & 0x80000000))
	{
		if(!(test & 0x80000000))
		{
			return true;
		}
	}
	else if(!(rs & 0x80000000) && !(rt & 0x80000000))
	{
		if((rs!=0||rt!=0) && (test & 0x80000000))
		{
			return true;
		}
	}
	

	return false;
}

bool arithmetic_overflow_32_bit(uint32_t rd, uint16_t n)
{
	return arithmetic_overflow_32_bit(rd,sign_extend(n));
}

//Endian Conversion
uint32_t to_big(const uint8_t *pData)
{
	return
		(((uint32_t)pData[0])<<24)
		|
		(((uint32_t)pData[1])<<16)
		|
		(((uint32_t)pData[2])<<8)
		|
		(((uint32_t)pData[3])<<0);
}

void to_little(const uint32_t rt, uint8_t* pData)
{
	uint8_t mem_buffer[4];

	mem_buffer[0] = (uint8_t)(rt>>24);
	mem_buffer[1] = (uint8_t)(rt>>16);
	mem_buffer[2] = (uint8_t)(rt>>8);
	mem_buffer[3] = (uint8_t)(rt>>0);
}

mips_error ADD(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	if(arithmetic_overflow_32_bit(rs,rt))
	{
		return mips_ExceptionArithmeticOverflow;
	}

	rd = rs + rt;

	return mips_Success;
}

mips_error ADDI(uint32_t& rt, uint32_t rs, const uint16_t n)
{
	return ADD(rt, rs, sign_extend(n));
}

mips_error ADDU(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	rd = rs + rt;

	return mips_Success;
}

mips_error ADDIU(uint32_t& rt, uint32_t rs, const uint16_t n)
{
	return ADDU(rt, rs, sign_extend(n));
}

mips_error AND(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	rd = rs & rt;

	return err;
}

mips_error ANDI(uint32_t& rt, uint32_t rs, const uint16_t n)
{
	return AND(rt, rs, sign_extend(n));
}

mips_error BEQ(mips_cpu_h state, uint32_t rs, uint32_t rt, const uint16_t n)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);

	if(rs == rt)
	{
		err = mips_cpu_set_npc(state, npc + sign_extend(n)<<2);
	}

	return err;
}

mips_error BGEZ(mips_cpu_h state, uint32_t rs, const uint16_t n)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);

	if(rs >= 0)
	{
		err = mips_cpu_set_npc(state, npc + sign_extend(n)<<2);
	}

	return err;
}

mips_error BGEZAL(mips_cpu_h state, uint32_t rs, const uint16_t n)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);

	if(rs >= 0)
	{
		err = mips_cpu_set_register(state, 31, npc);
		err = mips_cpu_set_npc(state, npc + sign_extend(n)<<2);
	}

	return err;
}

mips_error BGTZ(mips_cpu_h state, uint32_t rs, const uint16_t n)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);

	if(rs)
	{
		err = mips_cpu_set_npc(state, npc + sign_extend(n)<<2);
	}

	return err;
}

mips_error BLEZ(mips_cpu_h state, uint32_t rs, const uint16_t n)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_pc(state, &npc);

	if(rs <= 0)
	{
		err = mips_cpu_set_npc(state, npc + sign_extend(n)<<2);
	}

	return err;
}

mips_error BLTZ(mips_cpu_h state, uint32_t rs, const uint16_t n)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);

	if(rs < 0)
	{
		err = mips_cpu_set_npc(state, npc + sign_extend(n)<<2);
	}

	return err;
}

mips_error BLTZAL(mips_cpu_h state, uint32_t rs, const uint16_t n)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);

	if(rs < 0)
	{
		err = mips_cpu_set_register(state, 31, npc);
		err = mips_cpu_set_npc(state, npc + sign_extend(n)<<2);
	}

	return err;
}

mips_error BNE(mips_cpu_h state, uint32_t rs, uint32_t rt, const uint16_t n)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);

	if(rs != rt)
	{
		err = mips_cpu_set_npc(state, npc + sign_extend(n)<<2);
	}

	return err;
}

mips_error J(mips_cpu_h state, const uint32_t n)
{
	mips_error err = mips_Success;

	err = mips_cpu_set_npc(state, n);

	return err;
}

mips_error JALR(mips_cpu_h state, uint32_t rs, uint32_t nrd)
{
	mips_error err = mips_Success;

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);
	err = mips_cpu_set_register(state, nrd, npc);
	err = mips_cpu_set_npc(state, rs);

	return err;
}

mips_error JAL(mips_cpu_h state, const uint32_t n)
{
	mips_error err = mips_Success;
	
	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);
	err = mips_cpu_set_register(state, 31, npc);
	err = mips_cpu_set_npc(state, n);

	return err;
}

mips_error JR(mips_cpu_h state, uint32_t rs)
{
	mips_error err = mips_Success;

	err = mips_cpu_set_pc(state, rs);

	return err;
}

mips_error DIV(mips_cpu_h state, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	/*bool rs_pos = !(rs & 0x80000000);
	bool rt_pos = !(rt & 0x80000000);

	if(!rs_pos)
	{
		rs = ~(rs - 1);
	}

	if(!rt_pos)
	{
		rt = ~(rt - 1);
	}*/

	uint64_t q = sign_extend(rs/rt); // Quotient
	uint64_t r = sign_extend(rs%rt); // Remainder

	/*if((!rs_pos && rt_pos) || (rs_pos && !rt_pos))
	{
		q = ~q + 1;
		r = ~r + 1;
	}*/

	err = mips_cpu_set_accum(state, (uint32_t)r, (uint32_t)q);
	
	return err;
}

mips_error DIVU(mips_cpu_h state, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	uint32_t q = rs/rt; // Quotient
	uint32_t r = rs%rt; // Remainder

	err = mips_cpu_set_accum(state, r, q);

	return err;
}

mips_error LB(mips_mem_h mem, uint32_t addr, uint32_t& rt)
{
	mips_error err = mips_Success;
	
	uint8_t mem_buffer[4];

	uint32_t eff_addr = addr*4 - addr%4; // Calculates the effective address
	
	err = mips_mem_read(mem, eff_addr, 4, mem_buffer);

	rt = sign_extend(mem_buffer[addr%4]);

	return err;
}

mips_error LBU(mips_mem_h mem, uint32_t addr, uint32_t& rt)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];

	uint32_t eff_addr = addr*4 - addr %4;

	err = mips_mem_read(mem, eff_addr, 4, mem_buffer);

	rt = (uint32_t)mem_buffer[addr%4];

	return err;
}

mips_error LH(mips_mem_h mem, uint32_t addr, uint32_t& rt)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];
	
	uint32_t eff_addr = addr*2;

	if(addr%2)
	{
		return mips_ExceptionInvalidAddress;
	}

	err = mips_mem_read(mem, eff_addr, 4, mem_buffer);

	rt = sign_extend((uint16_t)to_big(mem_buffer));

	return err;
}

mips_error LHU(mips_mem_h mem, uint32_t addr, uint32_t& rt)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];
	
	uint32_t eff_addr = addr*2;

	if(addr%2)
	{
		return mips_ExceptionInvalidAddress;
	}

	err = mips_mem_read(mem, eff_addr, 4, mem_buffer);

	rt = to_big(mem_buffer) & 0x0000FFFF;

	return err;
}

mips_error LW(mips_mem_h mem, uint32_t addr, uint32_t& rt)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];

	if(addr%4)
	{
		return mips_ExceptionInvalidAddress;
	}

	err = mips_mem_read(mem, addr, 4, mem_buffer);

	rt = to_big(mem_buffer);

	return err;
}

mips_error LWL(mips_mem_h mem, uint32_t addr, uint32_t& rt)
{
	mips_error err = mips_Success;
	
	uint8_t mem_buffer[4];

	if(addr%4)
	{
		return mips_ExceptionInvalidAddress;
	}

	err = mips_mem_read(mem, addr, 4, mem_buffer);

	rt = (rt & 0x0000FFFF) | (to_big(mem_buffer) & 0xFFFF0000);

	return err;	
}

mips_error LWR(mips_mem_h mem, uint32_t addr, uint32_t& rt)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];

	if(addr%2)
	{
		return mips_ExceptionInvalidAddress;
	}

	err = mips_mem_read(mem, addr, 4, mem_buffer);

	rt = (rt & 0xFFFF0000) | (to_big(mem_buffer) & 0x0000FFFF);

	return err;
}

mips_error LUI(uint32_t& rt, const uint16_t n)
{
	mips_error err = mips_Success;

	rt = (uint32_t)n << 16;
	
	return err;
}

mips_error MFHI(mips_cpu_h state, uint32_t& rd)
{
	mips_error err = mips_Success;

	err = mips_cpu_get_hi(state, &rd);

	return err;
}

mips_error MFLO(mips_cpu_h state, uint32_t& rd)
{
	mips_error err = mips_Success;

	err = mips_cpu_get_lo(state, &rd);

	return err;
}

mips_error MTHI(mips_cpu_h state, uint32_t& rs)
{
	mips_error err = mips_Success;

	err = mips_cpu_set_hi(state, rs);

	return err;
}

mips_error MTLO(mips_cpu_h state, uint32_t& rs)
{
	mips_error err = mips_Success;

	err = mips_cpu_set_lo(state, rs);

	return err;
}

mips_error MULT(mips_cpu_h state, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	int64_t result = sign_extend(rs) * sign_extend(rt);	

	err = mips_cpu_set_accum(state, (uint32_t)(result>>32), (uint32_t)result);

	return err;
}

mips_error MULTU(mips_cpu_h state, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	uint64_t result = rs*rt;

	err = mips_cpu_set_accum(state, (uint32_t)(result>>32), (uint32_t)result);

	return err;
}

mips_error OR(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	rd = rs | rt;

	return err;
}

mips_error ORI(uint32_t& rt, uint32_t rs, const uint16_t n)
{
	return OR(rt, rs, sign_extend(n));
}

mips_error SB(mips_mem_h mem, uint32_t addr, uint32_t rt)
{
	mips_error err = mips_Success;
	
	uint8_t mem_buffer[4];

	uint32_t eff_addr = addr - addr%4; // Calculates the effective address
	
	err = mips_mem_read(mem, eff_addr, 4, mem_buffer);

	mem_buffer[addr%4] = (uint8_t)rt;

	err = mips_mem_write(mem, addr, 4, mem_buffer);

	return err;
}

mips_error SH(mips_mem_h mem, uint32_t addr, uint32_t rt)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];
	
	uint32_t eff_addr = addr*2;

	if(addr%2)
	{
		return mips_ExceptionInvalidAddress;
	}

	err = mips_mem_read(mem, addr, 4, mem_buffer);

	mem_buffer[0] = (uint8_t)rt;
	mem_buffer[1] = (uint8_t)(rt>>8);

	err = mips_mem_write(mem, addr, 4, mem_buffer);

	return err;

}

mips_error SLL(uint32_t& rd, uint32_t rt, const uint32_t n)
{
	mips_error err = mips_Success;

	if(n == 32)
	{
		return mips_ExceptionInvalidInstruction;
	}

	rd = rt >> n;

	return err;
}

mips_error SLLV(uint32_t& rd, uint32_t rt, uint32_t rs)
{
	return SLL(rd, rt, rs);
}

mips_error SLT(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	if((rs & 0x80000000) && (rt & 0x80000000))
	{
		rd = rs > rt;
	}
	else if((rs & 0x80000000) && !(rt & 0x80000000))
	{
		rd = 0x1;
	}
	else if(!(rs & 0x80000000) && (rt & 0x80000000))
	{
		rd = 0;
	}
	else
	{
		rd = rs < rt;
	}

	return err;
}

mips_error SLTI(uint32_t& rt, uint32_t rs, const uint16_t n)
{
	return SLT(rt, rs, sign_extend(n));
}

mips_error SLTIU(uint32_t& rt, uint32_t rs, const uint16_t n)
{
	return SLTU(rt, rs, (uint32_t)n);
}

mips_error SLTU(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	rd = rs < rt;

	return err;
}

mips_error SRA(uint32_t& rd, uint32_t rt, const uint32_t n)
{
	mips_error err = mips_Success;

	uint64_t result = 0;
	
	if(n == 32)
	{
		return mips_ExceptionInvalidInstruction;
	}

	result = sign_extend(rt) >> n;

	rd = (uint32_t)result;

	return err;
}

mips_error SRAV(uint32_t& rd, uint32_t rt, uint32_t rs)
{
	return SRA(rd, rt, rs);
}

mips_error SRL(uint32_t& rd, uint32_t rt, const uint32_t n)
{
	mips_error err = mips_Success;

	if(n == 32)
	{
		return mips_ExceptionInvalidInstruction;
	}

	rd = rt >> n;

	return err;
}

mips_error SRLV(uint32_t& rd, uint32_t rt, uint32_t rs)
{
	return SRL(rd, rt, rs); 
}

mips_error SUB(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	return ADD(rd, rs, ~rt + 1);
}

mips_error SUBU(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	return ADDU(rd, rs, ~rt + 1);
}

mips_error SW(mips_mem_h mem, uint32_t addr, uint32_t rt)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];

	to_little(rt, mem_buffer);

	if(addr%4)
	{
		return mips_ExceptionInvalidInstruction;
	}

	err = mips_mem_write(mem, addr, 4, mem_buffer);

	return err;
}

mips_error XOR(uint32_t& rd, uint32_t rs, uint32_t rt)
{
	mips_error err = mips_Success;

	rd = rs^rt;

	return err;
}

mips_error XORI(uint32_t& rt, uint32_t rs, const uint16_t n)
{
	return XOR(rt, rs, sign_extend(n));
}



