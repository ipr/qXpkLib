///////////////////////////////////
//
// Pure virtual interface-class
// for each decoder-library.
//


#ifndef XPKLIBRARYBASE_H
#define XPKLIBRARYBASE_H

#include "XpkProgress.h"
#include "XpkTags.h"

#include "AnsiFile.h"


class xpkLibraryBase
{
protected:
	// only inherited can be made instance of
	xpkLibraryBase(void) 
	{}
	virtual ~xpkLibraryBase(void)
	{}
	
public:

	//virtual QString GetPackerName()=0;
	//virtual QString GetPackerLongName()=0;

	// pack/compress
	virtual bool Crunch(XpkProgress *pProgress)=0;
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};


#endif // XPKLIBRARYBASE_H
