/*
   Unrar source may be used in any software to handle RAR archives
   without limitations free of charge, but cannot be used to re-create
   the RAR compression algorithm, which is proprietary. Distribution
   of modified Unrar source in separate form or as a part of other
   software is permitted, provided that it is clearly stated in
   the documentation and source comments that the code may not be used
   to develop a RAR (WinRAR) compatible archiver.

   More detailed license text is available in license.txt.
*/
/*
	Based on Portable UnRAR version (AmigaOS4 port).
	Authors: Alexander Roshal, Nicolas Mendoza, Joachim Birging, Stephan Matzke, Joerg Strohmayer
	
	This C++ library version by: Ilkka Prusi <ilkka.prusi@gmail.com>
*/

#ifndef UNRAR_H
#define UNRAR_H

#include <QObject>
#include <QString>

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

enum RarFormat
{
	Unknown = 0,
	RAR_OLD = 1,
	RAR_NEW = 2,
	RAR_SFX = 99 // self-extracting RAR (executable)
};

// force 1-byte align, no padding
#pragma pack(push, 1)

// TODO: new format file entry handling

// old-format file entry header
struct RarOldFileHeader
{
	uint32_t	PackSize;
	uint32_t	UnpSize;
	uint16_t	FileCRC;
	uint16_t	HeadSize;
	uint32_t	FileTime;
	uint8_t		FileAttr;
	uint8_t		Flags;
	uint8_t		UnpVer;
	uint8_t		NameSize;
	uint8_t		Method;
};

// restore normal align
#pragma pack(pop)


class RarEntry
{
public:
	RarEntry(RarOldFileHeader *pFile) 
	{}
};

typedef std::vector<RarEntry*> tArchiveEntryList;

class CUnRAR
{
private:
	// (this stuff we could inherit, same for most archive-types..)
	
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
	// archive header,
	// wrap old and new style
	class RarHeader
	{
	public:
		RarHeader() 
		 : m_format(Unknown)
		 , m_headerSize(0)
		 , m_oldFlags(0)
		{}
		
		RarFormat m_format; // new/old/SFX..
		size_t m_headerSize; // size of archive header
		uint8_t m_oldFlags; // old-style flags (parse?)
		
		// TODO: parse flags from value or keep as-is?
		void setOldFlags(uint8_t flags)
		{
			m_oldFlags = flags;
		}
	};
	RarHeader m_archiveHeader;


	RarFormat getRarFormat(const uint8_t *pBuffer) const
	{
		if (pBuffer[0] == 0x52
			&& pBuffer[1] == 0x61
			&& pBuffer[2] == 0x72
			&& pBuffer[3] == 0x21
			&& pBuffer[4] == 0x1a
			&& pBuffer[5] == 0x07
			&& pBuffer[6] == 0x00)
		{
			return RAR_NEW;
		}
		else if (pBuffer[0] == 0x52
				&& pBuffer[1] == 0x45
				&& pBuffer[2] == 0x7e
				&& pBuffer[3] == 0x5e)
		{
			return RAR_OLD;
		}

		//
		// note: for SFX need to seek past
		// the header of executable
		// (which platform/format size? windows/msdos only?)
		//
		
		return Unknown;
	}

	// rar stores in little-endian order:
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
	
	RarEntry *readEntry(CAnsiFile &archive);

	bool readArchiveHeader(CAnsiFile &archive);
	bool readEntryList(CAnsiFile &archive);
	
public:
    CUnRAR(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_EntryList()
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_szExtractionPath()
		, m_ReadBuffer() 
		, m_DecrunchBuffer() 
		, m_archiveHeader()
    {}
    
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

#endif // UNRAR_H
