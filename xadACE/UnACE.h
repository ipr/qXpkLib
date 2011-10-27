//////////////////////////////////////
//
// UnACE.h
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef UNACE_H
#define UNACE_H

#include <QObject>
#include <QString>

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

// CRC checksumming now in own class,
// needed here and in decompress..
#include "CRCsum.h"

// decompression now in own class
#include "Decompress.h"

#include "AceStructures.h"

// file-entry in archive
class AceEntry
{
public:
	AceEntry()
	{}
	
	std::string fileName;
	std::string comment;
	
	acefileheader m_header;
};


typedef std::vector<AceEntry*> tArchiveEntryList;

class CUnACE
{
private:
	std::string m_szArchive; // path and name of archive-file
	size_t m_nFileSize; // filesize of archive in bytes

	// archive header
	acearchiveheader m_archiveHeader;

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

	// for header checksumming at least..
	// see about sharing if data&header share same checksum..
	CRCsum m_Crc;
	
	// could inherit from this..
	CDecompress m_Decompress;

protected:
	// ace stores in little-endian order:
	// "last" byte from buffer is shifted most

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
	bool readEntryList(CAnsiFile &archive);

	// TODO: need volume-information and access to multiple volumes..
	bool extractEntry(CAnsiFile &archive, AceEntry *pEntry);

	void Clear()
	{
		auto it = m_EntryList.begin();
		auto itend = m_EntryList.end();
		while (it != itend)
		{
			AceEntry *pEntry = (*it);
			delete pEntry;
			++it;
		}
		m_EntryList.clear();
	}

public:
    CUnACE(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_EntryList()
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_szExtractionPath()
		, m_ReadBuffer(1024) // size_rdb
		, m_DecrunchBuffer(2048) // size_wrb
		, m_Crc()
		, m_Decompress()
    {}
    ~CUnACE()
    {
		Clear();
    }
    
    // expecting at least 14 bytes..
    bool isSupported(const uint8_t *pBuf) const
    {
        char *pData = (char*)(pBuf + 7);
        if (::memcmp(pData, "**ACE**", 7) == 0)
        {
			// ACE archiver
			return true;
        }
        return false;
    }
    
	// view a single archive:
	// get archive metadata
	// and list of each entry in the archive
	//
	bool List();
    
	// list contents
	//
	// note: user must NOT destroy the objects which have pointers in the list,
	// the objects are destroyed by this class.
	//
	bool GetEntryList(tArchiveEntryList &lstArchiveInfo) const;

	bool SetExtractPath(const std::string &szOutPath);

	// extract all files in single archive:
	// give path where files are extracted to,
	// additional directories are created under that (if necessary)
	//
	bool Extract();
	
	// TODO:
	// verify archive integrity
	//
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

#endif // UNACE_H
