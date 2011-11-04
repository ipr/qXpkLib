//////////////////////////////////////////////////////
//
// CUnZoo :
// Ilkka Prusi <ilkka.prusi@gmail.com>
//
// Rewrite of unzoo.c by Martin Schoenert
// 
// This file in public domain.
//
/*
*  unzoo.c by Martin Schoenert
*
*  @(#)$Id: unzoo.c,v 4.4 2000/05/29 08:56:57 sal Exp $
*
*  This file is in the Public Domain.
*
* Rewrite for use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
*
*/

/****************************************************************************
**
*A  unzoo.c                     Tools                        Martin Schoenert
**
*H  @(#)$Id: unzoo.c,v 4.4 2000/05/29 08:56:57 sal Exp $
**
*Y  This file is in the Public Domain.
**
**  DESCRIPTION
**
**  'unzoo' is  a zoo  archive extractor.   A zoo archive   is  a  file  that
**  contains several files, called its members, usually in compressed form to
**  save space.  'unzoo' can list all or  selected members or  extract all or
**  selected members, i.e.,  uncompress them and write   them  to files.   It
**  cannot add new members or  delete  members.  For this   you need the  zoo
**  archiver, called 'zoo', written by Rahul Dhesi.
**
**
**  COMPATIBILITY
**
**  'unzoo'  is based heavily on the 'booz' archive extractor by Rahul Dhesi.
**  I basically stuffed everything in one file (so  no 'Makefile' is needed),
**  cleaned it up (so that it is now more portable and  a little bit faster),
**  and added the  support for  long file names,  directories,  and comments.
**
**
**
**  ACKNOWLEDGMENTS
**
**  Rahul Dhesi  wrote the  'zoo' archiver and the  'booz' archive extractor.
**  Haruhiko Okumura  wrote the  LZH code (originally for his 'ar' archiver).
**  David Schwaderer provided the CRC-16 calculation in PC Tech Journal 4/85.
**  Jeff Damens  wrote the name match code in 'booz' (originally for Kermit).
**  Harald Boegeholz  ported 'unzoo' to OS/2 with the emx development system.
**  Dave Bayer ported 'unzoo' to the Macintosh,  including Macbinary support.
**
*/

#include "UnZoo.h"

void CUnZoo::readString(CAnsiFile &archive, const size_t offset, const size_t length, std::string &value)
{
	if (archive.Seek(offset, SEEK_SET) == false)
	{
		throw IOException("Failed to seek string entry");
	}
	
	if (archive.Read(m_ReadBuffer.GetBegin(), length) == false)
	{
		throw IOException("Failed to read string");
	}
	value.assign((char*)m_ReadBuffer.GetBegin(), length);
}

