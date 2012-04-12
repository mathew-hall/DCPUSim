//
//  DCPUAssParser.c
//  DCPUSim
//
//  Created by Mathew Hall on 09/04/2012.
//  Copyright (c) 2012 University of Sheffield. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "DCPUAss.h"
#include "DCPU.h"
#include "OperatorLUT.h"
#include "Fixup.h"



/*
 Splits input into up to 4 strings:
 [0] -> label ('\0' if no label)
 [1] -> operator
 [2] -> operand 1 (INCLUDING any , etc)
 [3] -> operand 2 ('\0') if none present.
 
 MUST be free()d after it's been used.
 
 */
char** split(const char *in, int line){
	int size = strlen(in) + 1; //len(in) + \0
	char* copy = malloc(size * sizeof(*copy));
	if(copy == NULL) return NULL;
	strncpy(copy, in, size);
	
	//truncate comment
	char* comment_start = strchr(copy, ';');
	if(comment_start){
		*comment_start = '\0';
		realloc(copy, comment_start - copy + 1);
	}
	
	char** buf = &copy;
	char* res;
	
	char** ret = malloc(4 * sizeof(*ret));
	int idx = 0;
	for(idx = 0; idx<4; idx++){
		res = strsep(buf, " \t");
		if(res != NULL){
			ret[idx] = res;
		}else if(idx < 3){
			printf("[%d] Error: wrong number of input symbols, must be at least 3. \"%s\" has %d.",line,in,idx);
			exit(-1);
		}
	}
	
	return ret;
	
}
int instruction_length(Instruction* ins){
	int size = 1;
	if(ins->operanda.type & OP_LITERAL) size++;
	if(ins->operandb.type & OP_LITERAL) size++;
	return size;
}
register_name decode_reg(char name){
	switch(name){
		case 'A':
		case 'B':
		case 'C':
			return name - 'A';
		case 'X':
		case 'Y':
		case 'Z':
			return name - 'X' + 3;
		case 'I':
		case 'J':
			return name - 'I' + 6;
		case 'O':
			return RO;
		case 'P':
			return RPC;
		case 'S':
			return RSP;
		default:
			return R_NONE;
	}
}


bool parse_literal(const char* in, Operand* destination){
	if(in[0] == '0' && in[1] == 'x' && sscanf(in, "%x", &destination->literal)){
		destination->type |= OP_LITERAL;
		return true;
	}
	else
		if(sscanf(in, "%d", &destination->literal)){
			destination->type |= OP_LITERAL;
			return true;
		}
	return false;
	
}

//TODO: make sure PC, etc. are handled - it's not possible to use these with indirect addressing
Operand* parse_operand(const char* operand, int line_number){
	int len = strlen(operand);
	Operand* ret = calloc(1,sizeof *ret);

	char* rbkt = strchr(operand,']');
	
	if(len == 0) return NULL; //probably unused (e.g. this is operand B of a JSR)
	if(*operand == '['){
		if(!rbkt){
			printf("[%d] Error: unmatched '[' character", line_number);
			exit(-1);
		}
		operand++;

		ret->type = OP_ADDRESS;
	}
	
	//TODO get rid of , from second reg

	
	register_name reg = decode_reg(*operand);
	
	if(strchr(operand,'+')){
		
		char* plusChar = strchr(operand,'+');
		
		char* a = calloc(1,(plusChar -operand) * sizeof(*a));
		strncpy(a, operand,plusChar-operand);

		char *b = malloc((rbkt - plusChar) * sizeof(*b));
		plusChar++;
		strncpy(b, plusChar,rbkt-plusChar);
		
		Operand* temp = parse_operand(a, line_number);
		
		if(temp){
			ret->type |= temp->type;
			if(temp->type & OP_REGISTER)
				ret->reg = temp->reg;
			else if(temp->type & OP_LITERAL)
				ret->literal = temp->literal;
			else {
				printf("[%d] Error: part of relative address couldn't be parsed: %s",line_number,a);
			}
			free(temp);
		}
		
		temp = parse_operand(b, line_number);
		
		if(temp){
			if(temp->type & ret-> type & OP_REGISTER || temp->type & ret->type & OP_LITERAL){
				printf("[%d] Error: ralative addresses have to be reg + address",line_number);
				exit(-1);
			}
			ret->type |= temp->type;
			if(temp->type & OP_REGISTER){
				ret->reg = temp->reg;
			}else if(temp->type & OP_LITERAL){
				ret->literal = temp->literal;
			}else {
				printf("[%d] Error: part of relative address couldn't be parsed: %s",line_number,b);
				exit(-1);
			}
			free(temp);
		}
		
	}if(strcmp(operand,"PEEK") == 0){
		ret->type = OP_PEEK;
	}else if(strcmp(operand,"POP")  == 0){
		ret->type = OP_POP;
	}else if(strcmp(operand,"PUSH") == 0){
		ret->type = OP_PUSH;
	}else if(reg != R_NONE){
		ret->type |= OP_REGISTER;
		ret->reg = reg;
	}else if(parse_literal(operand, ret)){
		//parse literal handles flags so this is8 just a nop
	}else{
		ret->type |= OP_LITERAL | OP_REFERENCE;
		ret->literal = 0xBEEF;
	}
	
	return ret;
}

