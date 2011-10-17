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

#include "XpkProgress.h"
#include "AnsiFile.h"


class xadLibraryBase
{
protected:
	// only derived can be instantiated
	xadLibraryBase(void) 
	{}
	virtual ~xadLibraryBase(void)
	{}
	
public:

	// list files in archive, get other metadata also..
	virtual bool archiveInfo(QXpkLib::CArchiveInfo &info)=0;
	
	// should be given in XpkProgress-object?
	//void setArchive(QString &szArchive)=0;
	
	// set path to uncompress files to
	virtual bool setExtractPath(QString &szPath)=0;
	
	// test archive integrity
	virtual bool testArchive()=0;
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};

#endif // XADLIBRARYBASE_H
