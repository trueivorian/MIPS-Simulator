#include "mips.h"
#include "mips_cpu_decoder.h"
#include "mips_cpu_alu.h"
#include <iostream>

using namespace std;

struct mips_cpu_impl
{
	uint32_t pc;
	uint32_t npc;
	uint32_t regs[32];
	uint32_t hi;
	uint32_t lo;

	unsigned logLevel;

	FILE* logDst;

	mips_mem_h ram;
};

mips_error fetch(mips_cpu_h state, uint32_t &instr);
mips_error fetch_next(mips_cpu_h state, uint32_t &instr);
mips_error execute(mips_cpu_h state, const uint32_t instr);
mips_error mips_cpu_get_npc(mips_cpu_h state, uint32_t *npc);
mips_error mips_cpu_set_npc(mips_cpu_h state, uint32_t npc);

mips_cpu_h mips_cpu_create(mips_mem_h mem)
{
	mips_cpu_impl *cpu = new mips_cpu_impl;
	cpu->ram = mem;
	cpu->pc = 0;
	cpu->npc = cpu->pc + 4;
	cpu->hi = 0;
	cpu->lo = 0;

	for(int i=0; i<=31; i++)
	{
		cpu->regs[i] = 0;
	}

	return cpu;
}

mips_error mips_cpu_reset(mips_cpu_h state)
{
	
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}
	
	state->pc = 0;
	state->npc = 0;

	for(int i=0; i<=31; i++)
	{
		state->regs[i] = 0;
	}

	return mips_Success;
}

/*! Returns the current value of one of the 32 general purpose MIPS registers */ 
mips_error mips_cpu_get_register( // If we refer to this function we can access the register
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	unsigned index,		//!< Index from 0 to 31
	uint32_t *value		//!< Where to write the value to
){
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	*value = state->regs[index]; 

	return mips_Success;
}

mips_error mips_cpu_set_register(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	unsigned index,		//!< Index from 0 to 31
	uint32_t value		//!< New value to write into register file
)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	state->regs[index] = value;

	return mips_Success;
}

mips_error mips_cpu_set_pc(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t pc			//!< Address of the next instruction to exectute.
)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	state->pc = pc;
	state->npc = pc +4;

	return mips_Success;
}

mips_error mips_cpu_get_pc(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t *pc		//!< Where to write the byte address too
)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	*pc = state->pc;

	return mips_Success;
}

mips_error mips_cpu_get_npc(
		mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t *npc		//!< Where to write the byte address too
)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	*npc = state->npc;

	return mips_Success;
}

mips_error mips_cpu_set_npc(
		mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t npc		//!< Valid (non-empty) handle to a CPU
)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	state->npc = npc;

	return mips_Success;
}

mips_error mips_cpu_set_accum(mips_cpu_h state, uint32_t hi, uint32_t lo)
{
	mips_error err = mips_Success;

	err = mips_cpu_set_hi(state, hi);
	err = mips_cpu_set_lo(state, lo);

	return err;
}

mips_error mips_cpu_set_hi(mips_cpu_h state, uint32_t hi)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	state->hi = hi;
	
	return mips_Success;
}

mips_error mips_cpu_set_lo(mips_cpu_h state, uint32_t lo)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	state->lo = lo;

	return mips_Success;
}

mips_error mips_cpu_get_hi(mips_cpu_h state, uint32_t* hi)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	*hi = state->hi;

	return mips_Success;
}

mips_error mips_cpu_get_lo(mips_cpu_h state, uint32_t* lo)
{
	if(!state)
	{
		return mips_ErrorInvalidHandle;
	}

	*lo = state->lo;

	return mips_Success;
}

mips_error mips_cpu_step( // this forwards the CPU by one instruction
	mips_cpu_h state	//! Valid (non-empty) handle to a CPU
)
{
	mips_error err = mips_Success;

	uint32_t instr = 0;

	// fetch
	err = fetch(state, instr);
	// execute
	err = execute(state, instr);

	err = mips_cpu_set_register(state, 0, 0);

	err = mips_cpu_set_pc(state, state->npc);

	return err;
}

mips_error fetch(mips_cpu_h state, uint32_t &instr)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];

	uint32_t pc = 0;

	err = mips_cpu_get_pc(state, &pc);

	err = mips_mem_read(state->ram, pc, 4, mem_buffer);
	


	instr = to_big(mem_buffer);

	return err;
}

mips_error fetch_next(mips_cpu_h state, uint32_t &instr)
{
	mips_error err = mips_Success;

	uint8_t mem_buffer[4];

	uint32_t npc = 0;

	err = mips_cpu_get_npc(state, &npc);

	err = mips_mem_read(state->ram, npc, 4, mem_buffer);

	return err;
}

mips_error mips_cpu_set_debug_level(mips_cpu_h state, unsigned level, FILE *dest)
{
	state->logLevel = level;
	state->logDst = dest;
	return mips_Success;
}

