/*
   Unrar source may be used in any software to handle RAR archives
   without limitations free of charge, but cannot be used to re-create
   the RAR compression algorithm, which is proprietary. Distribution
   of modified Unrar source in separate form or as a part of other
   software is permitted, provided that it is clearly stated in
   the documentation and source comments that the code may not be used
   to develop a RAR (WinRAR) compatible archiver.

   More detailed license text is available in license.txt.
*/
/*
	Based on Portable UnRAR version (AmigaOS4 port).
	Authors: Alexander Roshal, Nicolas Mendoza, Joachim Birging, Stephan Matzke, Joerg Strohmayer
	
	This C++ library version by: Ilkka Prusi <ilkka.prusi@gmail.com>
*/

#include "UnRAR.h"

////////////// protected methods


// support for old&new style entry information
// -> wrap to class
//
RarEntry *CUnRAR::readEntry(CAnsiFile &archive)
{
	// TODO: implement
	RarEntry *pEntry = nullptr;

	// TODO: new format file entry handling
	if (m_archiveHeader.m_format == RAR_NEW)
	{
		// read new style header
	}
	else if (m_archiveHeader.m_format == RAR_OLD)
	{
		RarOldFileHeader *pFile = (RarOldFileHeader*)m_ReadBuffer.GetAtCurrent();
		
		// parse entry and keep info
		pEntry = new RarEntry(pFile);
	}

	return pEntry;
}


// read and check archive-file header
//
bool CUnRAR::readArchiveHeader(CAnsiFile &archive)
{
	m_ReadBuffer.PrepareBuffer(1024, false);

	if (archive.Read(m_ReadBuffer.GetBegin(), 128) == false)
	{
		throw IOException("Failed reading archive header");
	}
	
	m_archiveHeader.m_format = getRarFormat(m_ReadBuffer.GetBegin());
	if (m_archiveHeader.m_format == Unknown)
	{
		throw IOException("Unsupported file type");
	}
	
	if (m_archiveHeader.m_format == RAR_NEW)
	{
		// read new style header
	}
	else if (m_archiveHeader.m_format == RAR_OLD)
	{
		// read old style header:
		// uint8_t		Mark[4];
		// uint16_t	HeadSize;
		// uint8_t		Flags;
		//
		m_ReadBuffer.SetCurrentPos(4); // skip identifier
		
		// keep interesting fields
		m_archiveHeader.m_headerSize = getUWord(m_ReadBuffer.GetNext(2));
		m_archiveHeader.setOldFlags(m_ReadBuffer.GetNextByte());
	}
	
	
}

// read information on each entry in archive
// for listing and extracting from archive
//
bool CUnRAR::readEntryList(CAnsiFile &archive)
{
	// something like this..:
	
	RarEntry *pEntry = readEntry(archive);
	while (pEntry != nullptr)
	{
		m_EntryList.push_back(pEntry);
		pEntry = readEntry(archive);
	}
}


////////////// public methods

bool CUnRAR::List()
{
	CAnsiFile archive(m_szArchive);
	if (archive.IsOk() == false)
	{
        throw ArcException("could not open archive", m_szArchive);
	}
    m_nFileSize = archive.GetSize();
    
    readArchiveHeader(archive);
    //readEntryList(archive);
    
    return true;
}

// allow access to information for caller,
// must not destroy contents: just read it
//
bool CUnRAR::GetEntryList(tArchiveEntryList &lstArchiveInfo) const
{
	lstArchiveInfo = m_EntryList;
	return true;
}

bool CUnRAR::Extract()
{
	// TODO:..
}

bool CUnRAR::TestArchive()
{
	// not yet supported
	//ExtractNoOutput();
	return false;
}

