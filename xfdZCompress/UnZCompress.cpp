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


#include "UnZCompress.h"


#define STACKSIZE	 (8000L)
#define FIRST		  (257)		/* first free entry */
#define CLEAR		  (256)		/* table clear output code */
#define INIT_BITS	    (9)		/* initial number of bits/code */


static uint8_t rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};


/* Decompress. This routine adapts to the codes in the file building the
 * "string" table on-the-fly; requiring no table to be stored in the
 * compressed file.
 */

/* negative values are error codes, positive value is resulting size */
int32_t CUnZCompress::decomp(CompressData &cd)
{
	// TODO: replace direct pointers by buffer-wrapper
	// -> we can grow when needed
	//
	int8_t *inbuf = (int8_t*)m_pInput->GetBegin();
	uint32_t insize = m_pInput->GetSize();
	int8_t *outbuf = (int8_t*)m_pOutput->GetBegin();
	uint32_t outsize = m_pOutput->GetSize();
	
	int8_t *outptr = outbuf;
	int8_t *stackp = cd.stack;
	int8_t *stack = cd.stack;
	int8_t *outend = outbuf+outsize;

	cd.maxbits = inbuf[2];
	cd.block_compress = cd.maxbits & BLOCK_MASK;
	cd.maxbits &= BIT_MASK;
	cd.maxmaxcode = 1 << cd.maxbits;
	cd.n_bits = INIT_BITS;
	cd.maxcode = MAXCODE(INIT_BITS);
	cd.free_ent = ((cd.block_compress) ? FIRST : 256);
	cd.clear_flg = 0;
	cd.offset = 0;
	cd.size = 0;
	cd.inptr = inbuf+3;
	cd.inendptr = inbuf+insize;

	/* Initialize the first 256 entries in the table. */
	cd.initTable();

	int32_t finchar, code, oldcode;
	finchar = oldcode = getcode(cd);
	if (finchar == -1)	/* EOF already ? */
	{
		throw IOException("corrupted data (EOF)");
		//return -XFDERR_CORRUPTEDDATA;		/* Get out of here */
	}
	*(outptr++) = (uint8_t)finchar; /* first code must be 8 bits = UBYTE */

	while((code = getcode(cd)) > -1)
	{
		if(code == CLEAR && cd.block_compress)
		{
			for(code = 255; code >= 0; code--)
			{
				cd.tab_prefixof[code] = 0;
			}
			cd.clear_flg = 1;
			cd.free_ent = FIRST - 1;
			code = getcode(cd);
			if(code == -1)
			{
				throw IOException("untimely death");
				//break;					/* O, untimely death! */
			}
		}
		
		int32_t incode = code;

		/* Special case for KwKwK string. */
		if(code >= cd.free_ent)
		{
			*stackp++ = finchar;
			code = oldcode;
		}

		/* Generate output characters in reverse order */
		while(code >= 256)
		{
			*stackp++ = cd.tab_suffixof[code];
			code = cd.tab_prefixof[code];
		}
		*stackp++ = finchar = cd.tab_suffixof[code];

		if(stackp - stack > outend - outptr) /* buffer was to short :-( */
		{
			throw IOException("buffer too short");
			//return -XFDERR_CORRUPTEDDATA;
		}

		/* And put them out in forward order */
		do
		{
			*(outptr++) = *(--stackp);
		} while(stackp > stack);

		/* Generate the new entry. */
		code = cd.free_ent;
		if (code < cd.maxmaxcode)
		{
			cd.tab_prefixof[code] = (uint16_t)oldcode;
			cd.tab_suffixof[code] = finchar;
			cd.free_ent = code+1;
		}
		/* Remember previous code. */
		oldcode = incode;
	}
	return (outptr-outbuf);
}

/* Read one code from input. If EOF, return -1. */
int32_t CUnZCompress::getcode(CompressData &cd)
{
	uint8_t *bp = cd.buf;

	if (cd.clear_flg > 0 || cd.offset >= cd.size || cd.free_ent > cd.maxcode)
	{
		/*
		 * If the next entry will be too big for the current code
		 * size, then we must increase the size.  This implies reading
		 * a new buffer full, too.
		 */
		if(cd.free_ent > cd.maxcode)
		{
			if(++cd.n_bits == cd.maxbits)
			{
				cd.maxcode = cd.maxmaxcode;	/* won't get any bigger now */
			}
			else
			{
				cd.maxcode = MAXCODE(cd.n_bits);
			}
		}
		if(cd.clear_flg > 0)
		{
			cd.n_bits = INIT_BITS;
			cd.maxcode = MAXCODE(cd.n_bits);
			cd.clear_flg = 0;
		}

		if(cd.inendptr <= cd.inptr)
		{
			// error case?
			//throw IOException("untimely death");
			return -1;			/* end of file */
		}

		/* This reads maximum n_bits characters into buf */
		cd.size = 0;
		while(cd.size < cd.n_bits && cd.inptr < cd.inendptr)
		{
			cd.buf[cd.size++] = *(cd.inptr++);
		}

		cd.offset = 0;
		/* Round size down to integral number of codes */
		cd.size = (cd.size << 3) - (cd.n_bits - 1);
	}
	
	int32_t r_off = cd.offset;
	int32_t bits = cd.n_bits;

	/* Get to the first byte. */
	bp += (r_off >> 3);
	r_off &= 7;

	/* Get first part (low order bits) */
	int32_t code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	r_off = 8 - r_off;			/* now, offset into code word */

	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if(bits >= 8)
	{
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}

	/* high order bits. */
	code |= (*bp & rmask[bits]) << r_off;
	cd.offset += cd.n_bits;

	return code;
}

///////////// public methods

bool CUnZCompress::uncompress()
{
	uint8_t bits = *(m_pInput->GetAt(2));
	uint32_t maxmaxcode = 1 << (bits & BIT_MASK);

	// allocate from stack
	// -> automatic cleanup in destructor on leaving scope
	CompressData cd;
	if (cd.allocBuf(maxmaxcode, STACKSIZE) == false)
	{
		throw IOException("allocation failed");
	}
	
	// start with twice the size,
	// no need to keep old data (full file only)
	m_pOutput->PrepareBuffer(m_pInput->GetSize(), false);

	// decompress now
	int32_t outSize = decomp(cd);
	if (outSize > 0)
	{
		return true;
	}
	return false;
}

