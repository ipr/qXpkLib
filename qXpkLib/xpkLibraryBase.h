///////////////////////////////////
//
// Pure virtual interface-class
// for each decoder-library.
//
// General information for XPK-libraries:
// - fileformat is XPK-native format
// - generally single packed file only (not collection)
// - decrunch as XPK-chunks
// - output to buffer only (in XpkProgress)
// -> master-library will either push to file or user buffer as needed
//


#ifndef XPKLIBRARYBASE_H
#define XPKLIBRARYBASE_H

#include <QString>


#include "XpkProgress.h"
#include "AnsiFile.h"


class xpkLibraryBase
{
protected:
	// only derived can be instantiated
	xpkLibraryBase(void) 
	{}
	virtual ~xpkLibraryBase(void)
	{}
	
public:

	//virtual QString GetPackerName()=0;
	//virtual QString GetPackerLongName()=0;

	// pack/compress
	// (optional, so far nowhere..)
	//virtual bool Crunch(XpkProgress *pProgress)=0;
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};


#endif // XPKLIBRARYBASE_H
