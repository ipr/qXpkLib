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


#ifndef HA_H
#define HA_H

#include <QObject>
#include <QString>

// use standard typedefs when possible:
// prefer portable types whenever size is specific
#include <stdint.h>
#include <limits.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

/* Method types	*/
enum MethodType // in C++0x : uint8_t
{
	M_CPY=0,
	M_ASC,
	M_HSC,
	M_UNK,
	M_DIR=14,
	M_SPECIAL
};

const int MyVersion = 2;			/* Version info in archives 	*/
const int LowVersion = 2;			/* Lowest supported version 	*/


/* Header of file in archive 	*/
struct Fheader
{
	// constructor
	Fheader()
		: method_type(0)
		, version(0)
		, comp_len(0)
		, orig_len(0)
		, time(0)
		, crc(0)
		, path(nullptr)
		, name(nullptr)
		, mdilen(0)
		, mylen(0)
	{}
	
	bool isSupported()
	{
		if (version > MyVersion
			|| version < LowVersion)
		{
			return false;
		}
		if (method_type != M_SPECIAL 
			&& method_type != M_DIR 
			&& method_type >= M_UNK)
		{
			return false;
		} 
		return true;
	}
	
	size_t calcLen()
	{
		mylen = mdilen + 20 + path.length() + name.length();
		return mylen;
	}

    uint8_t method_type; // see MethodType
    uint8_t version;
    uint32_t comp_len; // compressed length?
    uint32_t orig_len; // original length?
    uint32_t time;
    uint32_t crc;
    
    // replaced with common std::string
    std::string path;
    std::string name;
    
    // CHECK! these were just "unsigned" without proper size..
    // check size for reading from file/buffer..
    uint8_t mdilen; // just 8-bit unsigned?
    size_t mylen; // calculated, not read directly
};



// file-entry in archive
class HAEntry
{
public:
	HAEntry()
	{}
	
	std::string fileName;
	std::string comment;
};

typedef std::vector<HAEntry*> tArchiveEntryList;

class CHA
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

	// metadata of archive itself
	Fheader m_archiveHeader;

	uint32_t getvalue(const size_t nLen) const
	{
		// get next N bytes (update offset)
		uint8_t *buf = m_ReadBuffer.GetNext(nLen);
		uint32_t val = 0;
		for (int i = 0; i < nLen; i++)
		{
			// the last shift is somewhat odd..
			// should shift by 8 anyway..?
			val |= (uint32_t)((buf[i]&0xFF) << (i<<3));
		}
		return val;
	}
	
	// read NULL-terminated string from file.. 
	size_t getstring(CAnsiFile &archive, std::string &string) const
	{
		do
		{
			char c = UCHAR_MAX;
			if (archive.Read(&c, 1) == false)
			{
				throw IOException("Failure reading NULL-terminated string");
			}
			string += c;
		} while (c != 0);
		
		return string.length();
	}
	
	bool readHeader(CAnsiFile &archive, Fheader &header);

	bool readArchiveHeader(CAnsiFile &archive);
	bool readEntryList(CAnsiFile &archive);

	bool extractEntry(CAnsiFile &archive, HAEntry *pEntry);

	void Clear()
	{
		auto it = m_EntryList.begin();
		auto itend = m_EntryList.end();
		while (it != itend)
		{
			HAEntry *pEntry = (*it);
			delete pEntry;
			++it;
		}
		m_EntryList.clear();
	}

public:
    CHA(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_EntryList()
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_szExtractionPath()
		, m_ReadBuffer(1024) //
		, m_DecrunchBuffer(2048) // 
		, m_archiveHeader()
    {}
    ~CHA()
    {
		Clear();
    }

    bool isSupported(const uint8_t *pBuffer) const
    {
		if (pBuffer[0] == 'H'
			&& pBuffer[1] == 'A')
		{
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

#endif // HA_H
