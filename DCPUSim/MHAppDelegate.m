//
//  MHAppDelegate.m
//  DCPUSim
//
//  Created by Mathew Hall on 06/04/2012.
//  Copyright (c) 2012 Mathew Hall. All rights reserved.
//

#import "MHAppDelegate.h"
#import "MHDCPUWrapper.h"
#include "DCPU.h"


@implementation MHAppDelegate


@synthesize window = _window;

@synthesize RAMTableView = _RAMTableView;
@synthesize StackTableView = _StackTableView;

@synthesize RegisterTextField = _RegisterTextField;

@synthesize AddressColumn = _AddressColumn;
@synthesize HexDumpColumn = _HexDumpColumn;
@synthesize ASCIIDumpColumn = _ASCIIDumpColumn;

MHDCPUWrapper* _cpu;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	_cpu = [[MHDCPUWrapper alloc] init];
	_cpu.RAMTableView = _RAMTableView;
	_cpu.StackTableView = _StackTableView;
	_cpu.HexDumpColumn = _HexDumpColumn;
	_cpu.ASCIIDumpColumn = _ASCIIDumpColumn;
	_cpu.RegisterTextField = _RegisterTextField;

	[_RAMTableView setDataSource:_cpu];
	[_StackTableView setDataSource:_cpu];
}

- (IBAction)stepWasPressed:(id)sender {
	[_cpu step];
}

- (IBAction)resetWasPressed:(id)sender {
	[_cpu reset];
}

- (IBAction)openWasPressed:(id)sender {
	NSOpenPanel *openDialog = [NSOpenPanel openPanel];
	
	[openDialog setCanChooseFiles:YES];
	[openDialog setCanChooseDirectories:NO];
	
	[openDialog setAllowsMultipleSelection:NO];
	
	[openDialog beginSheetModalForWindow:_window completionHandler:^(NSInteger result) {
		if(result == NSOKButton){
			NSArray* urls = [openDialog URLs];
			NSURL* file = [urls objectAtIndex:0];

			NSError* ret = [_cpu loadWithContentsOfURL: file];
			if(ret != nil){
				[[NSAlert alertWithError:ret] runModal];
			}
		}
	}];
	
}
@end
