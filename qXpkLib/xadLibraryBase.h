///////////////////////////////////
//
// Pure virtual interface-class
// for multi-file archive handling.
//
// General information for XAD-libraries:
// - fileformat is "alien", non-XPK format
// - generally multiple files as collection in archive
// - various crunching ways, not in XPK-chunks
// - some cases support individual file extraction,
// some cases need "file-group" extraction (e.g. LZX merged files)
// - output to buffer ? (in XpkProgress? selection required)
// - output directly to files ? (path required -> simple enough)
// -> these will need some more planning to support both ways..
// -> generally extract to files (all)
// -> may need user-selection to given buffer?
// -> special cases (differs from XPK/XFD here..)
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XADLIBRARYBASE_H
#define XADLIBRARYBASE_H

#include <QString>
#include "qxpklib.h" // for info-container only

#include "XpkCapabilities.h"
#include "XpkProgress.h"
#include "AnsiFile.h" // for IOBuffer only


class xadLibraryBase
{
protected:
	// only derived can be instantiated
	xadLibraryBase(void) 
		: m_XpkCaps()
		//, m_pDest(nullptr)
	{}
	virtual ~xadLibraryBase(void)
	{}
	
	// capabilities of sub-library implementation
	//
	XpkCapabilities m_XpkCaps;
	
	// for chaining of libraries:
	// output chunks of decompressed data
	// directly to another library
	// (given by master-library?)
	// -> better way needed,
	// library might want help from other library
	// and may have different base..
	//
	//xadLibraryBase *m_pDest;
	
public:

	// access sub-library capabilities from master-library
	virtual XpkCapabilities *getCaps()
	{
		return &m_XpkCaps;
	}

	// for detailed checking if format is supported..
	// some formats can be a pain to check in a generic way
    virtual bool isSupported(CIOBuffer *pInputBuffer)=0;

	// list files in archive, get other metadata also..
	// need input for library..
	virtual bool archiveInfo(CIoContext *pInput, QXpkLib::CArchiveInfo &info)=0;
	
	// test archive integrity
	// TODO: check parameters needed.. input at least.. 
	virtual bool testArchive(CIoContext *pInput)=0;
	
	// unpack/decompress,
	// need input and output and way for multi-file handling..
	//
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};

#endif // XADLIBRARYBASE_H
