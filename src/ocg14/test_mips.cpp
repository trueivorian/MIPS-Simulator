/* This file is an implementation of the functions
   defined in mips_test.h. It is designed to be
   linked against something which implements the
   functions from mips_cpu.h and mips_mem.h, plus
   some sort of main program to run the tests.
*/

#include "mips.h"
#include "mips_test_encoder.h"
#include <string> 
#include <iostream>

using namespace std;

void to_little(const uint32_t rt, uint8_t* pData);

int main()
{

	mips_mem_h mem=mips_mem_create_ram(4096, 4);

	mips_cpu_h cpu=mips_cpu_create(mem);
 
	mips_error err = mips_cpu_set_debug_level(cpu, 4, stderr);

	int testId = 0;
	int passed = 0;

	uint32_t instr = 0;
	uint32_t got = 0;	
	
	uint8_t buffer[4];
	
	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	mips_test_begin_suite();

	// Standard Add Test
	testId = mips_test_begin_test("add");    
 
	// 1 - Setup an instruction in ram
	// addu r6, r4, r5
	instr = opcode(0) | rs(4) | rt(5) | rd(6) | shift(0) | func(0x20);
 
	to_little(instr, buffer);
 
	err = mips_mem_write(mem, 0, 4, buffer);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
  
	// 2 - put register values in cpu
	err = mips_cpu_set_register(cpu, 4, 40);
	err = mips_cpu_set_register(cpu, 5, 50);
 
	// 3 - step CPU
	err = mips_cpu_step(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	// 4 -Check the result

	err = mips_cpu_get_register(cpu, 6, &got); 

	passed = got == 40+50;

	mips_test_end_test(testId, passed, "40 + 50 != 90");

	err = mips_cpu_reset(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
	// Signed Add Test
	testId = mips_test_begin_test("add");    
 
	// 1 - Setup an instruction in ram
	// addu r6, r4, r5
	instr = opcode(0) | rs(4) | rt(5) | rd(6) | shift(0) | func(0x20);
 
	to_little(instr, buffer);
 
	err = mips_mem_write(mem, 0, 4, buffer);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
  
	// 2 - put register values in cpu
	err = mips_cpu_set_register(cpu, 4, -40);
	err = mips_cpu_set_register(cpu, 5, -50);
 
	// 3 - step CPU
	err = mips_cpu_step(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	// 4 -Check the result

	err = mips_cpu_get_register(cpu, 6, &got); 

	passed = got == 0;

	mips_test_end_test(testId, passed, "-40 + -50 != -90");

	err = mips_cpu_reset(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}

	// Overflow Add Test
	testId = mips_test_begin_test("add");    
 
	// 1 - Setup an instruction in ram
	// addu r6, r4, r5
	instr = opcode(0) | rs(4) | rt(5) | rd(6) | shift(0) | func(0x20);
 
	to_little(instr, buffer);
 
	err = mips_mem_write(mem, 0, 4, buffer);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
  
	// 2 - put register values in cpu
	err = mips_cpu_set_register(cpu, 4, 2147483647);
	err = mips_cpu_set_register(cpu, 5, 2147483647);
 
	// 3 - step CPU
	err = mips_cpu_step(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	// 4 -Check the result

	err = mips_cpu_get_register(cpu, 6, &got); 

	passed = got == 0;

	mips_test_end_test(testId, passed, "2147483647 + 2147483647 != Overflow");

	err = mips_cpu_reset(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}	

	// Standard Addiu test
	testId = mips_test_begin_test("addiu");    
 
	// 1 - Setup an instruction in ram
	// addiu r6, r4, 50
	instr = opcode(0x09) | rs(4) | rt(6) | data(50);	

	to_little(instr, buffer);
 
	err = mips_mem_write(mem, 0, 4, buffer);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
  
	// 2 - put register values in cpu
	err = mips_cpu_set_register(cpu, 4, 40);

	// 3 - step CPU
	err = mips_cpu_step(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	// 4 -Check the result
	

	err = mips_cpu_get_register(cpu, 6, &got); 

	passed = got == 40+50;

	mips_test_end_test(testId, passed, "40 + 50 != 90");

	err = mips_cpu_reset(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}

	// Overflow Addiu test
	testId = mips_test_begin_test("addiu");    
 
	// 1 - Setup an instruction in ram
	// addiu r6, r4, 50
	instr = opcode(0x09) | rs(4) | rt(6) | data(50);	

	to_little(instr, buffer);
 
	err = mips_mem_write(mem, 0, 4, buffer);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
  
	// 2 - put register values in cpu
	err = mips_cpu_set_register(cpu, 4, 40);
 
	// 3 - step CPU
	err = mips_cpu_step(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	// 4 -Check the result
	

	err = mips_cpu_get_register(cpu, 6, &got); 

	passed = (uint64_t)got == (uint32_t)(2147483647 + 2147483647);

	mips_test_end_test(testId, passed, "2147483647 + 2147483647 != 0xFFFFFFFF"); 

	err = mips_cpu_reset(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}

	testId = mips_test_begin_test("addu");    
 
	// 1 - Setup an instruction in ram
	// addiu r3, r4, r5
	instr = opcode(0) | rs(4) | rt(5) | rd(6) | shift(0) | func(0x21);	

	to_little(instr, buffer);
 
	err = mips_mem_write(mem, 0, 4, buffer);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
  
	// 2 - put register values in cpu
	err = mips_cpu_set_register(cpu, 4, 40);
	err = mips_cpu_set_register(cpu, 5, 50);
 
	// 3 - step CPU
	err = mips_cpu_step(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	// 4 -Check the result
	

	err = mips_cpu_get_register(cpu, 6, &got); 

	passed = got == 40+50;
 
	mips_test_end_test(testId, passed, "50 + 50 != 90"); 

	err = mips_cpu_reset(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}

	testId = mips_test_begin_test("and");    
 
	// 1 - Setup an instruction in ram
	// addu r3, r4, r5
	instr = opcode(0) | rs(4) | rt(5) | rd(6) | shift(0) | func(0x24);	

	to_little(instr, buffer);
 
	err = mips_mem_write(mem, 0, 4, buffer);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
  
	// 2 - put register values in cpu
	err = mips_cpu_set_register(cpu, 4, 40);
	err = mips_cpu_set_register(cpu, 5, 50);
 
	// 3 - step CPU
	err = mips_cpu_step(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	// 4 -Check the result
	

	err = mips_cpu_get_register(cpu, 6, &got); 

	passed = got == 40 & 50;

	mips_test_end_test(testId, passed, "40 & 50 != 32"); 

	err = mips_cpu_reset(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}

	testId = mips_test_begin_test("andi");    
 
	// 1 - Setup an instruction in ram
	// addu r3, r4, r5
	instr = opcode(0x0C) | rs(4) | rt(6) | data(50);	

	to_little(instr, buffer);
 
	err = mips_mem_write(mem, 0, 4, buffer);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	}
  
	// 2 - put register values in cpu
	err = mips_cpu_set_register(cpu, 4, 40);

	// 3 - step CPU
	err = mips_cpu_step(cpu);

	if(err != mips_Success)
	{
		fprintf(stderr, "mips_cpu_set_debug_level : failed.\n" );
		exit(EXIT_FAILURE);
	} 
 
	// 4 -Check the result
	
	err = mips_cpu_get_register(cpu, 6, &got); 

	passed = got == 40&50;

	mips_test_end_test(testId, passed, "40 & 50 != 32"); 
 
	mips_test_end_suite();

	return 0;
}

