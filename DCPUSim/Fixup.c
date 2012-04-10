//
//  Fixup.c
//  DCPUSim
//
//  Created by Mathew Hall on 10/04/2012.
//  Copyright (c) 2012 University of Sheffield. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "Fixup.h"
#include <string.h>

Fixup_Entry* find(Fixup_Table* table, const char* name){
	Fixup_Entry* cur = table;
	while(strcmp(cur->name,name)!= 0){
		cur = cur->next;
		if(cur == NULL) return NULL;
	}
	return cur;
}

Fixup_Table* new_fixup_table(){
	Fixup_Table* ret = malloc(sizeof(*ret));
	if(ret){
		ret->name = NULL;
		ret->next = NULL;
		ret->target = NULL;
	}
	return ret;
}

/**
 Add a new entry in the table.
 Return NULL if malloc failed, or the original table with the data saved somewhere in it.
 */
Fixup_Table* add_fixup(Fixup_Table* list, char* name, literal_value* target){
	if(list == NULL){
		list = new_fixup_table();
	}
	
	Fixup_Entry* current = list;
	while(current->next != NULL && current->name != NULL){
		current = current->next;
	}
	if(current->name != NULL){
		current->next=new_fixup_table();
		current = current->next;
		if(current == NULL) return NULL;
	}
	current->name = name;
	current->target = target;
	
	return list;
}

Label_Table* new_label_table(){
	Label_Table* ret = malloc(sizeof(*ret));
	
	if(ret){
		ret->name = NULL;
		ret->address = 0x0;
		ret->next = NULL;
	}
	
	return ret;
}

Label_Table* add_label(Label_Table* list, char* name, ushort address){
	if(list == NULL){
		list = new_label_table();
	}
	
	Label_Table* current = list;
	while(current-> next != NULL && current->name != NULL){
		current = current->next;
	}
	if(current->name != NULL){
		current->next = new_label_table();
		current = current->next;
		if(current == NULL) return NULL;
	}
	
	current->name = name;
	current->address = address;
	
	return list;
}
