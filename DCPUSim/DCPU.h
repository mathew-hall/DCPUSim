//
//  DCPU.h
//  DCPUSim
//
//  Created by Mathew Hall on 06/04/2012.
//  Copyright (c) 2012 Mathew Hall. All rights reserved.
//

#ifndef DCPUSim_DCPU_h
#define DCPUSim_DCPU_h

#define RA 0x0
#define RB 0x1
#define RC 0x2
#define RX 0x3
#define RY 0x4
#define RZ 0x5
#define RI 0x6
#define RJ 0x7
#define RSP 0x1b
#define RPC 0x1c
#define RO	0x1d

typedef unsigned short ushort;
#define RAM_SIZE 0x10000
typedef struct _DCPU{
	ushort A;
	ushort B;
	ushort C;
	ushort X;
	ushort Y;
	ushort Z;
	ushort I;
	ushort J;
	
	ushort PC;
	ushort SP;
	ushort O;
	
	ushort RAM[RAM_SIZE];
	
}DCPU;

extern void reset_regs(DCPU* cpu);
extern void step(DCPU* cpu);
extern void dump_state(DCPU* cpu);
extern void copy_rom(DCPU* cpu, ushort* data, ushort address, int size);


#endif
