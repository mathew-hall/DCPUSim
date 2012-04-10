//
//  Types.h
//  DCPUSim
//
//  Created by Mathew Hall on 10/04/2012.
//  Copyright (c) 2012 University of Sheffield. All rights reserved.
//

#ifndef DCPUSim_Types_h
#define DCPUSim_Types_h

typedef int operator;
typedef unsigned char register_name;
typedef ushort literal_value;
typedef unsigned char operand_type;

typedef struct _operand{
	operand_type type;
	register_name reg;
	literal_value literal;
} Operand;

typedef struct _instruction{
	operator operator;
	Operand  operanda;
	Operand  operandb;
} Instruction;


#endif
