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
// 8    1         size ??
// 9    1         ??
// 10   1         ??
// 11   1         host os? same types as Lha?
// 12   1         flags
// 13   1         method
// 14   1         type
// 15   1         (padding ?)
// 16   4         timestamp
// 20   4         compressed size
// 24   4         original size
// 28   4         file crc
// 32   2         entry (data?) offset
// 34   2         mode-flags (unix-style?)
// 36   2         host data ?
//
bool CUnARJ::readArchiveHeader(CAnsiFile &archive)
{
	m_ReadBuffer.PrepareBuffer(1024, false);

	if (archive.Read(m_ReadBuffer.GetBegin(), 8) == false)
	{
		throw IOException("Failed reading archive header");
	}
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		// something else or wrong identifier -> not supported here
		throw IOException("Unsupported file type");
	}
	m_ReadBuffer.SetCurrentPos(0); // should be already..
	
	m_archiveInfo.id = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveInfo.headerSize = getUWord(m_ReadBuffer.GetNext(2));
	m_archiveInfo.crc = getULong(m_ReadBuffer.GetNext(4));
	
	if (m_archiveInfo.headerSize == 0)
	{
		throw IOException("End-marker at beginning");
	}
	else if (m_archiveInfo.headerSize > m_nFileSize)
	{
		throw ArcException("invalid header size", m_archiveInfo.headerSize);
	}
	
	// TODO: don't include original CRC in counted crc or set to zero?
	// just skip this for now.. 
	/*
	m_Crc.ClearCrc();
	m_Crc.updatecrc(m_ReadBuffer.GetBegin(), m_ReadBuffer.GetCurrentPos());
	if (m_Crc.getcrc() != m_archiveInfo.headerSize)
	{
		throw ArcException("CRC error in archive header", crc);
	}
	*/
	
	return true;
}

ArjEntry *CUnARJ::readEntry(CAnsiFile &archive)
{
	// TODO, check: each entry includes "common" part
	// which is same as in archive-header?
	
	if (archive.Read(m_ReadBuffer.GetBegin(), 1) == false)
	{
		throw IOException("Failed reading entry header");
	}
	m_ReadBuffer.SetCurrentPos(0);
	
	uint8_t first_header_size = m_ReadBuffer.GetNextByte();
	if (first_header_size == 0) // eof-marker
	{
		// end-of-file marker reached?
		return nullptr;
	}
	
	if (archive.Read(m_ReadBuffer.GetAtCurrent(), first_header_size) == false)
	{
		throw IOException("Failed reading entry header");
	}
	
	ArjEntry *pEntry = new ArjEntry(first_header_size);
	
	pEntry->arj_nbr = m_ReadBuffer.GetNextByte();
	pEntry->arj_x_nbr = m_ReadBuffer.GetNextByte();
	pEntry->host_os = m_ReadBuffer.GetNextByte(); // same types as Lha ?
	pEntry->arj_flags = m_ReadBuffer.GetNextByte();
	pEntry->method = m_ReadBuffer.GetNextByte(); // packing method? same as Lha ?
	pEntry->file_type = m_ReadBuffer.GetNextByte();
	pEntry->padding = m_ReadBuffer.GetNextByte(); // skipped, padding only?
	uint32_t stamp = getULong(m_ReadBuffer.GetNext(4));
	pEntry->timestamp = (time_t)CGenericTime(stamp); // just convert it..
	pEntry->compressed_size = getULong(m_ReadBuffer.GetNext(4));
	pEntry->original_size = getULong(m_ReadBuffer.GetNext(4));
	pEntry->file_crc = getULong(m_ReadBuffer.GetNext(4));
	
	pEntry->entry_position = getUWord(m_ReadBuffer.GetNext(2));
	pEntry->file_mode = getUWord(m_ReadBuffer.GetNext(2));
	pEntry->host_data = getUWord(m_ReadBuffer.GetNext(2));
	
	// TODO: filename and rest..
	
	return pEntry;
}

// read information on each entry in archive
// for listing and extracting from archive
//
bool CUnARJ::readEntryList(CAnsiFile &archive)
{
	// TODO: something like this:
	ArjEntry *pEntry = readEntry(archive);
	while (pEntry != nullptr)
	{
		m_EntryList.push_back(pEntry);
		pEntry = readEntry(archive);
	}
}

// extract entry from archive
bool CUnARJ::extractEntry(CAnsiFile &archive, ArjEntry *pEntry)
{
	// locate entry in archive..
	if (archive.Seek(pHeader->data_pos, SEEK_SET) == false)
	{
		throw IOException("Failure seeking entry data");
	}
	
	// read entry data..
	m_ReadBuffer.PrepareBuffer(pEntry->compressed_size, false);
	m_DecrunchBuffer.PrepareBuffer(pEntry->original_size, false);
	if (archive.Read(m_ReadBuffer.GetBegin(), pEntry->compressed_size) == false)
	{
		//throw ArcException("Failure reading entry data", pEntry->file_name);
	}
	
	// decode to output-buffer..

	// create file for writing
	std::string outFilename = m_szExtractionPath;
	if (outFilename.at(outFilename.length() -1) != '/')
	{
		outFilename += "/";
	}
	outFilename += pEntry->fileName;

	// open file to write	
	CAnsiFile OutFile;
	if (OutFile.Open(outFilename, true) == false)
	{
		throw ArcException("Failed creating file for writing", outFilename);
	}
	if (OutFile.Write(m_DecrunchBuffer.GetBegin(), pEntry->original_size) == false)
	{
		throw ArcException("Failed writing output", outFilename);
	}
	OutFile.Close(); // closed on destructor anyway..
	
	return true; // entry done
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
    readEntryList(archive);
    
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
		ArjEntry *pEntry = (*it);
		
		extractEntry(archive, pEntry);
		
		++it;
	}
	return true;
}

bool CUnARJ::TestArchive()
{
	// not yet supported
	//ExtractNoOutput();
	return false;
}

