#include "crcio.h"

void CCrcIo::make_crctable()
{
	/* CRC-16 (x^16+x^15+x^2+1) */
	const unsigned int CRCPOLY = 0xA001; 
	
    for (unsigned int i = 0; i <= UCHAR_MAX; i++) 
	{
        unsigned int r = i;
        for (unsigned int j = 0; j < CHAR_BIT; j++)
		{
            if (r & 1)
			{
                r = (r >> 1) ^ CRCPOLY;
			}
            else
			{
                r >>= 1;
			}
		}
        m_crctable[i] = r;
    }
}

unsigned int CCrcIo::calccrc(unsigned int crc, unsigned char *data, unsigned int size)
{
    while (size-- > 0)
	{
        crc = getCRC(crc, *data++);
	}
    return crc;
}

