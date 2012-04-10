//
//  DCPUAss.c
//  DCPUSim
//
//  Created by Mathew Hall on 08/04/2012.
//  Copyright (c) 2012 University of Sheffield. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "OperatorLUT.h"
#include "Fixup.h"

#include "DCPU.h"
#include "DCPUAss.h"



Value* encode_operand(Operand* in){
	operand_type type = in->type;
	
	Value* ret = calloc(1, sizeof(*ret));

	if(type & OP_LITERAL){
		ret->value = in->literal;
		ret->size = 1;
	}

	if(type & OP_REGISTER){
		ushort retVal = 0;
		retVal = in->reg;
		if(type & OP_ADDRESS){
			retVal += 0x8;
		}
		if(type & OP_LITERAL){
			retVal += 0x8;
		}
		ret->operand = retVal;
	}else if(type == OP_POP){
		ret->operand = 0x18;
	}else if(type == OP_PEEK){
		ret->operand = 0x19;
	}else if(type == OP_PUSH){
		ret->operand = 0x1a;
	}else if(type & OP_LITERAL){
		if(type & OP_ADDRESS){
			ret->operand = 0x1e;
		}else{
			if(in->literal <= 0x1f){
				ret->operand = in->literal+0x20;
				ret->size = 0;
			}else{
				ret->operand = 0x1f;
			}
				
		}
	}
	

	return ret;
	
}

void assemble(){
	//walk down list
	//assemble, if missing an address, replace with ffff, push a pointer to fixup list:
	//SYMBOL -> [*fixup1,*fixup2]
	//on hitting a label, add symbol and address of its instruction to a symbol table:
	//SYMBOL -> *instr
	//walk down fixup list replacing ffff with symbol_table[SYMBOL]
	
}


Instruction_Bytecode* emit_instruction(Instruction* in){
	Instruction_Bytecode* ret = calloc(sizeof(*ret),1);
	ret->size = 1;
	
	ushort* instruction_words = ret->words;
	instruction_words[0] = in->operator & 0xf;
	
	Value* opa = encode_operand(&in->operanda);
	Value* opb = encode_operand(&in->operandb);
	
	if(in->operator == 0x0){
		//for JSR and the like, operand a is the opcode,
		//so just put it verbatim into the instruction.
		opa->size = 0;
		opa->operand = in->operanda.literal;
	}
	
	instruction_words[0] |= (opa->operand & 0x3f) << 4;
	instruction_words[0] |= (opb->operand & 0x3f) <<10;
	
	int next = 1;
	if(opa->size == 1){
		instruction_words[next] = opa->value;
		next++;
		ret->size = next;
	}
	if(opb->size == 1){
		instruction_words[next] = opb->value;
		next++;
		ret->size = next;
	}
	
	return ret;
}

int test(){
	Instruction_Bytecode* i = emit_instruction(parse_line("hello\tSUB I, 1 ", 0, new_fixup_table(), new_label_table(),0 ) );
	for(int j=0;j<i->size; j++){
		printf("%04x ",i->words[j]);
	}
	/*	Instruction prog[] = {
	{SET,{OP_REGISTER,RA,0}, {OP_LITERAL,0,0x30}},
	{SET,{OP_LITERAL|OP_ADDRESS,0,0x1000},{OP_LITERAL,0,0x20}}	
	};
	for(int i =0; i<2; i++){
		Instruction_Bytecode inst = emit_instruction(prog[i]);
		for(int j=0;j<inst.size; j++){
			printf("%04x ",inst.words[j]);
		}
		printf("\n");
	}
*/	
	printf("\n\n");
	return 0;
}

