//
//  DCPU.c
//  DCPUSim
//
//  Created by Mathew Hall on 06/04/2012.
//  Copyright (c) 2012 Mathew Hall. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "DCPU.h"

#define DBG printf("\nL:%d, %x ",__LINE__,cpu->PC); printf

//decode opcodes & operands:
#define OPCODE(inst) inst&0xf
#define VALA(inst) ((inst >> 4) & 0x3f)
#define VALB(inst) ((inst >> 10) & 0x3f)

//flags for value types:
#define LITERAL  0x10000
#define ADDRESS  0x20000
#define REGISTER 0x40000
#define OOPS	 0x80000

//take val | flags in int to val in a ushort
#define VALUE(value) (ushort)(value & 0xFFFF)



void reset_regs(DCPU* cpu){
	cpu->A = 0;
	cpu->B = 0;
	cpu->C = 0;
	
	cpu->X = 0;
	cpu->Y = 0;
	cpu->Z = 0;
	
	cpu->J = 0;
	cpu->I = 0;
	
	cpu->PC = 0;
	cpu->O = 0;
	cpu->SP = 0;
}

void copy_rom(DCPU* cpu, ushort* data, ushort address, int size){
	ushort *dest = &(cpu->RAM[address]);
	//don't overflow if size is too big:
	if(address + size > RAM_SIZE){
		size = RAM_SIZE - address;
	}
	while(size--){
		*dest++ = *data++;
	}
	
}

#define _DR(cpu,name) printf(#name":\t%x, ",cpu->name);
void dump_state(DCPU* cpu){
	printf("{");
	printf("\tPC: %x [PC]: %x\n",cpu->PC, cpu->RAM[cpu->PC]);
	printf("\tSP: %x [SP]: %x\n",cpu->SP, cpu->RAM[cpu->SP]);
	_DR(cpu,O)
	printf("\nGPRs: \n");
	_DR(cpu,A)
	_DR(cpu,B)
	_DR(cpu,C)
	printf("\n");
	_DR(cpu,X)
	_DR(cpu,Y)
	_DR(cpu,Z)
	printf("\n");
	_DR(cpu,I)
	_DR(cpu,J)
	printf("\n}\n");
	
}

//Use a macro to generate each case instead of manually writing them out:
#define _LUT(regname, reg) case regname: return cpu->reg;
ushort fetch_register(ushort val, DCPU* cpu){
	switch(val){
			_LUT(RA,A)
			_LUT(RB,B)
			_LUT(RC,C)
			_LUT(RX,X)
			_LUT(RY,Y)
			_LUT(RZ,Z)
			_LUT(RI,I)
			_LUT(RJ,J)
			_LUT(RSP,SP)
			_LUT(RPC,PC)
			_LUT(RO,O)
	}
	return(0xffff);
}

/*
 Decode an operand into the source/target:
	Returns:
		REGISTER | register ID as defined by instruction, eg
		0100 0000 0111 ; REGISTER #7 (RJ)
	
		ADDRESS  | address in RAM
		0010 0000 0000 ; ADDRESS 0x0
 
		LITERAL  | literal value
		0001 0000 0101 ; LITERAL 0x5
		
 */
unsigned int decode_destination(ushort val, DCPU* cpu){
	unsigned int retval = 0;
	
	ushort* RAM = cpu->RAM;
	
	if(val <= 0x7){
		return val | REGISTER;
	}
	if(val <= 0x17){ //register ref:
		retval = fetch_register(val % 0x8, cpu);
		if(val >= 0x8 && val <= 0x0f){ 
			//dereference reg:
			return retval | ADDRESS;
		}else{
			//relative address
			return retval+RAM[cpu->PC++] | ADDRESS;
		}
	}
	if(val >= 0x20 && val <= 0x3f){
		//literal val:
		retval = val - 0x20;
		return retval | LITERAL;
	}
	
	switch(val){
		case 0x18:
			return cpu->SP++ | ADDRESS;

		case 0x19:
			return cpu->SP   | ADDRESS;

		case 0x1a:
			cpu->SP--;
			return cpu->SP   | ADDRESS;

		case 0x1b:
		case 0x1c:
		case 0x1d:
			return val | REGISTER;

		case 0x1e:
			return RAM[cpu->PC++] | ADDRESS;

		case 0x1f:
			return RAM[cpu->PC++] | LITERAL;
	}
	return OOPS;
}

ushort get_value(int value, DCPU* cpu){
	if(value & LITERAL){
		return VALUE(value);
	}
	if(value & REGISTER){
		return fetch_register(VALUE(value), cpu);
	}
	if(value & ADDRESS){
		return cpu->RAM[VALUE(value)];
	}
	return 0x0BAD;
}

