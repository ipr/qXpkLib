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
* Modified for use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
*
*/

#ifndef UNZOO_H
#define UNZOO_H

#include <string>
#include <vector>

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

// use from master-library
// to reuse file-handling code
#include "AnsiFile.h"

// moved stuff to separate files for clarity
#include "crcsum.h"
#include "ZooStructures.h"

// moved LZH-decoding
#include "Decode.h"


typedef vector<ZooEntry*> tEntryList;

class CUnZoo
{
private:
	std::string m_szArchive; // path and name of archive-file
	size_t m_nFileSize; // filesize of archive in bytes

	// some counters for statistics of archive
	size_t m_ulTotalUnpacked;
	size_t m_ulTotalPacked;
	size_t m_ulTotalFiles;

	// user-given path where file(s) are 
	// extracted to from current archive
	// (may change on each extract() call..)
	std::string m_szExtractionPath;

	// internal buffer for read information
	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
	crcsum m_crc;

	DecodeZoo m_decode;

protected:

	ZooDescription m_archiveInfo;
	tEntryList m_EntryList;

    // expecting at least 24 bytes from header in buffer
    bool isSupportedArchive(const uint8_t *pBuffer) const
    {
		// should have 20 bytes: "ZOO 2.10 Archive.<ctr>Z"
		if (::memcmp(pBuffer, "ZOO ", 4) == 0
			&& pBuffer[23] == 0xfd
			&& pBuffer[22] == 0xc4
			&& pBuffer[21] == 0xa7
			&& pBuffer[20] == 0xdc
			)
		{
			return true;
		}
		return false;
    }


	// expecting 4 bytes from entry start
    bool isSupportedEntry(const uint8_t *pBuffer) const
    {
		if (pBuffer[3] == 0xfd
			&& pBuffer[2] == 0xc4
			&& pBuffer[1] == 0xa7
			&& pBuffer[0] == 0xdc)
		{
			return true;
		}
		return false;
    }
    
    bool fixPathname(std::string &name) const
    {
		// should be better ways, this is simple enough..
		for (size_t n = 0; n < name.length(); n++)
		{
			if (name[n] == '\\')
			{
				name[n] = '/';
			}
		}
		return true;
    }
    
	
	uint16_t getUWord(const uint8_t *pBuf) const
	{
		uint16_t res = pBuf[0];
		res += (pBuf[1] << 8);
		return res;
	}

	// ?? three-byte value?
	// only one case where used..
	uint32_t getTriple(const uint8_t *pBuf) const
	{
		uint32_t res = pBuf[0];
		res += (pBuf[1] << 8);
		res += (pBuf[2] << 16);
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


	// read contents of archive
	void readString(CAnsiFile &archive, const size_t offset, const size_t length, std::string &value);
	bool readArchiveEntryList(CAnsiFile &archive);
	bool readArchiveDescription(CAnsiFile &archive);

	bool ExtractEntry(ZooEntry *pEntry, CAnsiFile &archive);
	
	void Clear()
	{
		// statisticals are counted
		// -> clear when opening
		
		m_nFileSize = 0;
		m_ulTotalUnpacked = 0;
		m_ulTotalPacked = 0;
		m_ulTotalFiles = 0;
		
		auto it = m_EntryList.begin();
		auto itend = m_EntryList.end();
		while (it != itend)
		{
			ZooEntry *pEntry = (*it);
			delete pEntry;
			++it;
		}
		m_EntryList.clear();
	}

public:
    CUnZoo(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_szExtractionPath()
		, m_ReadBuffer(16384) 
		, m_DecrunchBuffer(16384*2) 
		, m_crc()
		, m_decode()
		, m_archiveInfo()
		, m_EntryList()
    {}
    ~CUnZoo()
    {
		Clear();
    }
    
	bool SetExtractPath(const std::string &szOutPath)
	{
		m_szExtractionPath = szOutPath;
		if (m_szExtractionPath.at(m_szExtractionPath.length() -1) != '/')
		{
			m_szExtractionPath += "/";
		}
		return true;
	}
	
	ZooDescription *GetArchiveInfo()
	{
		return &m_archiveInfo;
	}

	// note: caller must not destroy objects,
	// just access the information
	bool GetEntryList(tEntryList &lstArchiveInfo)
	{
		lstArchiveInfo = m_EntryList;
		return true;
	}

	// get list of archive entries to entry-list
	bool ListContents();
    
	bool TestArchive();

	// extract files in archive:
	// give path where files are extracted to,
	// additional directories are created under that (if necessary)
	//
	bool Extract();

	// with user selections
	//
	//bool ExtractSelection(QList<QString> &lstSelectedFiles);
	
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
	size_t GetTotalSizeUnpacked() 
	{ 
		return m_ulTotalUnpacked; 
	}
	size_t GetTotalSizePacked() 
	{ 
		return m_ulTotalPacked; 
	}
	size_t GetTotalFileCount() 
	{ 
		return m_ulTotalFiles; 
	}
};

#endif // UNZOO_H
