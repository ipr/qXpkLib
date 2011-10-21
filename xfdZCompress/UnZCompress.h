/* Objectheader

	Name:		UNIX-Compress.c
	Description:	xfd external decruncher for UNIX compress files
	Author:		SDI (stoecker@epost.de)
	Distribution:	PD

 1.0   22.12.97 : first version
 1.1   23.12.97 : added memory expansion
 1.2   04.08.98 : bug fix for block compress, added internal SysBase
*/

/* Because there is no way to find out the size of uncrunched file, this
   routine uses a destination buffer, which is 4 times as large as the
   source file. When there is not enough memory, the largest memory block
   is used.
   This does surely not cover all Compress files! So there may be some
   which cannot be decrunched. XPKERR_CORRUPTEDDATA should be reported for
   these. In this case the program tries to allocate the largest available
   memory block and restarts decrunching.

   Has anyone a better idea how to find out destination length?
*/

/*
  As difference to above, this implementation just increases buffer size as necessary
  when more data is uncompressed: not fastest way but should work..

  C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
*/


#ifndef UNZCOMPRESS_H
#define UNZCOMPRESS_H

#include <stdint.h>

// from master-project for buffer-class
#include "AnsiFile.h"

const uint32_t STACKSIZE = 8000L; 
const uint32_t FIRST = 257;		/* first free entry */
const uint32_t CLEAR = 256;		/* table clear output code */
const uint32_t INIT_BITS = 9;		/* initial number of bits/code */

const uint32_t BITS			= 16;
/* Defines for third byte of header */
const uint32_t BIT_MASK		= 0x1f;
const uint32_t BLOCK_MASK	= 0x80;
const uint32_t LZW_RESERVED	= 0x60;
	/* Masks 0x40 and 0x20 are free. I think 0x20 should mean that
	   there is a fourth header byte (for expansion). */


class CompressData 
{
protected:

	bool allocBuf(const size_t stacksize)
	{
		tab_prefixof = (uint16_t*)::malloc(sizeof(uint16_t)*maxmaxcode);
		if (tab_prefixof == NULL)
		{
			return false;
		}
		stack = (int8_t*)::malloc(stacksize);
		if (stack == NULL)
		{
			return false;
		}
		tab_suffixof = (int8_t*)::malloc(maxmaxcode);
		if (tab_suffixof == NULL)
		{
			return false;
		}
		return true;
	}

	void initTable()
	{
		/* Initialize the first 256 entries in the table. */
		for(int32_t code = 255; code >= 0; code--)
		{
			tab_prefixof[code] = 0;
			tab_suffixof[code] = (uint8_t)code;
		}
	}

public:

	uint16_t  	block_compress;
	int16_t		clear_flg;
	uint16_t	n_bits;			/* number of bits/code */
	uint16_t  	maxbits;		/* user settable max # bits/code */
	uint32_t 	maxcode;		/* maximum code, given n_bits */
	uint32_t 	maxmaxcode;
	int32_t 	free_ent;
	int32_t		offset;
	int32_t		size;
	int8_t		*inptr;			/* current input pointer */
	int8_t		*inendptr;		/* end of input buffer */
	uint16_t	*tab_prefixof;
	int8_t		*tab_suffixof;
	int8_t		*stack;
	uint8_t		buf[BITS];
	
	CompressData()
		: tab_prefixof(NULL)
		, stack(NULL)
		, tab_suffixof(NULL)
		, n_bits(INIT_BITS)
		, clear_flg(0)
		, offset(0)
		, size(0)
	{}
	~CompressData()
	{
		if (tab_prefixof != NULL)
		{
			free(tab_prefixof);
		}
		if (stack != NULL)
		{
			free(stack);
		}
		if (tab_suffixof != NULL)
		{
			free(tab_suffixof);
		}
	}
	
	bool init(const uint8_t bits, const uint32_t initmaxcode)
	{
		maxbits = bits;
		maxcode = initmaxcode;
		block_compress = bits & BLOCK_MASK;
		maxbits &= BIT_MASK;
		maxmaxcode = (1 << maxbits);
		free_ent = ((block_compress) ? FIRST : 256);
		
		if (allocBuf(STACKSIZE) == false)
		{
			return false;
		}
		
		/* Initialize the first 256 entries in the table. */
		initTable();
		return true;
	}
};


class CUnZCompress
{
protected:

	inline uint32_t MAXCODE(uint32_t n)
	{
		return (((uint32_t)1 << n) - 1);
	}

	int32_t decomp(CompressData &cd);
	int32_t getcode(CompressData &cd);

	// buffers for input&output
	CReadBuffer *m_pInput;
	CReadBuffer *m_pOutput;

public:
    CUnZCompress(CReadBuffer *pInput, CReadBuffer *pOutput)
		: m_pInput(pInput)
		, m_pOutput(pOutput)
    {
    }
    
    bool isZCompress(const uint8_t *data, const size_t nLen)
    {
		if (data[0] == 0x1F && data[1] == 0x9D)
		{
			uint8_t mb = data[2];
			if (mb & LZW_RESERVED)		/* Unknown format */
			{
				return false;
			}
			if ((mb & BIT_MASK) > BITS)		/* Too much bits */
			{
				return false;
			}
			return true; // supported
		}
		return false;
    }
    
    size_t uncompress();
};

#endif // UNZCOMPRESS_H
