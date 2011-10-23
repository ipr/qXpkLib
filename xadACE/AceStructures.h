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
enum BlockTypes
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


// force 1-byte align, no padding
#pragma pack(push, 1)

struct tech
{
   uint8_t TYPE;
   uint8_t QUAL;
   uint16_t PARM;
};

// ?
struct tacehead
{
   uint16_t HEAD_CRC;
   uint16_t HEAD_SIZE;
   uint8_t  HEAD_TYPE;
   uint16_t HEAD_FLAGS;
   uint32_t  ADDSIZE;
   uint8_t  other[2048]; // ? variable-sized? -> remove from here..
};

// archive header contents
struct tacemhead
{
   uint16_t HEAD_CRC;
   uint16_t HEAD_SIZE;
   uint8_t  HEAD_TYPE;
   uint16_t HEAD_FLAGS;

   uint8_t  ACESIGN[acesign_len];
   uint8_t  VER_MOD;
   uint8_t  VER_CR;
   uint8_t  HOST_CR;
   uint8_t  VOL_NUM;
   uint32_t  TIME_CR;
   uint16_t RES1;
   uint16_t RES2;
   uint32_t  RES;
   uint8_t  AV_SIZE;
   uint8_t  AV[256]; // is this really fixed-length? if not, remove from here..
   uint16_t COMM_SIZE;
   uint8_t  COMM[2048]; // is this really fixed-length? if not, remove from here..
};

// file-entry header?
struct tacefhead
{
   uint16_t HEAD_CRC;
   uint16_t HEAD_SIZE;
   uint8_t  HEAD_TYPE;
   uint16_t HEAD_FLAGS;

   uint32_t  PSIZE;
   uint32_t  SIZE;
   uint32_t  FTIME;
   uint32_t  ATTR;
   uint32_t  CRC32;
   struct tech TECH;
   uint16_t RESERVED;
   uint16_t FNAME_SIZE;
   uint8_t  FNAME[512]; // check: can we replace with std::string
   uint16_t COMM_SIZE;
   uint8_t  COMM[2048]; // check: can we replace with std::string
};

// restore normal align
#pragma pack(pop)


#endif // ACESTRUCTURES_H
