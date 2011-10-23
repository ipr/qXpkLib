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

bool CUnACE::readHeader(CAnsiFile &archive)
{
	m_ReadBuffer.PrepareBuffer(1024, false);
	
	// TODO: check how much necessary to read for file metadata,
	// assuming this type..
	if (archive.Read(m_ReadBuffer.GetBegin(), sizeof(tacemhead)) == false)
	{
		throw IOException("Failed reading archive header");
	}
	tacemhead *aceHead = (tacemhead*)m_ReadBuffer.GetBegin();	

	if (aceHead->HEAD_FLAGS & ACE_PASSW)
	{
		throw ArcException("Passworded file: decryption not supported", m_szArchive);
	}
}

////////////// public methods

bool CUnACE::List()
{
}

bool CUnACE::GetEntryList(tArchiveEntryList &lstArchiveInfo) const
{
}

bool CUnACE::Extract()
{
}

bool CUnACE::TestArchive()
{
}

