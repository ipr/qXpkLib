//////////////////////////////////////////////////////
//
// ZooStructures.h :
// rewrite of old shit into usable form
//
// Ilkka Prusi <ilkka.prusi@gmail.com>
//

#ifndef ZOOSTRUCTURES_H
#define ZOOSTRUCTURES_H

#include <string>

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

// conversion only
#include "GenericTime.h"


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
		comment.clear();
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
};

// variable (optional) details of entry (if any?)
// rewriting handling in progress..
//
class ZooVariableEntry
{
public:
	// constructor
	ZooVariableEntry()
		: variable_size(0)
		, timezone(127)
		, entry_crc(0)
		, systemid(0)
		, permissions(0)
		, modgen(0)
		, version(0)
	{}

    uint16_t      variable_size;           /* length of variable part         */
    uint8_t       timezone;         /* time zone                       */
    uint16_t      entry_crc;         /* crc value of entry              */
    
    // TODO: replace with QString / std::string
    // -> moved to "normal" entry
    //char                fileName [256];     /* namu: univ. name of member of archive */
    //char                dirName [256];     /* diru: univ. name of directory         */
    
    uint16_t      systemid;         /* system identifier               */
    
    // permissions actually mode_t ? (chmod() as-is)
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
		: magicid(0)
		, member_type(0)
		, method(0)
		, next_entry_pos(0)
		, data_position(0)
		, timestamp(0)
		, data_crc(0)
		, original_size(0)
		, compressed_size(0)
		, version_major(0)
		, version_minor(0)
		, deleted(0)
		, spared(0)
		, comment_position(0)
		, comment_size(0)
		, comment()
		, fileName()
		, pathName()
		, m_pVarDetails(nullptr)
	{}
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

	// keep full file & path names here
	std::string fileName;
	std::string pathName;

    // variable-sized (and optional?) details of entry (if any)
    //
    ZooVariableEntry *m_pVarDetails;
    
    std::string getName() const
    {
		if (pathName.length() == 0)
		{
			return fileName;
		}
		
		std::string temp = pathName;
		if (temp.at(temp.length() - 1) != '/')
		{
			temp += "/";
		}
		temp += fileName;
		return temp;
    }
    
    void setTimestamp(CGenericTime &generic)
    {
		timestamp = (time_t)generic;
    }
    
};


#endif // ZOOSTRUCTURES_H
