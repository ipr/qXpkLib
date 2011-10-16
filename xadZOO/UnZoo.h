#ifndef UNZOO_H
#define UNZOO_H

#include <string>
#include <vector>

// use from master-library
// to reuse file-handling code
#include "AnsiFile.h"

#include "crcsum.h"

//typedef vector<> tEntryList;

class CUnZoo
{
private:
	std::string m_szArchive; // path and name of archive-file
	size_t m_nFileSize; // filesize of archive in bytes

	// user-given path where file(s) are 
	// extracted to from current archive
	// (may change on each extract() call..)
	std::string m_szExtractionPath;

	// internal buffer for read information
	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
	crcsum m_crc;

protected:
	//tEntryList m_EntryList;

	bool DecodeCopy(unsigned long size, CAnsiFile &archive, CAnsiFile &outFile);
	//bool DecodeLzd();
	//bool DecodeLzh(unsigned long size, CAnsiFile &archive, CAnsiFile &outFile);

	// read contents of archive
	void ReadArchive(CAnsiFile &archive);


	bool ListArch(const std::string &archiveName);
	bool ExtrArch(const std::string &archiveName);

public:
    CUnZoo(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_szExtractionPath()
		, m_ReadBuffer(16384) 
		, m_DecrunchBuffer(16384*2) 
    {
    }
    
	bool SetExtractPath(const std::string &szOutPath)
	{
		m_szExtractionPath = szOutPath;
		return true;
	}

	bool GetEntryList(tEntryList &lstArchiveInfo);
    
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
};

#endif // UNZOO_H
