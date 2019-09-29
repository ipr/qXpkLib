#include "UnARC.h"

#include "arcexception.h"


////////////// protected methods

// read and check archive-file header
//
bool CUnARC::readArchiveHeader(CAnsiFile &archive)
{
	m_ReadBuffer.PrepareBuffer(1024, false);

	if (archive.Read(m_ReadBuffer.GetBegin(), m_metadata.arc_header_size) == false)
	{
		throw IOException("Failed reading archive header");
	}
	if (isSupported(m_ReadBuffer.GetBegin()) == false)
	{
		// something else or wrong identifier -> not supported here
		throw IOException("Unsupported file type");
	}
	m_ReadBuffer.SetCurrentPos(0); // should be already..
	parseMetadata(m_ReadBuffer);
	
	// if compressed data size is larger than file
	// either file is corrupted or it's not arc-file..
	//if (m_metadata.compressed_size != m_nFileSize) //should be equal?
	if (m_metadata.compressed_size > m_nFileSize)
	{
		throw IOException("Invalid file size");
	}
	
	return true;
}

/*
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
*/

// read information on each entry in archive
// for listing and extracting from archive
//
bool CUnARC::readEntryList(CAnsiFile &archive)
{
	// TODO: something like this:
	ArcEntry *pEntry = readEntry(archive);
	while (pEntry != nullptr)
	{
		m_EntryList.push_back(pEntry);
		pEntry = readEntry(archive);
	}
}

// extract entry from archive
bool CUnARC::extractEntry(CAnsiFile &archive, ArcEntry *pEntry)
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

bool CUnARC::isSupported(const uint8_t *pBuffer) const
{
	if (pBuffer[0] != 0x1A)
	{
		return false;
	}
	if (::memcmp(pBuffer +1, "HP", 2) == 0
		|| ::memcmp(pBuffer +1, "ST", 2) == 0)
	{
		return false;
	}
	
	// TODO: more checking?
	
	return true;
}


bool CUnARC::List()
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
bool CUnARC::GetEntryList(tArchiveEntryList &lstArchiveInfo) const
{
	lstArchiveInfo = m_EntryList;
	return true;
}

bool CUnARC::Extract()
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
		ArcEntry *pEntry = (*it);
		
		extractEntry(archive, pEntry);
		
		++it;
	}
	return true;
}

bool CUnARC::TestArchive()
{
	// not yet supported
	//ExtractNoOutput();
	return false;
}

