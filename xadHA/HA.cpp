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


bool CHA::readFileHeader(CAnsiFile &archive, Fheader &header)
{
	header.version = m_ReadBuffer.GetNextByte();
	if (header.version != 0xFF)
	{
		header.method_type = (m_archiveHeader.version & 0xf);
		header.version >>= 4;
		if (header.isSupported() == false)
		{
			return false;
		}
	}
	else if (header.version == 0xFF)
	{
		// invalid value ??
		// what's this in original?
	    //dirty=1;
	    //--i;
	}
	
	header.comp_len = getvalue(4);
	header.orig_len = getvalue(4);
	header.crc = getvalue(4);
	header.time = getvalue(4);

	// path..
	getstring(archive, header.path);
	// name..	
	getstring(archive, header.name);

	// read this final value now..
	if (archive.Read(&(header.mdilen), 1) == false)
	{
		throw IOException("Failed reading archive header");
	}
	
	// calculate total length now    
    header.calcLen();
    
    return true;
}

Mheader *CHA::readMachineHeader(CAnsiFile &archive, Fheader &parentHeader)
{
	Mheader *pmd = new Mheader();
	pmd->mtype = m_ReadBuffer.GetNextByte();
	if (pmd->mtype == UNIXMDH)
	{
		pmd->attr = getUWord(m_ReadBuffer.GetNext(2));
		pmd->user = getUWord(m_ReadBuffer.GetNext(2));
		pmd->group = getUWord(m_ReadBuffer.GetNext(2));
		return pmd;
	}
	
	switch (parentHeader.method_type)
	{
	case M_DIR:
		//pmd->attr = md_tohaattr(DEF_DIRATTR);
		//pmd->attr |= HA_IFDIR;
		break;
	default:
		//pmd->attr = md_tohaattr(DEF_FILEATTR);
		break;
	}
		
	return pmd;
}


// read and check archive-file header
//
bool CHA::readArchiveHeader(CAnsiFile &archive)
{
	//m_Crc.ClearCrc();
	m_ReadBuffer.PrepareBuffer(1024, false);

	// 2 for "HA", 18 for header to have anything usable,
	// rest depend on length of NULL-terminated names
	// so read more then
	// 
	if (archive.Read(m_ReadBuffer.GetBegin(), 20) == false)
	{
		throw IOException("Failed reading archive header");
	}
	
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		// something else or wrong identifier -> not supported here
		throw IOException("Unsupported file type");
	}
	m_ReadBuffer.SetCurrentPos(2);
	
	if (readFileHeader(archive, m_archiveHeader) == false)
	{
		// just skip or abort on unsupported type?
		//return false;
		throw IOException("Unsupported archive", m_archiveHeader.method_type);
	}

	/*
	hd=getheader();
	pos+=hd->clen+hd->mylen;
	if (hd->ver==0xff) {
	    dirty=1;
	    --i;
	    */
	    
	// machine header??
	// -> read now
	if (archive.Read(m_ReadBuffer.GetBegin(), m_archiveHeader.mdilen) == false)
	{
		throw IOException("Failed reading machine header");
	}
	
	m_archiveHeader.mdhd = readMachineHeader(archive, m_archiveHeader);
	
	return true;
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

