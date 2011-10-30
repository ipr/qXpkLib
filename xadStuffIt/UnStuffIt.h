#ifndef UNSTUFFIT_H
#define UNSTUFFIT_H

#include <QObject>
#include <QString>

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

class StuffitEntry
{
public:
	StuffitEntry() {}
};

typedef std::vector<StuffitEntry*> tArchiveEntryList;

class CUnStuffIt
{
private:
	QString m_szArchive; // path and name of archive-file
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
	QString m_szExtractionPath;

	// internal buffer for read information
	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;

	void Clear()
	{
		auto it = m_EntryList.begin();
		auto itend = m_EntryList.end();
		while (it != itend)
		{
			StuffitEntry *pEntry = (*it);
			delete pEntry;
			++it;
		}
		m_EntryList.clear();
	}

public:
    CUnStuffIt(const QString &archive)
		: m_szArchive(archive)
		, m_nFileSize(0)
		, m_EntryList()
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_szExtractionPath()
		, m_ReadBuffer(1024) 
		, m_DecrunchBuffer(2048) 
    {}
    ~CUnStuffIt()
    {
		Clear();
    }

    bool isSupported(const uint8_t *pBuffer) const
    {
        return false;
    }
    
	// view a single archive
	bool List();
	
	bool GetEntryList(tArchiveEntryList &lstArchiveInfo) const;
	
	bool SetExtractPath(const QString &szOutPath);

	// extract all files in single archive
	bool Extract();

	// verify archive integrity
	bool TestArchive();

	// information about archive file itself
	QString GetArchiveFileName()
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

#endif // UNSTUFFIT_H
