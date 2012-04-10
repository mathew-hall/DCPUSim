//
//  Fixup.h
//  DCPUSim
//
//  Created by Mathew Hall on 10/04/2012.
//  Copyright (c) 2012 University of Sheffield. All rights reserved.
//

#ifndef DCPUSim_Fixup_h
#define DCPUSim_Fixup_h

#include "DCPU.h"
#include "Types.h"

typedef struct _Fixup_Entry{
	const char* name;
	literal_value* target;
	struct _Fixup_Entry* next;
} Fixup_Entry;

typedef struct _Label_Entry{
	const char *name;
	ushort address;
	struct _Label_Entry* next;
} Label_Entry;

typedef Label_Entry Label_Table;
typedef Fixup_Entry Fixup_Table;

extern Label_Table* add_label(Label_Table* list, char* name, ushort address);
extern Label_Table* new_label_table();
extern Fixup_Table* add_fixup(Fixup_Table* list, char* name, literal_value* target);
extern Fixup_Table* new_fixup_table();
extern Fixup_Table* find(Fixup_Table* list, const char* name);
#endif
