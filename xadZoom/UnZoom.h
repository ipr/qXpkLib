#ifndef UNZOOM_H
#define UNZOOM_H

// use standard typedefs when possible
#include <stdint.h>

#include <string>
#include <vector>

// use wrapper(s) from parent-library
#include "AnsiFile.h"

// Zoom version and
// self-extracting types
// with Amiga executable header
//
enum ZoomType
{
	Unknown = 0,
	ZOOM = 1,
	ZOOM5 = 5,
	LHPAK = 6,
	PCOMPPAK = 7,
	SOMNI = 8,
	LHSFX = 9
};

class CUnZoom
{
private:

	CReadBuffer m_DecrunchBuffer;

	// TODO: as ptr for parent-instances instead?
	CReadBuffer *m_pInputBuffer;
	CReadBuffer *m_pOutputBuffer;
	
	ZoomType m_type;


protected:

	uint16_t getUWord(const uint8_t *pBuf) const
	{
		return ((pBuf[0] << 8) + pBuf[1]);
	}
	uint32_t getULong(const uint8_t *pBuf) const
	{
		return ((pBuf[0] << 24) + (pBuf[1] << 16) + (pBuf[2] << 8) + pBuf[3]);
	}
	

	ZoomType getType(CReadBuffer *buf);
	
public:
    CUnZoom(CReadBuffer *pIn, CReadBuffer *pOut)
		: m_pInputBuffer(pIn)
		, m_pOutputBuffer(pOut)
		, m_DecrunchBuffer() 
		, m_type(Unknown)
	{}
	~CUnZoom()
	{}
	
    
    bool isSupported(CReadBuffer *buf) const
    {
		if (getType(buf) != Unknown)
		{
			return true;
		}
		return false;
    }

	bool unpack();
};

#endif // UNZOOM_H
