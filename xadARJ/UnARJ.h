#ifndef UNARJ_H
#define UNARJ_H

#include <QObject>
#include <QString>

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

// timestamp conversion
#include "GenericTime.h"

// file mode flags (attributes, protection bits)
#include "FilemodeFlags.h"

// CRC checksumming now in own class,
// needed here and in decompress..
#include "CRCsum.h"


class ArjEntry
{
public:
	ArjEntry(const uint8_t fhs) 
		: first_header_size(fhs)
	{}
	
	uint8_t first_header_size;
	uint8_t arj_nbr;
	uint8_t arj_x_nbr;
	uint8_t host_os; // same types as Lha ?
	uint8_t arj_flags;
	uint8_t method; // packing method? same as Lha ?
	uint8_t file_type;
	uint8_t padding; // skipped, padding only?
	
	time_t timestamp; // just convert it..
	
	uint32_t compressed_size;
	uint32_t original_size;
	uint32_t file_crc;
	
	uint16_t entry_position;
	uint16_t file_mode;
	uint16_t host_data;
	
	// TODO: filename and rest..
};

typedef std::vector<ArjEntry*> tArchiveEntryList;

class CUnARJ
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

	// for header checksumming at least..
	// see about sharing if data&header share same checksum..
	CRCsum m_Crc;

	/* actually, skip this..? */
	// ARJ archive metadata
	struct ArjArchiveInfo
	{
		// constructor
		ArjArchiveInfo()
			: id(0)
			, headerSize(0)
			, crc(0)
		{}
		
		uint16_t id; // "magic-id"
		uint16_t headerSize; // archive header size
		uint32_t crc; // just header crc ?
	};
	ArjArchiveInfo m_archiveInfo;

	// arj stores in little-endian order:
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
	
	ArjEntry *readEntry(CAnsiFile &archive);
	bool readEntryList(CAnsiFile &archive);

	bool extractEntry(CAnsiFile &archive, ArjEntry *pEntry);

	void Clear()
	{
		auto it = m_EntryList.begin();
		auto itend = m_EntryList.end();
		while (it != itend)
		{
			ArjEntry *pEntry = (*it);
			delete pEntry;
			++it;
		}
		m_EntryList.clear();
	}

public:
    CUnARJ(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_EntryList()
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_szExtractionPath()
		, m_ReadBuffer(1024) 
		, m_DecrunchBuffer(2048) 
		, m_Crc()
		, m_archiveInfo()
    {}
    ~CUnARJ()
    {
		Clear();
    }

    bool isSupported(const uint8_t *pBuffer) const
    {
		if (pBuffer[0] == 0x60
			&& pBuffer[1] == 0xEA)
		{
			// ARJ identifier
			return true;
		}
        return false;
    }
    
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

#endif // UNARJ_H
