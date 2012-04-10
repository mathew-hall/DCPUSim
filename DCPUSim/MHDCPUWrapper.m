//
//  MHDCPUWrapper.m
//  DCPUSim
//
//  Created by Mathew Hall on 07/04/2012.
//  Copyright (c) 2012 Mathew Hall. All rights reserved.
//

#import "MHDCPUWrapper.h"
#include "DCPU.h"

@implementation MHDCPUWrapper

@synthesize StackTableView = _StackTableView;
@synthesize RAMTableView = _RAMTableView;

@synthesize RegisterTextField = _RegisterTextField;

@synthesize ASCIIDumpColumn = _ASCIIDumpColumn;
@synthesize HexDumpColumn = _HexDumpColumn;
@synthesize AddressColumn = _AddressColumn;


-(MHDCPUWrapper*) init
{
	self = [super init];
	if(self){
		_cpu = malloc(sizeof(DCPU));
		reset_regs(_cpu);
	}
	return self;	
}


-(NSInteger) numberOfRowsInTableView: (NSTableView*) tableview
{
	if(tableview == _RAMTableView)
		return RAM_SIZE/4;
	if(tableview == _StackTableView){
//		NSLog(@"SP: %x",_cpu->SP);
		if(_cpu->SP == 0) return 0;
//		NSLog(@"returning %d rows for table when SP is %x",1+0xffff - _cpu->SP,_cpu->SP);
		return 1+0xffff - _cpu->SP;
	}
	return 0;
}


- (id) hexAtAddress:(ushort) address
{
	ushort* RAM = _cpu->RAM;
	return[NSString stringWithFormat:@"%04x %04x %04x %04x",
		   RAM[address],
		   RAM[address+1],
		   RAM[address+2],
		   RAM[address+3]
		   ];
}

- (id) ASCIIAtAddress:(ushort) address
{
	//TODO: informal compiler specs seem to call for one word per char rather than packing.
	//TODO: fix probable endian-ness caused bug.
	char *ascii = (char*) _cpu->RAM;
	return[NSString stringWithFormat:@"%c%c%c%c%c%c%c%c",
		   ascii[address], 
		   ascii[address+1],
		   ascii[address+2],
		   ascii[address+3],
		   ascii[address+4],
		   ascii[address+5],
		   ascii[address+6],
		   ascii[address+7]
		   ]; //HNNNG TODO: refactor

}

- (id)tableView:(NSTableView *)tableView
objectValueForTableColumn:(NSTableColumn *)tableColumn
			row:(NSInteger)row
{
	if(tableView == _RAMTableView){
		int start = row * 4;
	
		if(tableColumn == _HexDumpColumn)
			return [self hexAtAddress: start];
		else if(tableColumn == _ASCIIDumpColumn){
			return [self ASCIIAtAddress:start];
		}else{ //address:
			return [NSString stringWithFormat:@"%04x",start];
		}
	}else{
		//Stack table:
		//TODO:fix this, spec says push is val -> [--SP] so address needs adjusting.
		return [NSString stringWithFormat:@"0x%04x",_cpu->RAM[0xffff - row]];
	}
}


-(void) updateRegisters
{
	NSString* str = [NSString stringWithFormat:
					 @"PC: %04x, SP: %04x, O: %04x\nA: %04x, B: %04x, C%04x\nX: %04x, Y: %04x, Z %04x\nI: %04x, J: %04x",
					 _cpu->PC,
					 _cpu->SP,
					 _cpu->O,
					 _cpu->A,
					 _cpu->B,
					 _cpu->C,
					 _cpu->X,
					 _cpu->Y,
					 _cpu->Z,
					 _cpu->I,
					 _cpu->J
					 ];
	[_RegisterTextField setStringValue:str];
	//TODO: do this on-demand rather than every step
	[_StackTableView reloadData];
	[_RAMTableView reloadData];
}

-(void) step
{
	step(_cpu);
	[self updateRegisters];
}
-(void) reset
{
	reset_regs(_cpu);
	[self updateRegisters];
}

-(NSError*) loadWithContentsOfURL:(NSURL *)file
{
	NSError* error;
	NSData* data = [NSData dataWithContentsOfURL:file options:NSDataReadingUncached error:&error];
	if(data == nil){
		return error;
	}else{
		int size = [data length]/2; //length is in bytes, 2bytes to a short.
		if(size > RAM_SIZE) size = RAM_SIZE;
		copy_rom(_cpu, (ushort*) [data bytes], 0x0, size);
	}
	[self updateRegisters];
	[_RAMTableView reloadData];
	return nil;
}

-(void) dealloc
{
	free(_cpu);
}

@end
