/*//////////////////////////////////////
//
// HA.h -- HA archive unpacker
// Original author: Harri Hirvola 
// Modified for C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "HA.h"

////////////// protected methods

// read and check archive-file header
//
bool CHA::readArchiveHeader(CAnsiFile &archive)
{
	m_Crc.ClearCrc();
	m_ReadBuffer.PrepareBuffer(1024, false);

	if (archive.Read(m_ReadBuffer.GetBegin(), 2) == false)
	{
		throw IOException("Failed reading archive header");
	}
	
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		// something else or wrong identifier -> not supported here
		throw IOException("Unsupported file type");
	}
	m_ReadBuffer.SetCurrentPos(2);

	
}

// read information on each entry in archive
// for listing and extracting from archive
//
bool CHA::readEntryList(CAnsiFile &archive)
{
}


// TODO: need volume-information and access to multiple volumes..
// check if entry is spanning multiple volumes, otherwise we can keep simpler
//
bool CHA::extractEntry(CAnsiFile &archive, HAEntry *pEntry)
{
}

////////////// public methods

bool CHA::List()
{
	CAnsiFile archive(m_szArchive);
	if (archive.IsOk() == false)
	{
        throw ArcException("could not open archive", m_szArchive);
	}
    m_nFileSize = archive.GetSize();
    
    readArchiveHeader(archive);
    readEntryList(archive);

    
    return true;
}

// allow access to information for caller,
// must not destroy contents: just read it
//
bool CHA::GetEntryList(tArchiveEntryList &lstArchiveInfo) const
{
	lstArchiveInfo = m_EntryList;
	return true;
}

bool CHA::Extract()
{
	CAnsiFile archive(m_szArchive);
	if (archive.IsOk() == false)
	{
        throw ArcException("could not open archive", m_szArchive);
	}
    m_nFileSize = archive.GetSize();
    
    readArchiveHeader(archive);
    readEntryList(archive);
    
    // TODO: need access to different files
    // in case of multi-volume archives..
    // entry may be spanning multiple
    
	auto it = m_EntryList.begin();
	auto itend = m_EntryList.end();
	while (it != itend)
	{
		HAEntry *pEntry = (*it);
		extractEntry(archive, pEntry);
		++it;
	}
	return true;
}

bool CHA::TestArchive()
{
	// not yet supported
	//ExtractNoOutput();
	return false;
}