//TODO: add offset param calculated by sum(size[instruction_0..instruction_n-1])
//TODO: reject invalid operations [PC] isn't legal for example

Instruction* parse_line(const char* line, int line_number, Fixup_Table* fixups, Label_Table* labels, ushort position){
	
	Instruction* ret = malloc(sizeof(*ret));
	
	char** tokens = split(line,line_number);
	char* label	   = tokens[0];
	char* operator = tokens[1];
	char* operanda = tokens[2];
	char* operandb = tokens[3];
	
	const struct OperatorName* opn = instruction_lookup(operator,strlen(operator));
	if(opn == NULL){
		printf("[%d] Error: operator \"%s\" not valid.",line_number,operator);
		exit(-1);
	}
	ret->operator = opn->code;
	
	Operand* opa = parse_operand(operanda, line_number);
	Operand* opb = NULL;

	
	if(opn->code == INST_JSR){
		opb = malloc(sizeof(*opb));
		if(!opb){
			printf("Failed to malloc for an operand struct\n");
			exit(-1);
		}
		opb->literal = opa->literal;
		opb->reg = opa->reg;
		opb->type = opa->type;
		
		opa->literal = 0x01; //JSR
		opa->reg = 0;
		opa->type = 0;
	}else{
		opb = parse_operand(operandb, line_number);
	}
	
	if(*label){
		//Add label & offset to table
		add_label(labels, label, position);
	}
	
	if(opa != NULL) {
		ret->operanda = *opa;
		if(ret->operanda.type | OP_REFERENCE){
			if(!add_fixup(fixups, operanda, &ret->operanda.literal)){
				printf("[%d] Error: couldn't add operand %s to fixup table\n",line_number,operanda);
				exit(-1);
			}
		}
	}if(opb != NULL) {
		ret->operandb = *opb;
		if(ret->operandb.type | OP_REFERENCE){
			if(!add_fixup(fixups, operandb, &ret->operandb.literal)){
				printf("[%d] Error: couldn't add operand %s to fixup table\n",line_number,operandb);
				exit(-1);
			}
		}
	}
	
	
	realloc(label, strlen(label)+1);
	free(tokens);
	
	
	return ret;
}

#define INIT_PROG_SIZE 20
Instruction** parse_file(FILE* fp, int* error){
	char* buf = malloc(80 * sizeof(*buf));
	int buff_sz = INIT_PROG_SIZE;
	Instruction** destination = malloc(sizeof(*destination) * INIT_PROG_SIZE);
	
	int count = 0;
	Fixup_Table* fixups = new_fixup_table();
	Label_Table* labels = new_label_table();

	ushort pos = 0;
	while((buf = fgets(buf,80,fp)) != NULL ){
		Instruction* i = parse_line(buf,count+1,fixups,labels,pos);
		pos+= instruction_length(i);
		destination[count] = i;
		count++;
		if(count == buff_sz){
			Instruction** d = realloc(destination, (buff_sz + INIT_PROG_SIZE) * sizeof(*d));
			if(d != NULL){
				destination = d;
			}else{
				printf("Couldn't realloc buffer, read %d lines.",count);
				exit(-1);
			}
		}
	}
	
	Label_Entry* current = labels;
	while(current != NULL){
		Fixup_Entry* cur_fixup = fixups;
		while(cur_fixup != NULL){
			cur_fixup = find(cur_fixup,current->name);
			if(cur_fixup){
				*(cur_fixup->target) = current->address;
				cur_fixup = cur_fixup->next;
			}
		}
		current = current->next;
	}

	return destination;

}