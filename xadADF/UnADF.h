#ifndef UNADF_H
#define UNADF_H

#include <QObject>
#include <QString>

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

// for each file
// (no need for directories)
//
class FileEntry
{
public:
	FileEntry()
	{}
	
	std::string fileName;
	std::string comment;
	
	size_t fileSize;
};

typedef std::vector<FileEntry*> tEntryList;

class CUnADF
{
private:
	//std::string m_szArchive; // path and name of archive-file
	//size_t m_nFileSize; // filesize of archive in bytes

	// list of items (files)
	tEntryList m_EntryList;

	CReadBuffer *m_pInput;
	
	void Clear()
	{
		auto it = m_EntryList.begin();
		auto itend = m_EntryList.end();
		while (it != itend)
		{
			FileEntry *pEntry = (*it);
			delete pEntry;
			++it;
		}
		m_EntryList.clear();
	}

public:
	/*
    CUnADF(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		*/
    CUnADF(CReadBuffer *pInput)
		: m_pInput(pInput)
		, m_EntryList()
	{}
	~CUnADF()
	{
		Clear();
	}
	
	// just access to list of contents:
	// caller must not destroy list
	//
	bool GetEntryList(tEntryList &lstInfo) const
	{
		lstInfo = m_EntryList;
		return true;
	}
	
	//bool SetExtractPath(const std::string &szOutPath);
	
	bool List();
	
	// extract each entry
	bool Extract();
	
	//bool ExtractSelected(QString &fileEntry);
	
};

#endif // UNADF_H