void mips_cpu_free(mips_cpu_h state)
{
	delete state;
}

mips_error execute(mips_cpu_h state, const uint32_t instr)
{
	mips_error err = mips_Success;

	// Initialise variables
	uint32_t opcode = decode_opcode(instr);
	uint32_t rs = 0;
	uint32_t rt = 0;
	uint32_t rd = 0;
	uint32_t shift = 0;
	uint32_t func = 0;
	uint32_t addr = 0;
	uint32_t data = 0;

	if(opcode==0)
	{
		// r type
		shift = decode_shift(instr);
		func = decode_func(instr);
		err = mips_cpu_get_register(state, decode_rs(instr), &rs);
		err = mips_cpu_get_register(state, decode_rt(instr), &rt);

		switch(func)
		{
			case 0x20:
				cout << "ADD $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;

				if(shift != 0x0)
				{
					return mips_ExceptionInvalidInstruction;
				}

				err = ADD(rd, rs, rt);
			break;
			case 0x21:
				cout << "ADDU $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;
				
				if(shift != 0x0)
				{
					return mips_ExceptionInvalidInstruction;
				}

				err = ADDU(rd, rs, rt);
			break;
			case 0x24:
				cout << "AND $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;
				
				if(shift != 0x0)
				{
					return mips_ExceptionInvalidInstruction;
				}

				err = AND(rd, rs, rt);
			break;
			case 0x1A:
				cout << "DIV $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;

				if(!rt)
				{
					return mips_ExceptionInvalidInstruction;
				}

				err = DIV(state, rs, rt);
			break;
			case 0x1B:
				cout << "DIVU $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;
				err = DIVU(state, rs, rt);
			break;
			case 0x05:
				cout << "JALR $" << decode_rd(instr) << ", $" << decode_rs(instr) << endl;
				err = JALR(state, rd, rs);
			break;
			case 0x08:
				cout << "JR $" << decode_rs(instr) << endl;
				err = JR(state, rs);
			break;
			case 0x10:
				cout << "MFHI $" << decode_rd(instr) << endl;
				err = MFHI(state, rd);
			break;
			case 0x12:
				cout << "MFLO $" << decode_rd(instr) << endl;
				err = MFLO(state, rd);
			break;
			case 0x11:
				cout << "MTHI $" << decode_rs(instr) << endl;
				err = MTHI(state, rd);
			break;
			case 0x13:
				cout << "MTLO $" << decode_rs(instr) << endl;
				err = MFLO(state, rs);
			break;
			case 0x18:
				cout << "MULT $" << decode_rs(instr) << ", $" << decode_rt(instr) <<  endl;
				err = MULT(state, rs, rt);
			break;
			case 0x19:
				cout << "MULTU $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;
				err = MULTU(state, rs, rt);
			break;
			case 0x25:
				cout << "OR $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;
				err = OR(rd, rs, rt);
			break;
			case 0x00:
				cout << "SLL $" << decode_rd(instr) << ", $" << decode_rt(instr) << ", " << shift << endl;
				err = SLL(rd, rt, shift);
			break;
			case 0x04:
				cout << "SLLV $" << decode_rd(instr) << ", $" << decode_rt(instr) << ", $" << decode_rs(instr) << endl;
				err = SLLV(rd, rt, rs);
			break;
			case 0x2A:
				cout << "SLT $" << decode_rd(instr) << ", $" << decode_rt(instr) << ", $" << decode_rs(instr) << endl;
				err = SLT(rd, rs, rt);
			break;
			case 0x2B:
				cout << "SLTU $" << decode_rd(instr) << ", $" << decode_rt(instr) << ", $" << decode_rs(instr) << endl;
				err = SLTU(rd, rs, rt);
			case 0x03:
				cout << "SRA $" << decode_rd(instr) << ", $" << decode_rt(instr) << ", " << shift << endl;
				err = SRA(rd, rt, shift);
			break;
			case 0x07:
				cout << "SRAV $" << decode_rd(instr) << ", $" << decode_rt(instr) << ", $" << decode_rs(instr) << endl;
				err = SRAV(rd, rt, rs);
			break;
			case 0x02:
				cout << "SRL $" << decode_rd(instr) << ", $" << decode_rt(instr) << ", " << shift << endl;
				err = SRL(rd, rt, shift);
			break;
			case 0x06:
				cout << "SRLV $" << decode_rd(instr) << ", $" << decode_rt(instr) << ", $" << decode_rs(instr) << endl;
				err = SRLV(rd, rt, rs);
			break;
			case 0x22:
				cout << "SUB $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;
				err = SUB(rd, rs, rt);
			break;
			case 0x23:
				cout << "SUBU $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;
				err = SUBU(rd, rs, rt);
			break;
			case 0x26:
				cout << "XOR $" << decode_rd(instr) << ", $" << decode_rs(instr) << ", $" << decode_rt(instr) << endl;
				err = XOR(rd, rs, rt);
			break;
		}

		err = mips_cpu_set_register(state, decode_rd(instr), rd);
	}
	else if(opcode==1)
	{
		// j type
		uint32_t branch_func = decode_branch_func(instr);
		data = decode_data(instr);

		switch(branch_func)
		{
			case 0x01:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = BGEZ(state, rs, data);
				cout << "BGEZ $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x11:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = BGEZAL(state, rs, data);
				cout << "BGEZAL $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x00:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = BLTZ(state, rs, data);
				cout << "BLTZ $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x10:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = BLTZAL(state, rs, data);
				cout << "BLTZAL $" << decode_rs(instr) << ", " << data << endl;
			break;
		}
	}
	else
	{
		// i type
		data = decode_data(instr);		
		uint32_t instr_next = 0;
		err = fetch_next(state, instr_next);
		uint32_t opcode_next = decode_opcode(instr_next);
		uint32_t nrt = 0;

		switch(opcode)
		{

			case 0x08:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = ADDI(rt, rs, data);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "ADDI $" << decode_rt(instr) << ", $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x09:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = ADDIU(rt, rs, data);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "ADDIU $" << decode_rt(instr) << ", $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x0C:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = ANDI(rt, rs, data);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "ANDI $" << decode_rt(instr) << ", $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x04:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = mips_cpu_get_register(state, decode_rt(instr), &rt);
				err = BEQ(state, rs, rt, data);
				cout << "BEQ $" << decode_rs(instr) << ", $" << decode_rt(instr) << ", " << data << endl;
			break;
			case 0x07:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = mips_cpu_get_register(state, decode_rt(instr), &rt);
				err = BGTZ(state, rs, data);
				cout << "BGTZ $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x06:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = BLEZ(state, rs, data);
				cout << "BLEZ $" << decode_rs(instr) << ", " << data << endl;	
			break;
			case 0x05:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = mips_cpu_get_register(state, decode_rt(instr), &rt);
				err = BNE(state, rs, rt, data);
				cout << "BNE $" << decode_rs(instr) << ", $" << decode_rt(instr) << ", " << data << endl;		
			break;
			case 0x02:
				err = J(state, decode_addr(instr));
				cout << "J " << decode_addr(instr) << endl;
			break;
			case 0x03:
				err = JAL(state, decode_addr(instr));
				cout << "JAL " << decode_addr(instr) << endl;
			break;
			case 0x20:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = LB(state->ram, rs + data, rt);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "LB $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x24:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = LBU(state->ram, rs + data, rt);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "LBU $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x21:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);

				if(!data%2)
				{
					return mips_ExceptionInvalidInstruction;
				}

				err = LH(state->ram, rs + data, rt);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "LH $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x25:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);

				if(!data%2)
				{
					return mips_ExceptionInvalidInstruction;
				}

				err = LHU(state->ram, rs + data, rt);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "LHU $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x23:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = LW(state->ram, rs + data, rt);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "LW $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x22:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = LWL(state->ram, rs + data, rt);
				err = mips_cpu_get_register(state, decode_rt(instr_next), &nrt);

				if((opcode_next != 0x22) || (opcode_next != 0x26))
				{
					if(rt==nrt)
					{
						return mips_ExceptionInvalidInstruction;
					}
				}

				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "LWL $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x26:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = LWR(state->ram, rs + data, rt);
				err = mips_cpu_set_register(state, decode_rs(instr), rt);
				cout << "LWR $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x0D:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = ORI(rt, rs, data);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "ORI $" << decode_rt(instr) << ", $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x28:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = mips_cpu_get_register(state, decode_rt(instr), &rt);
				err = SB(state->ram, rs + data, rt);
				cout << "SB $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;				
			break;
			case 0x29:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = mips_cpu_get_register(state, decode_rt(instr), &rt);

				if(!data%2)
				{
					return mips_ExceptionInvalidInstruction;
				}

				err = SH(state->ram, rs + data, rt);
				cout << "SH $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x0A:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = SLTI(rt, rs, data);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "SLTI $" << decode_rt(instr) << ", $" << decode_rs(instr) << ", " << data << endl;
			break;
			case 0x0B:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = SLTIU(rt, rs, data);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "SLTIU $" << decode_rt(instr) << ", $" << decode_rs(instr) << ", " << data << endl;				
			break;
			case 0x2B:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = mips_cpu_get_register(state, decode_rt(instr), &rt);
				err = SW(state->ram, rs + data, rt);
				cout << "SW $" << decode_rt(instr) << ", " << data << "($" << decode_rs(instr) << ")" << endl;
			break;
			case 0x0E:
				err = mips_cpu_get_register(state, decode_rs(instr), &rs);
				err = XORI(rs, rt, data);
				err = mips_cpu_set_register(state, decode_rt(instr), rt);
				cout << "XORI $" << decode_rt(instr) << ", $" << decode_rs << ", " << data << endl;
			break;
			default:
				return mips_ExceptionInvalidInstruction;
		}

	}

	return err;
}