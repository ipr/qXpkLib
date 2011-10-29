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

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"


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
    {}
    ~CHA()
    {
		Clear();
    }

    bool isSupported(const uint8_t *pBuf) const
    {
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
