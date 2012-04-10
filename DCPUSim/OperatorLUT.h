//
//  OperatorLUT.h
//  DCPUSim
//
//  Created by Mathew Hall on 09/04/2012.
//  Copyright (c) 2012 University of Sheffield. All rights reserved.
//

#ifndef DCPUSim_OperatorLUT_h
#define DCPUSim_OperatorLUT_h

struct OperatorName
{
	const char *name;
	int   code;
};

extern const struct OperatorName * instruction_lookup (register const char* str, register unsigned int len);


#endif
