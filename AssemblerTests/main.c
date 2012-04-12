//
//  main.c
//  AssemblerTests
//
//  Created by Mathew Hall on 10/04/2012.
//  Copyright (c) 2012 University of Sheffield. All rights reserved.
//

#include <stdio.h>
#include "DCPU.h"
#include "DCPUAss.h"

typedef struct _Expect_Instruction{
	Instruction inst;
	Instruction_Bytecode expect;
	
} Expect_Instruction;

typedef struct _Expect_Parser{
	char* input;
	Instruction expect;
} Expect_Parser;

#define _R(rnum) {OP_REGISTER,rnum,0x0}

#define _L(lnum) {OP_LITERAL, 0x0, lnum}
Expect_Instruction prog[] = {
	{{INST_SET,{OP_REGISTER,RA,0}, {OP_LITERAL,0,0x30}},{0,2,{0x7c01,0x0030,0x0}}},
	{{INST_SET,{OP_LITERAL|OP_ADDRESS,0,0x1000},{OP_LITERAL,0,0x20}},{0,3,{0x7de1,0x1000,0x20}}},
	{{INST_SUB,{OP_REGISTER,RA,0x0,},{OP_LITERAL|OP_ADDRESS,0,0x1000}},{0,2,{0x7803,0x1000,0x0}}},
	{{INST_IFN,{OP_REGISTER,RA,0x0},{OP_LITERAL,0x0,0x10}},{0,1,{0xc00d,0x0,0x0}}},
	{{INST_SET,{OP_REGISTER,RPC,0x0},{OP_LITERAL,0x0,0x1a}},{0,1,{0xe9c1,0x0,0x0}}}, //operand fits in one byte.
	{{INST_SET,_R(RI),_L(10)},{0,1,{0xa861}}},
	{{INST_SET,_R(RA),_L(0x2000)},{0,2,{0x7c01,0x2000}}},
	{{INST_SET,{OP_OFFSET,RI,0x2000},{OP_REGISTER|OP_ADDRESS,RA,0x0}},{0,2,{0x2161,0x2000,0x0}}},
	{{INST_SUB,_R(RI),_L(1)},{0,1,{0x8463,0x0,0x0}}},
	{{INST_IFN,_R(RI),_L(0)},{0,1,{0x806d,0x0,0x0}}},
	{{INST_SET,_R(RPC),_L(0xd)},{0,1,{0xB5c1,0x0,0x0}}},
	{{INST_SET,_R(RX), _L(0x4)},{0,1,{0x9031,0x0,0x0}}},
	{{INST_JSR,{0,0x0,0x1},_L(0x18)},{0,1,{0xE010,0x0,0x0}}},
	{{INST_SET,{OP_REGISTER,RPC,0x0},{OP_LITERAL,0x0,0x1a}},{0,1,{0xe9c1,0x0,0x0}}}, //operand fits in one byte.
	{{INST_SHL,_R(RX),_L(4)},{0,1,{0x9037,0x0,0x0}}},
	{{INST_SET,_R(RPC),{OP_POP,0x0,0x0}},{0,1,{0x61c1,0x0,0x0}}},
	{{INST_SET,{OP_REGISTER,RPC,0x0},{OP_LITERAL,0x0,0x1a}},{0,1,{0xe9c1,0x0,0x0}}}
};

const char* text[] = {
	"1 SET A, 0x30",
	"2	SET [0x1000], 0x20",
	"3 SUB A, [0x1000] ",
	"4 IFN A, 0x10",
	"5 SET PC, 0x1a",
	"6 SET I, 10",
	"7 SET A, 0x2000",
	"8 SET [I+0x2000], [A]",
	"9 SUB I, 1",
	"a IFN I, 0",
	"b SET PC, 0xd",
	"c SET X, 0x4",
	"d JSR 0x18",
	"e SET PC, 0x1a",
	"f SHL X, 4", //<- failure here
	"10 SET PC, POP",
	"11 SET PC, 0x1a"
};

int compare(Instruction_Bytecode* b1, Instruction_Bytecode* b2, int num){
	int ret= 0;
	if(b1->address != b2->address){
		printf("%d: Addresses differ: %x != %x\n",num,b1->address,b2->address);
		ret++;
	}
	if(b1->size != b2->size){
		printf("%d: Sizes differ: %x != %x\n",num,b1->size,b2->size);
		ret++;
	}
	for(int i =0; i < b1->size; i++){
		if(b1->words[i] != b2->words[i]){
			printf("%d: Words differ @ %d: %x != %x\n",num,i,b1->words[i],b2->words[i]);
			ret++;
		}
	}
	return ret;
}

int compare_operand(Operand* op1, Operand* op2, int num, char op){
	int count = 0;
	if(op1->reg != op2->reg){
		printf("%d%c: Registers differ: %x and %x\n",num,op,op1->reg,op2->reg);
		count++;
	}
	if(op1->literal !=op2->literal){
		printf("%d%c: Literal values differ: %x and %x\n",num,op,op1->literal,op2->literal);
		count++;
	}
	if(op1->type != op2->type){
		printf("%d%c: Types differ: %x and %x\n",num,op,op1->type,op2->type);
		count++;
	}
	return count;
}

int compare_instruction(Instruction* i1, Instruction* i2, int num){
	int count =0;
	if(i1->operator != i2->operator){
		printf("%d Operators differ: %x and %x\n",num, i1->operator, i2->operator);
		count++;
	}
	count += compare_operand(&i1->operanda, &i2->operanda,num,'a');
	count += compare_operand(&i1->operandb, &i2->operandb,num,'b');
	
	return count;
	
	
}

void test_parse(){
	int fails = 0;
	int num = sizeof(text)/sizeof(*text);
	for(int i =0; i<num; i++){
		Instruction* res = parse_line(text[i], i, new_fixup_table(), new_label_table(), 0);
		fails += compare_instruction(res, &prog[i].inst,i);
	}
	printf("\n%d tests run, %d failures\n",num,fails);
}

void test_codegen();
void test_codegen(){
	int fails = 0;
	int num = sizeof(prog)/sizeof(*prog);
	for(int i = 0; i<num; i++){
		Instruction_Bytecode* inst = emit_instruction(&prog[i].inst);
		fails += compare(inst, &prog[i].expect,i);
	}
	printf("\n%d tests run, %d failures\n",num,fails);
}

int main(int argc, const char * argv[])
{

//	test_codegen();
	test_parse();

    return 0;
}

