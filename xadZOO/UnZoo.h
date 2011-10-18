#ifndef UNZOO_H
#define UNZOO_H

#include <string>
#include <vector>

// use from master-library
// to reuse file-handling code
#include "AnsiFile.h"

#include "crcsum.h"


typedef vector<Entry*> tEntryList;

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
	tEntryList m_EntryList;

	// inline: compiler can avoid function call by replacing call with function body
	inline int PEEK_BITS(const int N, const unsigned long bits, const unsigned long bitc)
	{
		return ((bits >> (bitc - N)) & ((1L << N)-1));
	}

	// inline: compiler can avoid function call by replacing call with function body
	inline void FLSH_BITS(const int N, unsigned long &bits, unsigned long &bitc)
	{
		if ( (bitc -= N) < 16 ) 
		{ 
			bits  = (bits << 16) + FlahReadArch(); 
			bitc += 16; 
		}
	}


	bool DecodeCopy(unsigned long size, CAnsiFile &archive, CAnsiFile &outFile);
	bool DecodeLzd(unsigned long size, CAnsiFile &archive, CAnsiFile &outFile);
	bool DecodeLzh(unsigned long size, CAnsiFile &archive, CAnsiFile &outFile);

	// read contents of archive
	void ReadArchive(CAnsiFile &archive);
	bool readArchiveDescription(CAnsiFile &archive);


	bool ListArchive(const std::string &archiveName);
	bool ExtrArch(const std::string &archiveName);

public:
    CUnZoo(const std::string &szArchive)
		: m_szArchive(szArchive)
		, m_nFileSize(0)
		, m_szExtractionPath()
		, m_ReadBuffer(16384) 
		, m_DecrunchBuffer(16384*2) 
		, m_crc()
		, m_EntryList()
    {
    }
    ~CUnZoo()
    {
		auto it = m_EntryList.begin();
		auto itend = m_EntryList.end();
		while (it != itend)
		{
			Entry *pEntry = (*it);
			delete pEntry;
			++it;
		}
		m_EntryList.clear();
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
