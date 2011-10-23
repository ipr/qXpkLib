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

enum ZooPackMethod // : in C++0x uint8_t
{
	PackCopyOnly = 0,
	PackLzd = 1, // not supported currently ? (is it directory only like in normal Lha/Lzh ?)
	PackLzh = 2
};

// metadata from archive,
// we parse this member-by-member from file..
// changed to class for better choices in handling..
//
class ZooDescription
{
public:
	// constructor
	ZooDescription()
		: description()
		, magicid(0)
		, first_entry_pos(0)
		, klhvmh(0)
		, version_major(0)
		, version_minor(0)
		, member_type(0)
		, comment_pos(0)
		, comment_size(0)
		, modgen(0)
		, header_size(0)
		, is_new_style(false)
	{
		// make sure to initialize some optional values,
		// initalize rest also..
		description.clear();
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

    std::string comment;
    
    // note: part of header fields don't exist in old-style files,
    // they were added later -> keep track of amount actually read
    size_t      header_size; // amount read for header
    bool        is_new_style; // if new style header
    
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
// rewriting handling in progress..
//
class ZooVariableEntry
{
public:
	// constructor
	ZooVariableEntry()
	{
		variable_size = 0;
		timezone = 127; 
		entry_crc = 0;
		
		systemid = 0;
		permissions = 0;
		modgen = 0;
		version = 0;
	}

    uint16_t      variable_size;           /* length of variable part         */
    uint8_t       timezone;         /* time zone                       */
    uint16_t      entry_crc;         /* crc value of entry              */
    
    // no need to keep here, just read and get it done
    //uint8_t       lnamu;          /* length of long name             */
    //uint8_t       ldiru;          /* length of directory             */
    
    // TODO: replace with QString / std::string
    // -> moved to "normal" entry
    //char                fileName [256];     /* namu: univ. name of member of archive */
    //char                dirName [256];     /* diru: univ. name of directory         */
    
    uint16_t      systemid;         /* system identifier               */
    uint32_t      permissions;         /* file permissions                */
    uint8_t       modgen;         /* gens. on, last gen., gen. limit */ // so it's not permissions? wtf is it?
    uint16_t      version;            /* version number of member        */
};

// single entry in archive
//
class ZooEntry
{
public:
	// constructor
	ZooEntry()
	{
		magicid = 0;
		member_type = 0;
		method = 0;
		next_entry_pos = 0;
		data_position = 0;
		timestamp = 0;
		data_crc = 0;
		original_size = 0;
		compressed_size = 0;
		version_major = 0;
		version_minor = 0;
		deleted = 0;
		spared = 0;
		comment_position = 0;
		comment_size = 0;
		
		comment.clear();
		fileName.clear();
		pathName.clear();
		
		m_pVarDetails = nullptr;
	}
	// destructor
	~ZooEntry()
	{
		if (m_pVarDetails != nullptr)
		{
			delete m_pVarDetails;
			m_pVarDetails = nullptr;
		}
	}
	
    uint32_t    magicid;           // magic-id dword 0xfdc4a7dc       
    uint8_t     member_type;       // type of current member (1, 2?), what are the types??    
    uint8_t     method;            // packing method of member (0..2) 
    uint32_t    next_entry_pos;    // position of next member         
    uint32_t    data_position;     // position of data                
    
    //uint16_t    date_dos;          // date (in DOS format)            
    //uint16_t    time_dos;          // time (in DOS format)          
    time_t      timestamp;         // timestamp in actually usable form (conversion)
  
    uint16_t    data_crc;          // crc value of member             
    uint32_t    original_size;     // uncompressed size of member     
    uint32_t    compressed_size;   // compressed size of member     
    uint8_t     version_major;     // major version needed to extract 
    uint8_t     version_minor;     // minor version needed to extract 
    uint8_t     deleted;           // 1 if member is deleted, 0 else  (??)
    uint8_t     spared;            // spare entry to pad entry -> just padding?
    uint32_t    comment_position;  // position of comment, 0 if none  
    uint16_t    comment_size;      // length   of comment, 0 if none  

    // -> read comment here, remove extra values from here
    std::string comment;


	////////////    
    // remove this, pointless old hack:
    // only use if proper name is empty, read to temp and determine there..
    // only msdos ever needed this kind of stuff?
    //char                nameshort [14];      /* short name of member or archive */
	////////////    

	// keep full file & path names here
	std::string fileName;
	std::string pathName;

    // variable-sized (and optional?) details of entry (if any)
    //
    ZooVariableEntry *m_pVarDetails;
    
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

	// expecting 4 bytes from entry start
    bool isSupportedEntry(const uint8_t *pBuffer) const
    {
		if (pBuffer[0] == 0xfd
			&& pBuffer[1] == 0xc4
			&& pBuffer[2] == 0xa7
			&& pBuffer[3] == 0xdc)
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
    
	// simplify code, reusable method
	void bufferSet(const uint16_t code, uint16_t *pBuf, const int count) const
	{
		// note: 16-bit int buffer, expecting element count..
		for (int n = 0; n < count; n++)
		{
			pBuf[n] = code;
		}
	}


	// inline: compiler can avoid function call by replacing call with function body
	inline int PEEK_BITS(const int N, const unsigned long bits, const unsigned long bitc) const
	{
		return ((bits >> (bitc - N)) & ((1L << N)-1));
	}

	// inlining won't give any benefit since function call(s) are made anyway
	// (old called at least two functions, we call one or two depending on inlining..)
	void FLSH_BITS(const int N, unsigned long &bits, unsigned long &bitc) const
	{
		if ( (bitc -= N) < 16 ) 
		{ 
			bits  = (bits << 16) + getUWordReversed(m_ReadBuffer.GetAtCurrent()); 
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

	bool MakeTablLzh(const int nchar, const unsigned char *bitlen, const int tablebits, uint16_t *table);
	bool DecodeLzh(ZooEntry *pEntry, CAnsiFile &archive, CAnsiFile &outFile);

	// read contents of archive
	void readString(CAnsiFile &archive, const size_t offset, const size_t length, std::string &value);
	bool readArchiveEntryList(CAnsiFile &archive);
	bool readArchiveDescription(CAnsiFile &archive);

	bool ExtractEntry(ZooEntry *pEntry, CAnsiFile &archive);
	
	void Clear()
	{
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
		, m_archiveInfo()
		, m_EntryList()
    {
    }
    ~CUnZoo()
    {
		Clear();
    }
    
	bool SetExtractPath(const std::string &szOutPath)
	{
		m_szExtractionPath = szOutPath;
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
