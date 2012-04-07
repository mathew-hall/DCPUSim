//
//  main.m
//  DCPUSim
//
//  Created by Mathew Hall on 06/04/2012.
//  Copyright (c) 2012 Mathew Hall. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DCPU.h"
int main(int argc, char *argv[])
{
//	return NSApplicationMain(argc, (const char **)argv);
	
	//kludgy on the fly testing:
	DCPU cpu;
	ushort prog[] = {
		0x7c01, 0x0030, 0x7de1, 0x1000, 0x0020, 0x7803, 0x1000, 0xc00d
	};
	printf("Copying ROM...\n");
	copy_rom(&cpu, &prog[0], 0x0, sizeof(prog));
	printf("ROM copied. Setting resgisters to initial values...\n");
	reset_regs(&cpu);
	printf("Registers initialised, stepping\n");
	step(&cpu);
	dump_state(&cpu);
	
}
