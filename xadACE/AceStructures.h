/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */
/*
 * Modifications for C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
*/


#ifndef ACESTRUCTURES_H
#define ACESTRUCTURES_H

// use ISO typedefs
#include <stdint.h>

// dislike of fixed-length arrays,
// replace arrays with std::string if possible..
//
#include <string>


//archive-header-flags
enum ArchiveHeaderFlags // in C++0x : uint16_t
{
	ACE_LIM256   = 1024,
	ACE_MULT_VOL = 2048,
	ACE_AV       = 4096,
	ACE_RECOV    = 8192,
	ACE_LOCK     = 16384,
	ACE_SOLID    = 32768
};

//file-header-flags
enum FileHeaderFlags // also uint16_t
{
	ACE_ADDSIZE  = 1,
	ACE_PASSW    = 16384,
	ACE_SP_BEF   = 4096,
	ACE_SP_AFTER = 8192,
	ACE_COMM     = 2
};

//block types
enum BlockTypes // uint8_t
{
	MAIN_BLK = 0,
	FILE_BLK = 1,
	REC_BLK  = 2
};

//known compression types
enum CompressionTypes
{
	TYPE_STORE = 0,
	TYPE_LZ1   = 1
};

const int32_t acesign_len           = 7;
const int32_t bytes_before_acesign  = 7;
const int32_t acever                = 10;

const int32_t aceheader_common_len  = 7;

// force 1-byte align, no padding
// on second though, don't cast to structures..
//#pragma pack(push, 1)

// what are these values?
struct tech
{
	tech()
	{
		TYPE = 0;
		QUAL = 0;
		PARM = 0;
	}
	
	uint8_t		TYPE;
	uint8_t		QUAL;
	uint16_t	PARM;
};

// shared start of header for 
// archive-header and entry-header
//
struct tacehead
{
	// constructor
	tacehead()
	{
		HEAD_CRC = 0;
		HEAD_SIZE = 0;
		HEAD_BLOCK_TYPE = 0;
		HEAD_FLAGS = 0;
	}

	uint16_t	HEAD_CRC;
	uint16_t	HEAD_SIZE; // size of header data
	uint8_t		HEAD_BLOCK_TYPE; // see BlockTypes
	uint16_t	HEAD_FLAGS;
};

// archive header contents
//
struct acearchiveheader
{
	acearchiveheader()
	 : header()
	{
		ADDSIZE = 0;
		VER_MOD = 0;
		VER_CR = 0;
		HOST_CR = 0;
		VOL_NUM = 0;
		TIME_CR = 0;
		RES1 = 0;
		RES2 = 0;
		RES = 0;
	}

	tacehead	header;
	
	uint32_t	ADDSIZE; // (optional) additional field, bytes to skip in entry headers?

	// no need to keep this..
	//uint8_t		ACESIGN[acesign_len]; // 7 characters
	uint8_t		VER_MOD;
	uint8_t		VER_CR;
	uint8_t		HOST_CR;
	uint8_t		VOL_NUM;
	uint32_t	TIME_CR; // replace with time_t ..?
	uint16_t	RES1;
	uint16_t	RES2;
	uint32_t	RES;
	
	//uint8_t  AV_SIZE; // authentication-verification information?
	//uint8_t  AV[256]; // is this really fixed-length? if not, remove from here..
	//uint16_t COMM_SIZE;
	//uint8_t  COMM[2048]; // is this really fixed-length? if not, remove from here..
};

// file-entry header contents
//
struct acefileheader
{
	acefileheader()
	 : header()
	 , TECH()
	{
		PSIZE = 0;
		SIZE = 0;
		FTIME = 0;
		ATTR = 0;
		CRC32 = 0;
	}
	
	tacehead	header;

	uint32_t	PSIZE;
	uint32_t	SIZE;
	uint32_t	FTIME; // timestamp? replace with time_t ..?
	uint32_t	ATTR; // attributes? which format?
	uint32_t	CRC32;
	struct tech	TECH;
	uint16_t	RESERVED;
	
	//uint16_t FNAME_SIZE;
	//uint8_t  FNAME[512]; // check: can we replace with std::string
	//uint16_t COMM_SIZE;
	//uint8_t  COMM[2048]; // check: can we replace with std::string
};

// restore normal align
//#pragma pack(pop)


#endif // ACESTRUCTURES_H
