/*
 * unzoo.c by Martin Schoenert
 *
 * Modified for use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
 * 
 * Reuses code from qLhaLib and xadLHA, which are based on Unix LHA,
 * Copyrights:
 * LHarc    for UNIX  V 1.02  Copyright(C) 1989  Y.Tagawa
 * LHx      for MSDOS V C2.01 Copyright(C) 1990  H.Yoshizaki
 * LHx(arc) for OSK   V 2.01  Modified     1990  Momozou
 * LHa      for UNIX  V 1.00  Copyright(C) 1992  Masaru Oki
 * LHa      for UNIX  V 1.14  Modified     1995  Nobutaka Watazaki
 * LHa      for UNIX  V 1.14i Modified     2000  Tsugio Okamoto
 *                    Autoconfiscated 2001-2004  Koji Arai
 * 
 * Modifications for use in C++ library by Ilkka Prusi (2011).
 * 
 */


#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>
#include <limits.h>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

// moved stuff to separate files for clarity
#include "crcsum.h"
#include "ZooStructures.h"


// replace weird handling from original,
// reuse Lzh-related bitio with class already made for qLhaLib.
// taking buffers only:
// read/write in larger chunks instead of per-byte.
//
class BitIo
{
public:
	uint16_t bitbuf;
	uint8_t subbitbuf;
	uint8_t bitcount;

	size_t origsize; // (uncompressed) size of file
	size_t compsize; // compressed size of file


	// helper for accessing same data
	// in decoder and extract-handling
	CReadBuffer *m_pReadBuf;
	CReadBuffer *m_pWriteBuf;
	
	uint16_t peekbits(const uint8_t count)
	{
		return (bitbuf >> ((sizeof(bitbuf)*8) - count));
	}

	/* Shift bitbuf n bits left, read n bits */
	void fillbuf(const uint8_t count)
	{
		uint8_t n = count;
		while (n > bitcount) 
		{
	        n -= bitcount;
	        bitbuf = (bitbuf << bitcount) + (subbitbuf >> (CHAR_BIT - bitcount));
	        if (compsize != 0) 
			{
	            compsize--;
				subbitbuf = m_pReadBuf->GetNextByte();
	        }
	        else
			{
	            subbitbuf = 0;
			}
	        bitcount = CHAR_BIT;
	    }
	    bitcount -= n;
	    bitbuf = (bitbuf << n) + (subbitbuf >> (CHAR_BIT - n));
	    subbitbuf <<= n;
	}
	
	uint16_t getbits(const uint8_t count)
	{
		uint16_t x = (bitbuf >> (2 * CHAR_BIT - count));
		fillbuf(count);
		return x;
	}
	
	void init_getbits()
	{
		bitbuf = 0;
		subbitbuf = 0;
		bitcount = 0;
		fillbuf(2 * CHAR_BIT);
	}

public:
    BitIo()
		: m_pReadBuf(nullptr)
		, m_pWriteBuf(nullptr)
		, bitbuf(0)
		, subbitbuf(0)
	{}
	
};


class DecodeZoo
{
protected:
	crcsum m_crc;
	BitIo m_BitIo;

	// prefer constexpr over preprocessor macros
	//
	const int32_t MAX_LIT         = 255;     /* maximal literal code            */
	const int32_t MIN_LEN         = 3;       /* minimal length of match         */
	const int32_t MAX_LEN         = 256;     /* maximal length of match         */
	const int32_t MAX_CODE        = (MAX_LIT+1 + MAX_LEN+1 - MIN_LEN);
	const int32_t BITS_CODE       = 9;       /* 2^BITS_CODE > MAX_CODE (+1?)    */
	const int32_t MAX_OFF         = 8192;    /* 13 bit sliding directory        */
	const int32_t MAX_LOG         = 13;      /* maximal log_2 of offset         */
	const int32_t BITS_LOG        = 4;       /* 2^BITS_LOG > MAX_LOG (+1?)      */
	const int32_t MAX_PRE         = 18;      /* maximal pre code                */
	const int32_t BITS_PRE        = 5;       /* 2^BITS_PRE > MAX_PRE (+1?)      */
	
	// use constants for buffer-sizes also where fixed-length
	const int32_t c_TREE_SIZE = 2*MAX_CODE+1;
	const int32_t c_TAB_CODE_SIZE = 4096;
	
	uint16_t  TreeLeft [c_TREE_SIZE];        /* tree for codes   (upper half)   */
	uint16_t  TreeRight[c_TREE_SIZE];        /* and  for offsets (lower half)   */
	uint16_t TabCode  [c_TAB_CODE_SIZE];     /* table for fast lookup of codes  */
	uint8_t   LenCode  [MAX_CODE+1];         /* number of bits used for code    */
	uint16_t  TabLog   [256];                /* table for fast lookup of logs   */
	uint8_t   LenLog   [MAX_LOG+1];          /* number of bits used for logs    */
	uint16_t  TabPre   [256];                /* table for fast lookup of pres   */
	uint8_t   LenPre   [MAX_PRE+1];          /* number of bits used for pres    */


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
	inline uint32_t PEEK_BITS(const int N, const uint32_t bits, const uint32_t bitc) const
	{
		return ((bits >> (bitc - N)) & ((1L << N)-1));
	}

	// reversed byte-order on values accessed by Lzh-decode.. (see FLSH_BITS)
	uint16_t getUWordReversed(const uint8_t *pBuf) const
	{
		uint16_t res = pBuf[1];
		res += (pBuf[0] << 8);
		return res;
	}

	// inlining won't give any benefit since function call(s) are made anyway
	// (old called at least two functions, we call one or two depending on inlining..)
	void FLSH_BITS(const int N, uint32_t &bits, uint32_t &bitc) const
	{
		if ( (bitc -= N) < 16 ) 
		{
			uint8_t *pBuf =  m_BitIo.m_pReadBuf.GetAtCurrent();
			uint16_t tableBits = getUWordReversed(pBuf);
		
			bits  = (bits << 16) + tableBits; 
			bitc += 16; 
		}
	}

	bool MakeTablLzh(const int nchar, const uint8_t *bitlen, const int tablebits, uint16_t *table);

public:
    DecodeZoo()
     : m_crc()
     , m_BitIo()
    {}
    
	bool DecodeLzh(ZooEntry *pEntry);
	
	void setup(ZooEntry *pEntry, CReadBuffer *pIn, CReadBuffer *pOut)
	{
		m_BitIo.compsize = pEntry->compressed_size;
		m_BitIo.origsize = pEntry->original_size;
		m_BitIo.m_pReadBuf = pIn;
		m_BitIo.m_pWriteBuf = pOut;
		m_BitIo.init_getbits();
	}
    
};

#endif // DECODE_H
