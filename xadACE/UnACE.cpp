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
#include "arcexception.h"

////////////// protected methods

// read and check archive-file header
//
bool CUnACE::readArchiveHeader(CAnsiFile &archive)
{
	m_Crc.ClearCrc();
	m_ReadBuffer.PrepareBuffer(1024, false);

	// at least 14 bytes is needed to identify file as ACE-archive
	// (four fields at start needed anyway = 7 bytes + 7 for "acesign"), 
	// may need 4 more if "ADDSIZE" field exists in archive-header?
	if (archive.Read(m_ReadBuffer.GetBegin(), bytes_before_acesign + acesign_len) == false)
	{
		throw IOException("Failed reading archive header");
	}
	
	// TODO: if "ADDSIZE" may exist in archive-header,
	// this needs to adjust accordingly.. (offset +4)
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		// something else or wrong identifier -> not supported here
		throw IOException("Unsupported file type");
	}
	m_ReadBuffer.SetCurrentPos(0); // should be already..

	// fields of "tacehead"	
	m_archiveHeader.header.HEAD_CRC = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveHeader.header.HEAD_SIZE = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveHeader.header.HEAD_BLOCK_TYPE = m_ReadBuffer.GetNextByte();
	
	// should be this for "main" header?
	if (m_archiveHeader.header.HEAD_BLOCK_TYPE != MAIN_BLK)
	{
		throw ArcException("Unexpected block in archive header", m_archiveHeader.header.HEAD_BLOCK_TYPE);
	}
	
	m_archiveHeader.header.HEAD_FLAGS = getUWord(m_ReadBuffer.GetNext(2));

	// if this is set, there are 4 bytes here,
	// is it only for "file-block" entry?
	if (m_archiveHeader.header.HEAD_FLAGS & ACE_ADDSIZE)
	{
		// when part of multi-volume archive, has additional 4-byte field??
		// does this field exist with archive-header or only on entry-header?
		//m_archiveHeader.ADDSIZE = getULong(m_ReadBuffer.GetNext(4));
	}
	if (m_archiveHeader.header.HEAD_FLAGS & ACE_PASSW)
	{
		throw ArcException("Passworded file: decryption not supported", m_szArchive);
	}
	// what to do with this flag if set?
	if (m_archiveHeader.header.HEAD_FLAGS & ACE_AV)
	{
		// some kind of authentication verification information??
	}

	// "main" header has 7-byte signature here
	m_ReadBuffer.SetCurrentPos(m_ReadBuffer.GetCurrentPos()); // skip acesign, checked already
	
	// count header CRC upto current position
    m_Crc.updatecrc(m_ReadBuffer.GetBegin(), m_ReadBuffer.GetCurrentPos());
	
	// read rest of archive header according to size
	// TODO: check size, does it include fields above?
	//m_ReadBuffer.Reserve(m_archiveHeader.header.HEAD_SIZE); // prepare more at beginning, no need..
	if (archive.Read(m_ReadBuffer.GetAtCurrent(), m_archiveHeader.header.HEAD_SIZE) == false)
	{
		throw IOException("Failed reading rest of archive header");
	}

	// count header CRC upto current position
	//m_Crc.updatecrc(m_ReadBuffer.GetAtCurrent(), m_ReadBuffer.GetCurrentPos();
	/*
	if ((m_Crc.getcrc() & 0xffff) != m_archiveHeader.header.HEAD_CRC)
	{
		throw ArcException("CRC error in header", m_archiveHeader.header.HEAD_CRC);
	}
	*/
	m_archiveHeader.VER_MOD = m_ReadBuffer.GetNextByte();
	m_archiveHeader.VER_CR = m_ReadBuffer.GetNextByte();
	m_archiveHeader.HOST_CR = m_ReadBuffer.GetNextByte();
	m_archiveHeader.VOL_NUM = m_ReadBuffer.GetNextByte(); // part of multi-volume archive?
	
	// TODO: convert to time_t instead
	m_archiveHeader.TIME_CR = getULong(m_ReadBuffer.GetNext(4));
	
	// what are these fields?
	m_archiveHeader.RES1 = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveHeader.RES2 = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveHeader.RES = getULong(m_ReadBuffer.GetNext(4));

	// some "av" information?
	uint8_t avSize = m_ReadBuffer.GetNextByte();
	std::string av; // temp, see where to keep..
	av.assign((char*)m_ReadBuffer.GetAtCurrent(), avSize);
	
	// compressed comment?
	uint16_t commSize = getUWord(m_ReadBuffer.GetNext(2));
	std::string comm; // temp, see where to keep..
	comm.assign((char*)m_ReadBuffer.GetAtCurrent(), commSize);
	
	// that should be all in here?
	return true;
}

