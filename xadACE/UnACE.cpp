//////////////////////////////////////
//
// UnACE.cpp
//
// Author: Ilkka Prusi <ilkka.prusi@gmail.com>
//
// Based on public UNACE v1.2b  January 2004
// Copyright by Marcel Lemke
/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */
//

#include "UnACE.h"

////////////// protected methods

// read and check archive-file header
//
bool CUnACE::readArchiveHeader(CAnsiFile &archive)
{
	m_ReadBuffer.PrepareBuffer(1024, false);
	
	// TODO: check how much necessary to read for file metadata,
	// assuming this type..
	if (archive.Read(m_ReadBuffer.GetBegin(), sizeof(tacemhead)) == false)
	{
		throw IOException("Failed reading archive header");
	}
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		// something else or wrong identifier -> not supported here
		throw IOException("Unsupported file type");
	}
	
	tacemhead *aceHead = (tacemhead*)m_ReadBuffer.GetBegin();
	if (aceHead->HEAD_FLAGS & ACE_PASSW)
	{
		throw ArcException("Passworded file: decryption not supported", m_szArchive);
	}
	
	// what to do with this?
	//if (head.HEAD_FLAGS & ACE_AV)
	
	// what else is needed here..?
	
}

// read information on each entry in archive
// for listing and extracting from archive
//
bool CUnACE::readEntryList(CAnsiFile &archive)
{
}

////////////// public methods

bool CUnACE::List()
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
bool CUnACE::GetEntryList(tArchiveEntryList &lstArchiveInfo) const
{
	lstArchiveInfo = m_EntryList;
	return true;
}

bool CUnACE::Extract()
{
	// TODO:..
}

bool CUnACE::TestArchive()
{
	// not yet supported
	//ExtractNoOutput();
	return false;
}