// read list of archive contents (entry-list)
//
bool CUnZoo::readArchiveEntryList(CAnsiFile &archive)
{
	// size of fixed-length part for entry-information
	const size_t entryFixedSize = 34 + 13; // values + short name len

	size_t nReadCount = m_archiveInfo.header_size;
	//while ((nReadCount+entryFixedSize) < m_nFileSize)

	size_t nNextOffset = m_archiveInfo.first_entry_pos;
	while (nNextOffset > 0 && nNextOffset < m_nFileSize)
	{
		if (archive.Seek(nNextOffset, SEEK_SET) == false)
		{
			throw IOException("Failed to seek entry");
		}
	
		if (archive.Read(m_ReadBuffer.GetBegin(), entryFixedSize) == false)
		{
			// corrupted file or bug? should detect end before..
			throw IOException("Failed to read entry header");
		}
		nReadCount += entryFixedSize;
		
		if (isSupportedEntry(m_ReadBuffer.GetBegin()) == false)
		{
			throw IOException("Unsupported entry detected");
		}
		m_ReadBuffer.SetCurrentPos(0); // simplify rest, keep counting offset

		// should be supported -> allocate entry to list, get rest of values	
		ZooEntry *pEntry = new ZooEntry();
		m_EntryList.push_back(pEntry);
		
	    pEntry->magicid = getULong(m_ReadBuffer.GetNext(4)); // keep for debugging at least..
	    pEntry->member_type = m_ReadBuffer.GetNextByte();
	    pEntry->method = m_ReadBuffer.GetNextByte();
	    pEntry->next_entry_pos = getULong(m_ReadBuffer.GetNext(4));
	    pEntry->data_position = getULong(m_ReadBuffer.GetNext(4));

		// convert this timestamp to usable form
	    uint16_t date = getUWord(m_ReadBuffer.GetNext(2));
	    uint16_t time = getUWord(m_ReadBuffer.GetNext(2));
	    pEntry->setTimestamp(CGenericTime(date, time));
	    
	    pEntry->data_crc = getUWord(m_ReadBuffer.GetNext(2));
	    pEntry->original_size = getULong(m_ReadBuffer.GetNext(4));
	    pEntry->compressed_size = getULong(m_ReadBuffer.GetNext(4));
	    pEntry->version_major = m_ReadBuffer.GetNextByte();
	    pEntry->version_minor = m_ReadBuffer.GetNextByte();
	    pEntry->deleted = m_ReadBuffer.GetNextByte();
	    pEntry->spared = m_ReadBuffer.GetNextByte(); // padding-byte only?
	    pEntry->comment_position = getULong(m_ReadBuffer.GetNext(4));
	    pEntry->comment_size = getUWord(m_ReadBuffer.GetNext(2));
	    
		// some short name (fucking msdos..):
		// overwrite this with proper name when it is found in file
		//
		pEntry->fileName.assign((char*)m_ReadBuffer.GetNext(13), 13);
		fixPathname(pEntry->fileName); // fix path-separator

		// handle extension of entry-information..
		// part of it has fixed size and 
		// 
		if (pEntry->member_type == 2) // (what are these types anyway?)
		{
			pEntry->m_pVarDetails = new ZooVariableEntry();
			
			size_t extendedRead = 2+1+2; // next fields needed..
			if (archive.Read(m_ReadBuffer.GetBegin(), extendedRead) == false)
			{
				throw IOException("Failed to read extended header start");
			}
			m_ReadBuffer.SetCurrentPos(0); // simplify rest, keep counting offset
			nReadCount += extendedRead;
			
			// does this include size of length-field or just the rest?
			// just the rest after these three fields?
			pEntry->m_pVarDetails->variable_size = getUWord(m_ReadBuffer.GetNext(2));
			pEntry->m_pVarDetails->timezone = m_ReadBuffer.GetNextByte();
			pEntry->m_pVarDetails->entry_crc = getUWord(m_ReadBuffer.GetNext(2));
			
			extendedRead = pEntry->m_pVarDetails->variable_size;
			if (extendedRead > 0)
			{
				if (archive.Read(m_ReadBuffer.GetBegin(), extendedRead) == false)
				{
					throw IOException("Failed to read extended header variable-sized part");
				}
				m_ReadBuffer.SetCurrentPos(0); // simplify rest, keep counting offset
				nReadCount += extendedRead;
	
				// no point in this if there is no name also.. check anyway (broken archives?)
				uint8_t fileNameLen = m_ReadBuffer.GetNextByte();
				extendedRead -= 1;
				
				uint8_t dirNameLen = 0;
				if (extendedRead > 0)
				{
					dirNameLen = m_ReadBuffer.GetNextByte();
					extendedRead -= 1;
				}
				
				if (fileNameLen > 0 && extendedRead > 0)
				{
					// overwrite msdos-shit from before when we have proper name for file
					pEntry->fileName.assign((char*)m_ReadBuffer.GetNext(fileNameLen), fileNameLen);
					fixPathname(pEntry->fileName); // fix path-separator for later
					extendedRead -= fileNameLen;
				}
				if (dirNameLen > 0 && extendedRead > 0)
				{
					pEntry->pathName.assign((char*)m_ReadBuffer.GetNext(dirNameLen), dirNameLen);
					fixPathname(pEntry->pathName); // fix path-separator for later
					
					// append path-separator to directory name if missing
					if (pEntry->pathName.at(pEntry->pathName.length() -1) != '/')
					{
						pEntry->pathName += "/";
					}
					
					extendedRead -= dirNameLen;
				}
				if (extendedRead > 0)
				{
					pEntry->m_pVarDetails->systemid = getUWord(m_ReadBuffer.GetNext(2));
					extendedRead -= 2;
				}
				if (extendedRead > 0)
				{
					// some weird three-byte value..
					pEntry->m_pVarDetails->permissions = getTriple(m_ReadBuffer.GetNext(3));
					extendedRead -= 3;
				}
				if (extendedRead > 0)
				{
					// some weird three-byte value..
					pEntry->m_pVarDetails->modgen = m_ReadBuffer.GetNextByte();
					extendedRead -= 1;
				}
				if (extendedRead > 0)
				{
					// some weird three-byte value..
					pEntry->m_pVarDetails->version = getUWord(m_ReadBuffer.GetNext(2));
					extendedRead -= 2;
				}
			}
		}
		
		// read entry-related comment if any
		if (pEntry->comment_position > 0 && pEntry->comment_size > 0)
		{
			readString(archive,
						pEntry->comment_position,
						pEntry->comment_size,
						pEntry->comment);
		}

		// update count for archive statistics now
		m_ulTotalPacked += pEntry->compressed_size;		
		m_ulTotalUnpacked += pEntry->original_size;
		m_ulTotalFiles += 1;
		
		nNextOffset = pEntry->next_entry_pos;
	} // while..
	
	return true;
}


