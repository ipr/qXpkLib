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
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XPKLIBRARYBASE_H
#define XPKLIBRARYBASE_H

#include <QString>
#include "qxpklib.h"

#include "XpkCapabilities.h"
#include "XpkProgress.h"
#include "AnsiFile.h"


class xpkLibraryBase
{
protected:
	// only derived can be instantiated
	xpkLibraryBase(void) 
		: m_XpkCaps()
	{}
	virtual ~xpkLibraryBase(void)
	{}

	// capabilities of sub-library implementation
	//
	XpkCapabilities m_XpkCaps;
	
public:

	// access sub-library capabilities from master-library
	virtual XpkCapabilities *getCaps()
	{
		return &m_XpkCaps;
	}

	// pack/compress
	// (optional, so far nowhere..)
	//virtual bool Crunch(XpkProgress *pProgress)=0;
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};

#endif // XPKLIBRARYBASE_H
