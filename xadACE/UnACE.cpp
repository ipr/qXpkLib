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

	// at least 14 bytes is needed to identify file as ACE-archive
	if (archive.Read(m_ReadBuffer.GetBegin(), bytes_before_acesign + acesign_len) == false)
	{
		throw IOException("Failed reading archive header");
	}
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		// something else or wrong identifier -> not supported here
		throw IOException("Unsupported file type");
	}
	m_ReadBuffer.SetCurrentPos(0); // should be already..
	
	/* on second though, better not this way after all..
	tacearchead *aceHead = (tacearchead*)m_ReadBuffer.GetBegin();
	if (aceHead->HEAD_FLAGS & ACE_PASSW)
	{
		throw ArcException("Passworded file: decryption not supported", m_szArchive);
	}
	*/
	
	m_archiveHeader.header.HEAD_CRC = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveHeader.header.HEAD_SIZE = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveHeader.header.HEAD_BLOCK_TYPE = m_ReadBuffer.GetNextByte();
	m_archiveHeader.header.HEAD_FLAGS = getUWord(m_ReadBuffer.GetNext(2));
	m_ReadBuffer.SetCurrentPos(m_ReadBuffer.GetCurrentPos()); // skip acesign, checked already
	
	// what to do with this flag if set?
	if (m_archiveHeader.header.HEAD_FLAGS & ACE_AV)
	{
		// some kind of authentication verification information??
	}
	if (m_archiveHeader.header.HEAD_FLAGS & ACE_ADDSIZE)
	{
		// when part of multi-volume archive, has additional 4-byte field??
	}
	
	// read rest of archive header according to size
	//m_ReadBuffer.Reserve(m_archiveHeader.header.HEAD_SIZE); // prepare more at beginning, no need..
	if (archive.Read(m_ReadBuffer.GetCurrentPos(), m_archiveHeader.header.HEAD_SIZE) == false)
	{
		throw IOException("Failed reading rest of archive header");
	}
	
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

