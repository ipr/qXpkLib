#include "UnARJ.h"

////////////// protected methods

// read and check archive-file header
//
bool CUnARJ::readArchiveHeader(CAnsiFile &archive)
{
	m_ReadBuffer.PrepareBuffer(1024, false);

	if (archive.Read(m_ReadBuffer.GetBegin(), 128) == false)
	{
		throw IOException("Failed reading archive header");
	}
	
	m_ReadBuffer.SetCurrentPos(0); // should be already..
	
	
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

