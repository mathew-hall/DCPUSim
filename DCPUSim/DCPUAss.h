//
//  Header.h
//  DCPUSim
//
//  Created by Mathew Hall on 09/04/2012.
//  Copyright (c) 2012 University of Sheffield. All rights reserved.
//

#ifndef DCPUSim_Header_h
#define DCPUSim_Header_h

#include "DCPU.h"
#include "Fixup.h"

#define INST_JSR 0x0
#define INST_SET 0x1
#define INST_ADD 0x2
#define INST_SUB 0x3
#define INST_MUL 0x4
#define INST_DIV 0x5
#define INST_MOD 0x6
#define INST_SHL 0x7
#define INST_SHR 0x8
#define INST_AND 0x9
#define INST_BOR 0xa
#define INST_XOR 0xb
#define INST_IFE 0xc
#define INST_IFN 0xd
#define INST_IFG 0xe
#define INST_IFB 0xf

//usage
#define OP_REGISTER 1
#define OP_LITERAL  2
//both:
#define OP_OFFSET   OP_REGISTER | OP_LITERAL | OP_ADDRESS

//special cases
#define OP_POP		4
#define OP_PUSH		8
#define OP_PEEK		16

#define R_NONE 0xff

//is operand a value or an address?
#define OP_ADDRESS 32
//is it a ref to some label?
#define OP_REFERENCE 64

typedef struct _Instruction_Bytecode{
	//int id; //used as a label
	ushort address; //position of instruction in RAM, used for linking
	int size; //how many words the instruction is
	ushort words[3];
} Instruction_Bytecode;

typedef struct _Value{
	ushort operand;
	int size;
	ushort value;
} Value;

extern int test();
extern Instruction* parse_line(const char* line, int line_number, Fixup_Table* fixups, Label_Table* labels, ushort position);
extern Instruction_Bytecode* emit_instruction(Instruction* in);
#endif
