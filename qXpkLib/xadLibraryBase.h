///////////////////////////////////
//
// Pure virtual interface-class
// for multi-file archive handling.
//


#ifndef XADLIBRARYBASE_H
#define XADLIBRARYBASE_H

#include <QString>


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

	/*
	// TODO: for listing archive contents
	class archiveEntry
	{
	public:
		archiveEntry() {};
	};
	*/

	// pack/compress
	//virtual bool Crunch(XpkProgress *pProgress)=0;

	// get amount of files in archive
	//virtual size_t fileCount()=0;

	// list files in archive
	//virtual bool fileList(QList<archiveEntry> &entryList)=0;
	
	// should be given in progress-object?
	//void setArchive(QString &szArchive)=0;
	
	// set path to uncompress files to
	virtual bool setExtractPath(QString &szPath)=0;
	
	// test archive integrity
	virtual bool testArchive()=0;
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};

#endif // XADLIBRARYBASE_H
