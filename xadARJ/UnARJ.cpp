#include "UnARJ.h"

////////////// protected methods

// read and check archive-file header
//
// ARJ archive header format goes something like this:
// ------------------------------------
// off  size (t)  desc
// 0    2  (u2)   "magic-id"
// 2    2  (u2)   header size
// 4    4  (u4)   crc (for header?)
//
bool CUnARJ::readArchiveHeader(CAnsiFile &archive)
{
	m_Crc.ClearCrc();
	m_ReadBuffer.PrepareBuffer(1024, false);

	if (archive.Read(m_ReadBuffer.GetBegin(), 128) == false)
	{
		throw IOException("Failed reading archive header");
	}
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		// something else or wrong identifier -> not supported here
		throw IOException("Unsupported file type");
	}
	m_ReadBuffer.SetCurrentPos(2);
	m_archiveInfo.headerSize = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveInfo.crc = getULong(m_ReadBuffer.GetNext(4));
	
	// TODO: don't include original CRC in counted crc or set to zero?
	// just skip this for now.. 
	/*
	m_Crc.updatecrc(m_ReadBuffer.GetBegin(), m_ReadBuffer.GetCurrentPos());
	if (m_Crc.getcrc() != m_archiveInfo.crc)
	{
		throw ArcException("CRC error in archive header", m_archiveInfo.crc);
	}
	*/
	
}

// read information on each entry in archive
// for listing and extracting from archive
//
bool CUnARJ::readEntryList(CAnsiFile &archive)
{
}


////////////// public methods

bool CUnARJ::List()
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
bool CUnARJ::GetEntryList(tArchiveEntryList &lstArchiveInfo) const
{
	lstArchiveInfo = m_EntryList;
	return true;
}

bool CUnARJ::Extract()
{
	// TODO:..
}

bool CUnARJ::TestArchive()
{
	// not yet supported
	//ExtractNoOutput();
	return false;
}

