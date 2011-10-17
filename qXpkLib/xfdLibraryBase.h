///////////////////////////////////
//
// Pure virtual interface-class
// for external "foreign/alien" decrunch handling.
//
// General information for XFD-libraries:
// - fileformat is "alien", non-XPK format
// - generally single packed file only (not collection)
// - decrunch as whole file, not in XPK-chunks
// - output to buffer only (in XpkProgress)
// -> master-library will either push to file or user buffer as needed
//


#ifndef XFDLIBRARYBASE_H
#define XFDLIBRARYBASE_H

#include <QString>


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
