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

/*

 

 :testsub      SHL X, 4                 ; 9037
 SET PC, POP              ; 61c1
 
 ; Hang forever. X should now be 0x40 if everything went right.
 :crash        SET PC, crash            ; 7dc1 001a [*]
 */

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
	{{INST_JSR,_R(RPC),_L(0x18)},{0,1,{0xE010,0x0,0x0}}},
	{{INST_SET,{OP_REGISTER,RPC,0x0},{OP_LITERAL,0x0,0x1a}},{0,1,{0xe9c1,0x0,0x0}}}, //operand fits in one byte.
	{{INST_SHL,_R(RX),_L(4)},{0,1,{0x9037,0x0,0x0}}},
	{{INST_SET,_R(RPC),{OP_POP,0x0,0x0}},{0,1,{0x61c1,0x0,0x0}}},
	{{INST_SET,{OP_REGISTER,RPC,0x0},{OP_LITERAL,0x0,0x1a}},{0,1,{0xe9c1,0x0,0x0}}}
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

	test_codegen();

    return 0;
}

