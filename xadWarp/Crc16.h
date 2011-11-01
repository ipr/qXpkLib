/*
	yet another 16-bit CRC,
	should bw near-equal to other similar ?

	Original by Dirk Stoecker:
	http://libxad.cvs.sourceforge.net/libxad/libxad/portable/clients/
	
	WinUAE version by Toni Wilen:
	https://github.com/tonioni/WinUAE/blob/master/archivers/wrp/
	
	Modifications to use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
*/


#ifndef CRC16_H
#define CRC16_H

// use standard typedefs whenever possible
#include <stdint.h>


class Crc16
{
protected:
	uint16_t wrpcrc16table[256];

	void MakeCRC16(uint16_t *buf, uint16_t ID)
	{
		for (uint16_t i = 0; i < 256; ++i)
		{
			uint16_t k = i;
			for(uint16_t j = 0; j < 8; ++j)
			{
				if(k & 1)
				{
					k = (k >> 1) ^ ID;
				}
				else
				{
					k >>= 1;
				}
			}
			buf[i] = k;
		}
	}

public:
	Crc16()
	{
		MakeCRC16(wrpcrc16table, 0xa001);
	}
	
	uint16_t wrpcrc16(uint8_t *buf, int len)
	{
		uint16_t crc = 0;
		while (len-- > 0)
		{
			crc = wrpcrc16table[(crc ^ *buf++) & 0xFF] ^ (crc >> 8);
		}
		return crc;
	}
};

#endif // CRC16_H