// read archive metadata (description) from file
//
bool CUnZoo::readArchiveDescription(CAnsiFile &archive)
{
    if (archive.Read(m_ReadBuffer.GetBegin(), 34) == false)
    {
		// file less than header size or just not readable,
		// nothing to do here anyway -> just get out of here
		throw IOException("Failed to read header");
    }
    
    // check text and ID at near start
    if (isSupportedArchive(m_ReadBuffer.GetBegin()) == false)
    {
		throw IOException("Unsupported file type");
    }
    
    m_archiveInfo.description.assign((char*)m_ReadBuffer.GetBegin(), 20);
    m_ReadBuffer.SetCurrentPos(20); // simplify rest.. keep counting offset
    m_archiveInfo.magicid = getULong(m_ReadBuffer.GetNext(4));
    m_archiveInfo.first_entry_pos = getULong(m_ReadBuffer.GetNext(4));
    m_archiveInfo.klhvmh = getULong(m_ReadBuffer.GetNext(4));
    
    m_archiveInfo.version_major = m_ReadBuffer.GetNextByte();
    m_archiveInfo.version_minor = m_ReadBuffer.GetNextByte();

	// set size so far..
	m_archiveInfo.header_size = 34;

	// extension of header in newer formats
	// if first entry is not directly after "old" header
	if (m_archiveInfo.first_entry_pos > 34)
	{
		// 8 bytes as optional information
		if (archive.Read(m_ReadBuffer.GetAtCurrent(), 8) == false)
		{
			throw IOException("Failed to read header extension");
		}
		m_archiveInfo.member_type = m_ReadBuffer.GetNextByte();
		m_archiveInfo.comment_pos = getULong(m_ReadBuffer.GetNext(4));
		m_archiveInfo.comment_size = getUWord(m_ReadBuffer.GetNext(2));
		m_archiveInfo.modgen = m_ReadBuffer.GetNextByte();
		m_archiveInfo.is_new_style = true;
		m_archiveInfo.header_size += 8; // update (extension included)
		
		if (m_archiveInfo.comment_size > 0 && m_archiveInfo.comment_pos > 0)
		{
			readString(archive, 
						m_archiveInfo.comment_pos, 
						m_archiveInfo.comment_size, 
						m_archiveInfo.comment);
		}
	}
    return true;
}

