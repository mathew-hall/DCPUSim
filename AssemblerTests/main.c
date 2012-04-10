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

#define NUM_CODEGEN_TESTS 2

typedef struct _Expect_Instruction{
	Instruction inst;
	Instruction_Bytecode expect;
	
} Expect_Instruction;

Expect_Instruction prog[] = {
	{{INST_SET,{OP_REGISTER,RA,0}, {OP_LITERAL,0,0x30}},{0,2,{0x7c01,0x0030,0x0}}},
	{{INST_SET,{OP_LITERAL|OP_ADDRESS,0,0x1000},{OP_LITERAL,0,0x20}},{0,3,{0x7de1,0x1000,0x20}}}	
};

int compare(Instruction_Bytecode* b1, Instruction_Bytecode* b2, int num){
	if(b1->address != b2->address){
		printf("%d: Addresses differ: %x != %x\n",num,b1->address,b2->address);
	}
	if(b1->size != b2->size){
		printf("%d: Sizes differ: %x != %x\n",num,b1->size,b2->size);
	}
	for(int i =0; i < b1->size; i++){
		if(b1->words[i] != b2->words[i]){
			printf("%d: Words differ @ %d: %x != %x",num,i,b1->words[i],b2->words[i]);
		}
	}
	return 0;
}

void test_codegen();
void test_codegen(){
	for(int i = 0; i<NUM_CODEGEN_TESTS; i++){
		Instruction_Bytecode* inst = emit_instruction(&prog[i].inst);
		compare(inst, &prog[i].expect,i);
	}
}

int main(int argc, const char * argv[])
{

	test_codegen();
	printf("Hello, World!\n");
    return 0;
}

