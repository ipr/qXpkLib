#include "UnStuffIt.h"


////////////// public methods

bool CUnStuffIt::List()
{
	/*
	CAnsiFile archive(m_szArchive);
	if (archive.IsOk() == false)
	{
        throw ArcException("could not open archive", m_szArchive);
	}
    m_nFileSize = archive.GetSize();
    */
    
    
    return true;
}

// allow access to information for caller,
// must not destroy contents: just read it
//
bool CUnStuffIt::GetEntryList(tArchiveEntryList &lstArchiveInfo) const
{
	lstArchiveInfo = m_EntryList;
	return true;
}

bool CUnStuffIt::Extract()
{
/*
	CAnsiFile archive(m_szArchive);
	if (archive.IsOk() == false)
	{
        throw ArcException("could not open archive", m_szArchive);
	}
    m_nFileSize = archive.GetSize();
    
    readArchiveHeader(archive);
    readEntryList(archive);

	auto it = m_EntryList.begin();
	auto itend = m_EntryList.end();
	while (it != itend)
	{
		extractEntry(archive, (*it));
		
		++it;
	}
*/
	return true;
}

bool CUnStuffIt::TestArchive()
{
	// not yet supported
	//ExtractNoOutput();
	return false;
}

