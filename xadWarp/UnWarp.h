/*
	Amiga Warp disk-image unpacking code.

	Original by Dirk Stoecker:
	http://libxad.cvs.sourceforge.net/libxad/libxad/portable/clients/
	
	WinUAE version by Toni Wilen:
	https://github.com/tonioni/WinUAE/blob/master/archivers/wrp/
	
	Modifications to use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
*/

#ifndef UNWARP_H
#define UNWARP_H

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

// moved to separate for cleanup
#include "Crc16.h"

// compression algorithms used by Warp
// for decrunching
//
enum WarpAlgo
{
	ALGO_RLE1 = 0, // runlen
	ALGO_Z = 1, // same method as Un*x Z-compress?
	ALGO_ARCSQUEEZE = 2,
	ALGO_RLE2 = 3 // runlen, alternate?
};

// max uncompressed image size:
// 1760 blocks * 512 bytes in block = 901120 bytes
//
const int32_t max_imagesize = 1760 * 512;

// Warp disk image/track info
struct WarpDiskInfo
{
	// "Warp v1.1"
	uint8_t warpIdentifier[10];
	uint16_t track;
	uint8_t algo;
	int8_t side;
	uint16_t crc;
	uint32_t size;
	
	size_t nOffset;
	size_t nDestPos;
	
	// 26 bytes required from start
	void parseBuf(const uint8_t *pBuf)
	{
		::memcpy(warpIdentifier, pBuf, 10);
		track = (buf[10] << 8) | buf[11];
		algo = buf[19];
		side = -1;
		if (memcmp (buf + 12, "BOT\0", 4) == 0)
		{
			side = 1;
		}
		if (memcmp (buf + 12, "TOP\0", 4) == 0)
		{
			side = 0;
		}
		crc = (buf[20] << 8) | buf[21];
		size = (buf[22] << 24) | (buf[23] << 16) | (buf[24] << 8) | buf[25];
		
	}
	void setDestOffset()
	{
		if (side >= 0 && track >= 0 && track <= 79)
		{
			nDestPos = track * 22 * 512 + (side * 11 * 512);
		}
	}
};

class CUnWarp
{
private:

	std::string m_sourceFile; // temp, see changes planned to parent-library
	std::string m_destFile; // temp, see changes planned to parent-library
	size_t m_nFileSize; // filesize of archive in bytes

	CReadBuffer m_ReadBuffer;
	CReadBuffer m_DecrunchBuffer;
	
protected:
	Crc16 m_crc16;
	
	void decrunchRle(WarpDiskInfo *info);
	void unsqueezeArc(WarpDiskInfo *info);
	void uncompress(WarpDiskInfo *info);

public:
    CUnWarp()
		: m_sourceFile()
		, m_destFile()
		, m_nFileSize(0)
		, m_ReadBuffer() 
		, m_DecrunchBuffer() 
		, m_crc16()
	{}
	~CUnWarp()
	{}
    
    bool isSupported(CReadBuffer *buf) const
    {
		uint8_t *buffer = buf->GetBegin();
		if (buffer[0] == 'W' && buffer[1] == 'a' && buffer[2] == 'r' && buffer[3] == 'p'
			&& buffer[4] == ' ' && buffer[5] == 'v' && buffer[6] == '1' && buffer[7] == '.'
			&& buffer[8] == '1' && !buffer[9] && !buffer[18] && buffer[19] <= 3)
		{
			return true;
		}
		return false;
    }

	// TODO: allow to/from buffer/file unpacking..
	// implement necessary details..
	// better way to pass IOcontext from master to here
	// as planned, just need to determine interface..

	// file as source
	void setSourceFile(const std::string &szDms)
	{
		m_sourceFile = szDms;
	}
	// file as dest
	void setDestFile(const std::string &szDms)
	{
		m_destFile = szDms;
	}
	
	/*
	// TODO:
	// client-buffer as source..
	void setSourceBuffer(CReadBuffer *pIn)
	{}
	// client-buffer as dest..
	void setDestBuffer(CReadBuffer *pOut)
	{}
	*/

	bool unpack();
};

#endif // UNWARP_H
