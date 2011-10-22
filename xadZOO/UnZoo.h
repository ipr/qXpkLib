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

#include "crcsum.h"

// metadata from archive
// note: we parse this member-by-member from file,
// don't cast buffer to this since it's not byte-packed 
// (may have padding)
//
struct ZooDescription
{
	// constructor
	ZooDescription()
	{
		// make sure to initialize some optional values,
		// initalize rest also..
		description.clear();
		magicid = 0;
		first_entry_pos = 0;
		klhvmh = 0;
		version_major = 0;
		version_minor = 0;
		member_type = 0;
		comment_pos = 0;
		comment_size = 0;
		modgen = 0;
	}

    std::string description;         // 20 bytes: "ZOO 2.10 Archive.<ctr>Z"
    uint32_t    magicid;             // magic type id, dword 0xfdc4a7dc           
    uint32_t    first_entry_pos;     // position of first directory ent.
    uint32_t    klhvmh;              // two's complement of posent  (why?)
    uint8_t     version_major;       // major version needed to extract 
    uint8_t     version_minor;       // minor version needed to extract 
    uint8_t     member_type;         // type of current member (0,1)    
    uint32_t    comment_pos;         // position of comment, 0 if none  
    uint32_t    comment_size;        // length   of comment, 0 if none  
    uint8_t     modgen;              // gens. on, gen. limit  (??? WTF ??? any LESS descriptive comment?)
    
    // expecting at least 24 bytes from header in buffer
    bool isSupported(const uint8_t *pBuffer) const
    {
		if (::memcmp(pBuffer, "ZOO ", 4) == 0
			&& pBuffer[20] == 0xfd
			&& pBuffer[21] == 0xc4
			&& pBuffer[22] == 0xa7
			&& pBuffer[23] == 0xdc
			)
		{
			return true;
		}
		return false;
    }
};

// variable (optional) details of entry (if any?)
struct ZooVariableEntry
{
};

struct ZooEntry
{
    uint32_t    magicid;           // magic-id dword 0xfdc4a7dc       
    uint8_t     member_type;       // type of current member (1)      
    uint8_t     method;            // packing method of member (0..2) 
    uint32_t    next_entry_pos;    // position of next member         
    uint32_t    data_position;     // position of data                
    uint16_t    date_dos;          // date (in DOS format)            
    uint16_t    time_dos;          // time (in DOS format)            
    uint16_t    data_crc;          // crc value of member             
    uint32_t    original_size;     // uncompressed size of member     
    uint32_t    compressed_size;   // compressed size of member     
    uint8_t     version_major;     // major version needed to extract 
    uint8_t     version_minor;     // minor version needed to extract 
    uint8_t     deleted;           // 1 if member is deleted, 0 else  (??)
    uint8_t     spared;            // spare entry to pad entry -> just padding?
    uint32_t    comment_position;  // position of comment, 0 if none  
    uint16_t    comment_size;      // length   of comment, 0 if none  

	////////////    
    // remove this, pointless old hack:
    // only use if proper name is empty, read to temp and determine there..
    // only msdos ever needed this kind of stuff?
    char                nameshort [14];      /* short name of member or archive */
	////////////    

    // variable-sized (and optional?) details of entry (if any)
    ZooVariableEntry *m_pVarDetails;
    
    unsigned short      lvar;           /* length of variable part         */
    unsigned char       timzon;         /* time zone                       */
    unsigned short      crcent;         /* crc value of entry              */
    unsigned char       lnamu;          /* length of long name             */
    unsigned char       ldiru;          /* length of directory             */
    
    // TODO: replace with QString / std::string
    char                fileName [256];     /* namu: univ. name of member of archive */
    char                dirName [256];     /* diru: univ. name of directory         */
    
    unsigned short      systemid;         /* system identifier               */
    unsigned long       permis;         /* file permissions                */
    unsigned char       modgen;         /* gens. on, last gen., gen. limit */
    unsigned short      ver;            /* version number of member        */
    
    time_t            timestamp; /* timestamp in usable form */
};

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

protected:

	ZooDescription m_archiveInfo;
	tEntryList m_EntryList;

	// inline: compiler can avoid function call by replacing call with function body
	inline int PEEK_BITS(const int N, const unsigned long bits, const unsigned long bitc) const
	{
		return ((bits >> (bitc - N)) & ((1L << N)-1));
	}

	// inline: compiler can avoid function call by replacing call with function body
	inline void FLSH_BITS(const int N, unsigned long &bits, unsigned long &bitc) const
	{
		if ( (bitc -= N) < 16 ) 
		{ 
			bits  = (bits << 16) + FlahReadArch(); 
			bitc += 16; 
		}
	}
	
	uint16_t getUWord(const uint8_t *pBuf) const
	{
		uint16_t res = pBuf[0];
		res += (pBuf[1] << 8);
		return res;
	}
	
	// reversed byte-order on some values..?
	uint16_t getUWordReversed(const uint8_t *pBuf) const
	{
		uint16_t res = pBuf[1];
		res += (pBuf[0] << 8);
		return res;
	}

	// ?? three-byte value?
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
		, m_ulTotalUnpacked(0)
		, m_ulTotalPacked(0)
		, m_ulTotalFiles(0)
		, m_szExtractionPath()
		, m_ReadBuffer(16384) 
		, m_DecrunchBuffer(16384*2) 
		, m_crc()
		, m_archiveInfo()
		, m_EntryList()
    {
    }
    ~CUnZoo()
    {
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
    
	bool SetExtractPath(const std::string &szOutPath)
	{
		m_szExtractionPath = szOutPath;
		return true;
	}

	bool GetEntryList(tEntryList &lstArchiveInfo);

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
