//
//  MHAppDelegate.h
//  DCPUSim
//
//  Created by Mathew Hall on 06/04/2012.
//  Copyright (c) 2012 Mathew Hall. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface MHAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;

@property (assign) IBOutlet NSTableView *RAMTableView;
@property (assign) IBOutlet NSTableView *StackTableView;

@property (assign) IBOutlet NSTextField *RegisterTextField;

@property (assign) IBOutlet NSTableColumn *AddressColumn;
@property (assign) IBOutlet NSTableColumn *HexDumpColumn;
@property (assign) IBOutlet NSTableColumn *ASCIIDumpColumn;


- (IBAction)stepWasPressed:(id)sender;
- (IBAction)resetWasPressed:(id)sender;
- (IBAction)openWasPressed:(id)sender;



@end
