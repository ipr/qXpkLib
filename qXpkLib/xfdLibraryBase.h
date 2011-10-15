///////////////////////////////////
//
// Pure virtual interface-class
// for external "foreign/alien" decrunch handling.
//


#ifndef XFDLIBRARYBASE_H
#define XFDLIBRARYBASE_H

#include "XpkProgress.h"

#include "AnsiFile.h"


class xfdLibraryBase
{
protected:
	// only derived can be instantiated
	xfdLibraryBase(void) 
	{}
	virtual ~xfdLibraryBase(void)
	{}
	
public:

	//virtual QString GetPackerName()=0;
	//virtual QString GetPackerLongName()=0;

	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};


#endif // XFDLIBRARYBASE_H
