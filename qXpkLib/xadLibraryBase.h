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


#ifndef XADLIBRARYBASE_H
#define XADLIBRARYBASE_H

#include <QString>
#include "qxpklib.h"

#include "XpkCapabilities.h"
#include "XpkProgress.h"
#include "AnsiFile.h"


// fwd. decl.
class xadLibraryBase;

class xadLibraryBase
{
protected:
	// only derived can be instantiated
	xadLibraryBase(void) 
		: m_XpkCaps()
		, m_pDest(nullptr)
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
	//
	xadLibraryBase *m_pDest;
	
public:

	// access sub-library capabilities from master-library
	virtual XpkCapabilities *getCaps()
	{
		return &m_XpkCaps;
	}

	// set sub-library destination (optional, if supported)
	virtual bool setDestLibrary(xadLibraryBase *pDest) 
	{
		return false;
	}

	// should be given in XpkProgress-object?
	virtual bool setArchive(QString &szArchive)=0;

	// list files in archive, get other metadata also..
	virtual bool archiveInfo(QXpkLib::CArchiveInfo &info)=0;
	
	// set path to uncompress files to
	// (optionally specific file name if only one output?)
	virtual bool setExtractPath(QString &szPath)=0;
	
	// test archive integrity
	virtual bool testArchive()=0;
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};

#endif // XADLIBRARYBASE_H
