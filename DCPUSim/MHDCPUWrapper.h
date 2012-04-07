//
//  MHDCPUWrapper.h
//  DCPUSim
//
//  Created by Mathew Hall on 07/04/2012.
//  Copyright (c) 2012 Mathew Hall. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DCPU.h"
@interface MHDCPUWrapper : NSObject <NSTableViewDataSource>{
	DCPU* _cpu;	
	
}
@property (assign) IBOutlet NSTableView *StackTableView;
@property (assign) IBOutlet NSTableView *RAMTableView;

@property (assign) IBOutlet NSTextField *RegisterTextField;

@property (assign) IBOutlet NSTableColumn *HexDumpColumn;
@property (assign) IBOutlet NSTableColumn *ASCIIDumpColumn;
@property (assign) IBOutlet NSTableColumn *AddressColumn;

-(NSInteger) numberOfRowsInTableView: (NSTableView*) tableview;
- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn
row:(NSInteger)row;


-(void) updateRegisters;
-(void) step;
-(void) reset;

-(NSError*) loadWithContentsOfURL: (NSURL*) file;

@end