#define _WT(regname,reg) case regname: cpu->reg = value; break;
void write_value(int dest, DCPU* cpu, ushort value){
	if(dest & LITERAL){
		return; //silently fail when trying to write to a literal
	}
	if(dest & ADDRESS){
		cpu->RAM[VALUE(dest)] = value;
		return;
	}
	if(dest & REGISTER){
		switch(VALUE(dest)){
				_WT(RA,A)
				_WT(RB,B)
				_WT(RC,C)
				_WT(RX,X)
				_WT(RY,Y)
				_WT(RZ,Z)
				_WT(RI,I)
				_WT(RJ,J)
				_WT(RSP,SP)
				_WT(RPC,PC)
				_WT(RO,O)
		}
	}
}
#define HAS_OPERAND(val) (val >= 0x10 && val <= 0x17) || val == 0x1e || val == 0x1f
/*
 Used in conditional handling; figure out how long the instruction is
 so the PC can be fixed up to point to the instruction after it.
 */
int instruction_size(ushort instruction){
	int size = 1;
	if(HAS_OPERAND(VALA(instruction)))size++;
	if(HAS_OPERAND(VALB(instruction)))size++;
	return size;
}


#define IL(opcode,operanda,operandb) printf("op:%s a:%x b:%x\n",opcode, operanda, operandb)
#define set_carry() cpu->O = (r >> 16) & 0xffff
void step(DCPU* cpu){
	ushort instruction = cpu->RAM[cpu->PC++];
	ushort opcode = OPCODE(instruction);
	int operanda = decode_destination(VALA(instruction), cpu);
	int operandb = decode_destination(VALB(instruction), cpu);
	
	int r; //intermediate value used to store packed value + flags.
	
	ushort val_a = 0;
	if(opcode != 0x0){ 
		//A is an opcode for non-basic mode; don't clobber RAM if that's the case
		val_a = get_value(operanda,cpu);
	}
	ushort val_b = get_value(operandb,cpu);
	
	int next_inst_sz = instruction_size(cpu->RAM[cpu->PC]);
	
	bool is_conditional = false; //Is the instruction a conditional?
	bool conditional_value = false; //if so, was the condition true or false?

	bool writeback = true;	
	ushort value;
	
	switch(opcode){
		case 0x0:
			//non-basic:
			//6 bit opcode in A
			switch(VALA(instruction)){
				case 0x01:
					//JSR
					writeback = false;
					cpu->SP--;
					cpu->RAM[cpu->SP] = cpu->PC;
					cpu->PC = val_b;
					break;
			}
			
			break;
		case 0x1:
			//SET
			value = val_b;
			break;
		/* Arithmetic: */
		case 0x2:
			//ADD
			r = val_a + val_b;
			value = VALUE(r);
			set_carry();
			break;
		case 0x3:
			//SUB
			r = val_a - val_b;
			value = VALUE(r);
			set_carry();
			break;
		case 0x4:
			//MUL
			r = val_a * val_b;
			value = VALUE(r);
			set_carry();
			break;
		case 0x5:
			//DIV
			if(val_b == 0){
				value = 0;
				cpu->O = 0;
				break;
			}			
			r = val_a / val_b;
			value = VALUE(r);
			r = (val_a << 16)/val_b;
			//O = a<<16/b & 0xffff
			cpu->O = VALUE(r);
			break;
		case 0x6:
			//MOD
			if(val_b == 0){
				val_a = 0;
				break;
			}
			r = val_a % val_b;
			value = VALUE(r);
			break;
		/* Shifts: */
		case 0x7:
			//SHL
			r = val_a << val_b;
			value = VALUE(r);
			set_carry();
			break;
		case 0x8:
			//SHR
			r = val_a >> val_b;
			value = VALUE(r);
			r = (val_a << 16)>>val_b;
			cpu->O = VALUE(r);
			break;
		/* Logical operators: */
		case 0x9:
			//AND
			value = val_a & val_b;
			break;
		case 0xa:
			//BOR
			value = val_a | val_b;
			break;
		case 0xb:
			//XOR
			value = val_a ^ val_b;
			break;
		/* Conditionals: */
		case 0xc:
			//IFE
			is_conditional = true;
			conditional_value = val_a == val_b;
			break;
		case 0xd:
			//IFN
			is_conditional = true;
			conditional_value = val_a != val_b;
			break;
		case 0xe:
			//IFG
			is_conditional = true;
			conditional_value = val_a > val_b;
			break;
		case 0xf:
			//IFB
			is_conditional = true;
			conditional_value = (val_a & val_b) != 0;
			break;
	}
	
	if (is_conditional) {
		if(!conditional_value){
			//skip next instruction if cond == false
			cpu->PC += next_inst_sz;
		}
		writeback = false;
	}
	
	if(writeback){
		write_value(operanda, cpu, value);
	}
	
}