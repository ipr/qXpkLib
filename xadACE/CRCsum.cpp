/* ------------------------------------------------------------------------ */
/*                                                                          */
/*      CRC-calculation routines.                                           */
/*                                                                          */
/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */
/*
 * Modified for C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
*/


#include "CRCsum.h"

///////// protected methods

void CRCsum::make_crctable(void)   // initializes CRC table
{
	for (uint32_t i = 0; i <= 255; i++)
	{
		uint32_t r = i;
		for (uint32_t j = 8; j; j--)
		{
			r = (r & 1) ? (r >> 1) ^ CRCPOLY : (r >> 1);
		}
		crctable[i] = r;
	}
}


///////// public methods

CRCsum::CRCsum()
 : rd_crc(0)
{
	make_crctable();
}

// Updates crc from addr till addr+len-1
//
uint32_t CRCsum::getcrc(uint32_t crc, uint8_t *addr, const size_t length)
{
	size_t len = length;
	while (len--)
	{
		crc = crctable[(uint8_t) crc ^ (*addr++)] ^ (crc >> 8);
	}
	return (crc);
}
