/*
 * This C++ library version by: Ilkka Prusi <ilkka.prusi@gmail.com>
 * reuses multiple parts of code from qLhaLib (now xadLHA) based on Unix LHa
 * since ARJ uses same static Huffman as LHa (methods 4..7)
 * instead of rewriting old unarj-decoding.
 *
 * Based on Unix LhA source code: lha-1.14i-ac20040929
 * Copyrights:
 * LHarc    for UNIX  V 1.02  Copyright(C) 1989  Y.Tagawa
 * LHx      for MSDOS V C2.01 Copyright(C) 1990  H.Yoshizaki
 * LHx(arc) for OSK   V 2.01  Modified     1990  Momozou
 * LHa      for UNIX  V 1.00  Copyright(C) 1992  Masaru Oki
 * LHa      for UNIX  V 1.14  Modified     1995  Nobutaka Watazaki
 * LHa      for UNIX  V 1.14i Modified     2000  Tsugio Okamoto
 *                    Autoconfiscated 2001-2004  Koji Arai
 * 
 * C++ library version (qLhaLib and xadLHA), modifications by Ilkka Prusi (2011).
 *
 */

#ifndef DECODE_H
#define DECODE_H

#include <stdint.h>
#include <limits.h>

// use wrapper(s) from parent-library
#include "AnsiFile.h"


// Reuse rewrite of "bit-io" handling
// from qLhaLib (now xadLHA) -- IPr.
//
// all file-IO was made through functions in bitio,
// which is now this class taking buffers only:
// read/write in larger chunks instead of per-character now.
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


class CHuffmanTree
{
protected:

	enum tHuffmanTree
	{
		C_TABLE_LEN		= 4096, // element count, not bytes
		PT_TABLE_LEN	= 256, // element count, not bytes
		
		
		NC_LEN          = (UCHAR_MAX + MAXMATCH + 2 - THRESHOLD), // 510
		C_LEN_SIZE		= NC_LEN, // temp..
		PT_LEN_SIZE		= 0x80 // pt_len array size (8*1024/64)==128==0x80
	};

	
	uint16_t left[2 * NC_LEN - 1];
	uint16_t right[2 * NC_LEN - 1];
	
	uint16_t c_table[C_TABLE_LEN];   /* decode */
	uint16_t pt_table[PT_TABLE_LEN];   /* decode */
	
	// used by shuffle and static
	uint8_t c_len[C_LEN_SIZE];
	
	// used by shuffle and static
	uint8_t pt_len[PT_LEN_SIZE];
	
	// protected: only derived instance possible
    CHuffmanTree()
	{}
	
public:
	void make_table(const int16_t nchar, uint8_t bitlen[], int16_t tablebits, uint16_t table[]);

protected:
	inline void make_table_tree(const int treeLen, const int j, unsigned int &i, uint16_t *pTbl, int &avail);
};


class DecodeArj : public CHuffmanTree
{
public:
    DecodeArj()
     : CHuffmanTree()
    {}
};

#endif // DECODE_H
