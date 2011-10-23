/* ------------------------------------------------------------------------ */
/*  ML - 01/2004: changed licence to GPL                                    */
/* ------------------------------------------------------------------------ */  
/*
 * Modified for C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
*/


#ifndef CRCSUM_H
#define CRCSUM_H

// use ISO typedefs
#include <stdint.h>

class CRCsum
{
protected:
	const uint32_t CRC_MASK = 0xFFFFFFFFL;
	const uint32_t CRCPOLY  = 0xEDB88320L;

	uint32_t crctable[256];
	uint32_t rd_crc;

	void make_crctable(void);

public:
    CRCsum();
    
    // just to detect uses..
    void ClearCrc()
    {
		rd_crc = CRC_MASK;
    }
    
	uint32_t getcrc(uint32_t crc, uint8_t *addr, const size_t length);

};

#endif // CRCSUM_H
