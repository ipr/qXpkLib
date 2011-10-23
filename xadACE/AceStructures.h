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

// force 1-byte align, no padding
#pragma pack(push, 1)

struct tech
{
   uint8_t TYPE;
   uint8_t QUAL;
   uint16_t PARM;
};

typedef struct tacehead
{
   uint16_t HEAD_CRC;
   uint16_t HEAD_SIZE;
   uint8_t  HEAD_TYPE;
   uint16_t HEAD_FLAGS;
   uint32_t  ADDSIZE;
   uint8_t  other[2048];

}  thead;

typedef struct tacemhead
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
   uint8_t  AV[256];
   uint16_t COMM_SIZE;
   uint8_t  COMM[2048];

}  tmhead;


typedef struct tacefhead
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
   uint8_t  FNAME[PATH_MAX]; // check: can we replace with std::string
   uint16_t COMM_SIZE;
   uint8_t  COMM[2048]; // check: can we replace with std::string

}  tfhead;


// restore normal align
#pragma pack(pop)


#endif // ACESTRUCTURES_H