// read information on each entry in archive
// for listing and extracting from archive
//
bool CUnACE::readEntryList(CAnsiFile &archive)
{
	m_Crc.ClearCrc();
	m_ReadBuffer.PrepareBuffer(1024, false);
	
	// temp, change later..
	acefileheader fileHead;

	// 7 "common" bytes at start of entry header
	if (archive.Read(m_ReadBuffer.GetBegin(), bytes_before_acesign) == false)
	{
		throw IOException("Failed reading entry header");
	}

	m_ReadBuffer.SetCurrentPos(0); // should be already..

	// fields of "tacehead"	
	fileHead.header.HEAD_CRC = getUWord(m_ReadBuffer.GetNext(2));
	fileHead.header.HEAD_SIZE = getUWord(m_ReadBuffer.GetNext(2));
	fileHead.header.HEAD_BLOCK_TYPE = m_ReadBuffer.GetNextByte();
	
	// non-supported block type?
	if (fileHead.header.HEAD_BLOCK_TYPE == REC_BLK)
	{
		// ignore only?
		return true;
		//throw ArcException("Unexpected block in archive header", m_archiveHeader.header.HEAD_BLOCK_TYPE);
	}
	
	// should be this for file entry header?
	if (fileHead.header.HEAD_BLOCK_TYPE != FILE_BLK)
	{
		throw ArcException("Unexpected block in file entry header", fileHead.header.HEAD_BLOCK_TYPE);
	}
	
	fileHead.header.HEAD_FLAGS = getUWord(m_ReadBuffer.GetNext(2));

	if (fileHead.header.HEAD_FLAGS & ACE_ADDSIZE)
	{
		// when part of multi-volume archive, has additional 4-byte field??
		// does this field exist with archive-header or only on entry-header?
		fileHead.ADDSIZE = getULong(m_ReadBuffer.GetNext(4));
	}
	
	// TODO: check size, does it include fields above?
	if (archive.Read(m_ReadBuffer.GetAtCurrent(), fileHead.header.HEAD_SIZE) == false)
	{
		throw IOException("Failed reading rest of archive header");
	}
	fileHead.PSIZE = getULong(m_ReadBuffer.GetNext(4));
	fileHead.SIZE = getULong(m_ReadBuffer.GetNext(4));

	// TODO: convert to time_t instead
	fileHead.FTIME = getULong(m_ReadBuffer.GetNext(4));

	fileHead.ATTR = getULong(m_ReadBuffer.GetNext(4));
	fileHead.CRC32 = getULong(m_ReadBuffer.GetNext(4));
	
	// "TECH" structure
	fileHead.TECH.TYPE = m_ReadBuffer.GetNextByte();
	fileHead.TECH.QUAL = m_ReadBuffer.GetNextByte();
	fileHead.TECH.PARM = getUWord(m_ReadBuffer.GetNext(2));
	fileHead.RESERVED = getUWord(m_ReadBuffer.GetNext(2));
	
	// some file name?
	uint16_t fnameSize = getUWord(m_ReadBuffer.GetNext(2));
	std::string fname; // temp, see where to keep..
	fname.assign((char*)m_ReadBuffer.GetAtCurrent(), fnameSize);
	
	// compressed comment?
	uint16_t commSize = getUWord(m_ReadBuffer.GetNext(2));
	std::string comm; // temp, see where to keep..
	comm.assign((char*)m_ReadBuffer.GetAtCurrent(), commSize);
	
	// that's all for one entry?
	return true;
}


// TODO: need volume-information and access to multiple volumes..
// check if entry is spanning multiple volumes, otherwise we can keep simpler
//
bool CUnACE::extractEntry(CAnsiFile &archive, AceEntry *pEntry)
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
    readEntryList(archive);

    
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
		AceEntry *pEntry = (*it);
		extractEntry(archive, pEntry);
		++it;
	}
	return true;
}

bool CUnACE::TestArchive()
{
	// not yet supported
	//ExtractNoOutput();
	return false;
}

