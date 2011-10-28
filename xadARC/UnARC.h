#ifndef UNARC_H
#define UNARC_H

#include <QObject>
#include <QString>

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

#include "GenericTime.h"


enum ArcCompressionMode // in C++0x : uint8_t
{
	EOFMODE = 0, // end of file marker (in packing mode field?)
	UNPACKED1 = 1, // ARC 1.0 ?
	UNPACKED2 = 2, // ARC 3.1
	RLE = 3, // RLE (runlen) encoding
	SQUEEZED = 4, // acter packing?
	CRUNCHED5 = 5, // ARC 4.0
	CRUNCHED6 = 6, // ARC 4.1
	CRUNCHED_HASHED = 7, // ARC 4.6, hashing algorithm
	CRUNCHED_DYNLZW = 8, // ARC 5.0, dynamic LZW variations
	SQUASHED = 9, // c/o Phil Katz (no packing?)
	CRUSHED = 10, // PAK only?
	DISTILLED = 11 // PAK only?
	// 12..19 unknown, ARC 6.0-7.0 ?
	// 20..29 informational items? (ARC 6.0?)
	// 30..39 control items? (ARC 6.0?)
	// 40+ reserved
};

enum ArcInformationalItemType 
{
	ARCHIVE_INFO = 20,
	FILE_INFO = 21,
	OS_INFO = 22 // (reserved only?)
};


// archive metadata (archive-file header)
struct ArcMetadataInfo
{
	const uint32_t arc_header_size = 28;
	
	// constructor
	ArcMetadataInfo()
	 : m_id(0)
	{}

	uint8_t m_id; // one-byte identifier only..
	ArcCompressionMode compression_mode; // see ArcCompressionMode (single byte)
	
	std::string file_name; // archive name? (fixed size: 12 bytes)
	
	uint32_t compressed_size;
	time_t timestamp; // convert from generic time..
	
	uint16_t crc; // 16-bit crc
	uint32_t original_size;

	// these may be found in some additional information later..
	std::string arc_description;
	std::string arc_creator_name;
	std::string arc_modifier_name;
};

// entry descriptor in archive:
// describes following "header" type&size, 
// see ArcInformationalItemType list
struct ArcEntryDesc
{
	uint16_t desc_len; // includes "length" and "type" sizes?
	uint8_t desc_type; // type of entry
};

// archive entry information
class ArcEntry
{
public:
	ArcEntry() {}
	
	std::string file_description;
	std::string file_name;
	
	time_t timestamp;
	
	// icon ??
	
	// attributes
};

typedef std::vector<ArcEntry*> tArchiveEntryList;

class CUnARC
{
private:
	std::string m_szArchive; // path and name of archive-file
	size_t m_nFileSize; // filesize of archive in bytes

	// list of items in archive (files)
	tArchiveEntryList m_EntryList;

	// some counters for statistics of archive
	unsigned long m_ulTotalUnpacked;
	unsigned long m_ulTotalPacked;
	unsigned long m_ulTotalFiles;
	
	// user-given path where file(s) are 
	// extracted to from current archive
	// (may change on each extract() call..)
	std::string m_szExtractionPath;

	// internal buffer for read information
	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;

protected:

	// archive-file metadata (file header)
	ArcMetadataInfo m_metadata;
	void parseMetadata(const CReadBuffer *pBuf)
	{
		m_metadata.m_id = pBuf->GetNextByte();
		m_metadata.compression_mode = (ArcCompressionMode)pBuf->GetNextByte();
		
		// fixed-size string
		m_metadata.file_name.assign(pBuf->GetNext(12), 12);
		
		m_metadata.compressed_size = getULong(pBuf->GetNext(4));
		
		// just convert that shit..
		m_metadata.timestamp = (time_t)CGenericTime(getULong(pBuf->GetNext(4)));
		
		m_metadata.crc = getUWord(pBuf->GetNext(2));
		m_metadata.original_size = getULong(pBuf->GetNext(4));
	}


	// arc stores in little-endian order:
	// "last" byte from buffer is shifted most,
	// reuse code made for other unpackers.

	uint16_t getUWord(const uint8_t *pBuf) const
	{
		uint16_t res = pBuf[0];
		res += (pBuf[1] << 8);
		return res;
	}

	uint32_t getULong(const uint8_t *pBuf) const
	{
		uint32_t res = pBuf[0];
		res += (pBuf[1] << 8);
		res += (pBuf[2] << 16);
		res += (pBuf[3] << 24);
		return res;
	}

	bool readArchiveHeader(CAnsiFile &archive);
	
	//ArjEntry *readEntry(CAnsiFile &archive);
	bool readEntryList(CAnsiFile &archive);

	bool extractEntry(CAnsiFile &archive, ArcEntry *pEntry);

	void Clear()
	{
		auto it = m_EntryList.begin();
		auto itend = m_EntryList.end();
		while (it != itend)
		{
			ArcEntry *pEntry = (*it);
			delete pEntry;
			++it;
		}
		m_EntryList.clear();
	}

public:
    CUnARC(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_EntryList()
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_szExtractionPath()
		, m_ReadBuffer(1024) 
		, m_DecrunchBuffer(2048) 
		, m_metadata()
    {}
    ~CUnARC()
    {
		Clear();
    }

    bool isSupported(const uint8_t *pBuffer) const;
    
	// view a single archive
	bool List();
	
	bool GetEntryList(tArchiveEntryList &lstArchiveInfo) const;
	
	bool SetExtractPath(const std::string &szOutPath);

	// extract all files in single archive
	bool Extract();

	// verify archive integrity
	bool TestArchive();

	// information about archive file itself
	std::string GetArchiveFileName()
	{
		return m_szArchive;
	}
	size_t GetArchiveFileSize()
	{
		return m_nFileSize;
	}

	// statistical information access to caller
	unsigned long GetTotalSizeUnpacked() 
	{ 
		return m_ulTotalUnpacked; 
	}
	unsigned long GetTotalSizePacked() 
	{ 
		return m_ulTotalPacked; 
	}
	unsigned long GetTotalFileCount() 
	{ 
		return m_ulTotalFiles; 
	}
};

class CUnARC
{
public:
    CUnARC();
};

#endif // UNARC_H
