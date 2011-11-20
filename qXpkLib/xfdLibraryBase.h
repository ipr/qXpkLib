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
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XFDLIBRARYBASE_H
#define XFDLIBRARYBASE_H

#include <QString>
#include "qxpklib.h" // for info-container only

#include "XpkCapabilities.h"
#include "XpkProgress.h"
#include "AnsiFile.h" // for IOBuffer only


class xfdLibraryBase
{
protected:
	// only derived can be instantiated
	xfdLibraryBase(void) 
		: m_XpkCaps()
	{}
	virtual ~xfdLibraryBase(void)
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

	// for detailed checking if format is supported..
	// some formats can be a pain to check in a generic way
    virtual bool isSupported(CIOBuffer *pInputBuffer)=0;

	// list compressed file information..
	virtual bool archiveInfo(CIoContext *pInput, QXpkLib::CArchiveInfo &info)=0;

	// test archive integrity,
	// mostly just test of decompression for error detection, can we use it generally here?
	// TODO: check parameters needed.. input at least.. 
	//virtual bool testArchive(CIoContext *pInput)=0;

	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};

#endif // XFDLIBRARYBASE_H
