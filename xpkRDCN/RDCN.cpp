/* Algorithm (c) Ed Ross. See RDCN.doc (bottom) for more info. */
/* Amiga implementation by Niklas Sjberg (both C and .asm)
   reachable on 2:203/415.3@Fidonet
*/
/*
  Some C to C++ library code change by Ilkka Prusi <ilkka.prusi@gmail.com>
  - modern&portable typedefs (uint16_t, uint8_t)
  - remove unused variables
  - reduce scope of some for easier debugging
  etc.
*/

#include "RDCN.h"

bool CRDCN::decrunch(CReadBuffer *pIn, CReadBuffer *pOut,
			const uint32_t chunkIn, uint32_t &chunkOut)
{
	uint8_t *inbuff_idx = pIn->GetBegin();
	uint8_t *outbuff_idx = pOut->GetBegin();
	uint8_t *inbuff_end = inbuff_idx + chunkIn;
	uint16_t ctrl_bits;
	uint16_t ctrl_mask = 0;
	uint16_t ofs;

	/* process each item in inbuff */
	while (inbuff_idx < inbuff_end)
	{
		/* get new load of control bits if needed */
		if ((ctrl_mask >>= 1) == 0)
		{
		  ctrl_bits = *(uint16_t *) inbuff_idx;
		  inbuff_idx += 2;
		  ctrl_mask = 0x8000;
		}

		/* just copy this char if control bit is zero */
		if ((ctrl_bits & ctrl_mask) == 0)
		{
		  *outbuff_idx++ = *inbuff_idx++;
		  continue;
		}

		/* undo the compression code */
		uint16_t cmd = (*inbuff_idx >> 4) & 0x0F;
		uint16_t cnt = *inbuff_idx++ & 0x0F;

		if (cmd > 2)
		{
		  ofs = cnt + 3;
		  ofs += (*inbuff_idx++ << 4);
		  memcpy (outbuff_idx, outbuff_idx - ofs, cmd);
		  outbuff_idx += cmd;
		  continue;
		}

		switch (cmd)
		{
		case 0:		/* short rle */
		  cnt += 3;
		  memset (outbuff_idx, *inbuff_idx++, cnt);
		  outbuff_idx += cnt;
		  break;

		case 1:		/* long rle */
		  cnt += (*inbuff_idx++ << 4);
		  cnt += 19;
		  memset (outbuff_idx, *inbuff_idx++, cnt);
		  outbuff_idx += cnt;
		  break;

		case 2:		/* long pattern */
		  ofs = cnt + 3;
		  ofs += (*inbuff_idx++ << 4);
		  cnt = *inbuff_idx++;
		  cnt += 16;
		  memcpy (outbuff_idx, outbuff_idx - ofs, cnt);
		  outbuff_idx += cnt;
		  break;
		  /*Default is the most often used code, so test for this case first */
		  /*
					  default:  short pattern
						  ofs = cnt + 3;
						  ofs += (*inbuff_idx++ << 4);
						  memcpy(outbuff_idx, outbuff_idx - ofs, cmd);
						  outbuff_idx += cmd;
						  break;
		  */
		}
	}

	/* return length of decompressed buffer */
	chunkOut = (outbuff_idx - outbuff);
	return true;
}