bool CUnZoo::ExtractEntry(ZooEntry *pEntry, CAnsiFile &archive)
{
	if (pEntry->method == PackLzd)  
	{
		// not currently supported
		return true; // skip it
	}

	if (pEntry->method != PackCopyOnly
		&& pEntry->method != PackLzh)
	{
		// warning only or error?
		// (unsupported packing method)
		throw ArcException("unknown packing method", pEntry->fileName);
	}

	// locate data in archive
	if (archive.Seek(pEntry->data_position, SEEK_SET) == false)
	{
		throw ArcException("Failed to seek entry", pEntry->fileName);
	}

	// path must begin with user-given root
	std::string outFilename = m_szExtractionPath;
	// we fixed paths on reading -> just concatenate
	outFilename += pEntry->getName();

	// file creation fails if this fails
	// -> we can detect error then
	CPathHelper::MakePathToFile(outFilename);
	
	CAnsiFile outFile(outFilename, true);
	if (outFile.IsOk() == false)
	{
		throw ArcException("Failed to open file for writing", outFilename);
	}
	
	m_crc.ClearCrc(); // = 0;
	bool bRes = false;

	if (pEntry->method == PackCopyOnly)
	{
		m_ReadBuffer.PrepareBuffer(pEntry->compressed_size, false);
		if (archive.Read(m_ReadBuffer.GetBegin(), pEntry->compressed_size) == false)
		{
			throw ArcException("Failed reading compressed data for file", pEntry->fileName);
		}
		m_crc.UpdateCrc(m_ReadBuffer.GetBegin(), pEntry->compressed_size);

		// uncompressed data: just write it directly to output
		if (outFile.Write(m_ReadBuffer.GetBegin(), pEntry->compressed_size) == false)
		{
			throw ArcException("Failed writing uncompressed data for file", pEntry->fileName);
		}
		bRes = true;
	}
	else if (pEntry->method == PackLzh)
	{
		// just read entry to buffer.. 
		
		m_ReadBuffer.PrepareBuffer(pEntry->compressed_size, false);
		m_WriteBuffer.PrepareBuffer(pEntry->original_size, false);
		
		if (archive.Read(m_ReadBuffer.GetBegin(), pEntry->compressed_size) == false)
		{
			throw ArcException("Failed reading compressed data for file", pEntry->fileName);
		}
		m_decode.setup(pEntry, &m_ReadBuffer, &m_WriteBuffer);
	
		// this expects buffer to be ready for decoding now
		bRes = m_decode.DecodeLzh(pEntry);
		
		//m_crc.UpdateCrc(m_WriteBuffer.GetBegin(), pEntry->original_size);
		if (outFile.Write(m_WriteBuffer.GetBegin(), pEntry->original_size) == false)
		{
			throw ArcException("Failed writing uncompressed data for file", pEntry->fileName);
		}
	}

	if (m_crc.m_Crc != pEntry->data_crc)
	{
		throw ArcException("CRC error", pEntry->fileName);
	}
	
	return bRes;
}

/////////// public methods

// get list of archive entries to entry-list
bool CUnZoo::ListContents()
{
	// TODO: better way of handling multiple reads of same file?
	Clear();
	
    CAnsiFile archive(m_szArchive);
    if (archive.IsOk() == false)
    {
        throw ArcException("could not open archive", m_szArchive);
    }
    m_nFileSize = archive.GetSize();

	// should throw exception on error anyway..
	if (readArchiveDescription(archive) == false)
	{
        throw ArcException("failed reading header", m_szArchive);
	}
	
	if (readArchiveEntryList(archive) == false)
	{
        throw ArcException("failed reading entry list", m_szArchive);
	}
	
	return true;
}

// test integrity, try extracting without output
bool CUnZoo::TestArchive()
{
	//return ExtractNoOutput(m_szArchive);
	return false;
}

// extract all files to path given before,
// just extract "as-is": users have other tools to convert text-encoding etc.
bool CUnZoo::Extract()
{
	// TODO: better way of handling multiple reads of same file?
	Clear();
	
    CAnsiFile archive(m_szArchive);
    if (archive.IsOk() == false)
    {
        throw ArcException("could not open archive", m_szArchive);
    }
    m_nFileSize = archive.GetSize();

	// should throw exception on error anyway..
	if (readArchiveDescription(archive) == false)
	{
        throw ArcException("failed reading header", m_szArchive);
	}
	
	if (readArchiveEntryList(archive) == false)
	{
        throw ArcException("failed reading entry list", m_szArchive);
	}
	
	auto it = m_EntryList.begin();
	auto itend = m_EntryList.end();
	while (it != itend)
	{
		ZooEntry *pEntry = (*it);
        if (pEntry->deleted == 1)
        {
			// what is this really? skip it anyway..
			++it;
            continue;
        }
        
        if ((pEntry->method > 2) 
			|| (pEntry->version_major > 2)
			|| (pEntry->version_major == 2 && pEntry->version_minor > 1))
		{
			// packed with newer version than supported?
			// -> try it anyway?
		}
		
		if (pEntry->data_position == 0 || pEntry->data_position > m_nFileSize)
		{
			// invalid position?
			// skip it or abort? (can we extract others even..?)
			++it;
            continue;
		}
		
		bool bRes = ExtractEntry(pEntry, archive);
        if (bRes == false)  
        {
			// or just warning and skip to next?
	        throw ArcException("Failed to extract file", pEntry->fileName);
	    }
		++it;
	}

	return true;
}

// with user selections
//
//bool CUnZoo::ExtractSelection(QList<QString> &lstSelectedFiles)
//{}
