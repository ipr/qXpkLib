#ifndef CRCIO_H
#define CRCIO_H

#include <stdint.h>
#include <limits.h>

// 16-bit CRC table and counting,
// could make static instance of this..
// CRC table is only modified when instance is created.
//
class CCrcIo
{
protected:
	unsigned int m_crctable[UCHAR_MAX + 1];

	void make_crctable();

	// table is not modified -> make const
	inline unsigned int getCRC(const unsigned int crc, const unsigned int c) const
	{
		return m_crctable[(crc ^ c) & 0xFF] ^ (crc >> CHAR_BIT);
	}
	
public:
    CCrcIo()
	{
		make_crctable();
	}
	
	unsigned int UpdateCrc(const unsigned int crc, const unsigned int c) const
	{
		return getCRC(crc, c);
	}
	
	unsigned int calccrc(unsigned int crc, unsigned char *data, unsigned int size);
};

#endif // CRCIO_H
