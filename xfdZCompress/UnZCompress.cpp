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


static uint8_t rmask[9] = {0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};


/* Decompress. This routine adapts to the codes in the file building the
 * "string" table on-the-fly; requiring no table to be stored in the
 * compressed file.
 */

/* negative values are error codes, positive value is resulting size */
int32_t CUnZCompress::decomp(CompressData &cd)
{
	cd.inptr = (int8_t*)m_pInput->GetAt(3); // two for id, one for bits
	cd.inendptr = (int8_t*)m_pInput->GetEnd();

	int32_t finchar, code, oldcode;
	finchar = oldcode = getcode(cd);
	if (finchar == -1)	/* EOF already ? */
	{
		throw IOException("corrupted data (EOF)");
		//return -XFDERR_CORRUPTEDDATA;		/* Get out of here */
	}
	//*(outptr++) = (uint8_t)finchar; /* first code must be 8 bits = UBYTE */
	m_pOutput->SetNextByte(finchar); /* first code must be 8 bits = UBYTE */

	// stack for collecting "plaintext" from uncompression
	int8_t *stackp = cd.stack;
	int8_t *stack = cd.stack; // no need to keep, can access whenever..

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
		
		finchar = cd.tab_suffixof[code];
		*stackp++ = cd.tab_suffixof[code];
		
		size_t nStackOut = (stackp - stack);
		if (nStackOut > m_pOutput->GetSpaceSize()) 
		{
			// prepare more
			m_pOutput->Reserve(nStackOut);
		}
		/*
		if ((stackp - stack) > (outend - outptr)) // buffer was to short :-( 
		{
			throw IOException("buffer too short");
		}
		*/

		/* And put them out in forward order */
		// (may be overlapped?) 
		// reversed anyway so no direct buf copying/appending..
		//
		do
		{
			m_pOutput->SetNextByte(*(--stackp));
			//m_pOutput->SetNextByte(*(stackp));
			//--stackp; // can we separate or does this screw up order..?
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
	return m_pOutput->GetCurrentPos();
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
			// if current larger than end -> error
			// what if it's equal? error or ok? (EOF?)
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

size_t CUnZCompress::uncompress()
{
	uint8_t bits = *(m_pInput->GetAt(2));  // skip two (id)

	// allocate from stack
	// -> automatic cleanup in destructor on leaving scope
	CompressData cd;
	if (cd.init(bits, MAXCODE(INIT_BITS)) == false)
	{
		throw IOException("allocation failed");
	}
	
	// start with twice the size,
	// no need to keep old data (full file only).
	// we don't know exact output size
	// so allocate enough at once (more efficient than growing later..)
	m_pOutput->PrepareBuffer(m_pInput->GetSize() * 4, false);

	// decompress now
	int32_t outSize = decomp(cd);
	if (outSize > 0)
	{
		return outSize;
	}
	return 0;
}

