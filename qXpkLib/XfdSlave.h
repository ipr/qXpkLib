///////////////////////////////////
//
// XFD-decrunching support:
// slave-decrunchers for use by XFD-master.
//
// (May be used in XpkMaster when XFD-decruncher is needed.)
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XFDSLAVE_H
#define XFDSLAVE_H

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

#include "AnsiFile.h"

// pure virtual interface 
// for XFD-slave decruncher implementations
//
class XfdSlave
{
protected:
	CReadBuffer *m_pIn;
	
	uint16_t GetUWord(const uint8_t *pBuf)
	{
		return ((pBuf[0] << 8) + pBuf[1]);
	}
	uint32_t GetULong(const uint8_t *pBuf)
	{
		return ((pBuf[0] << 24) + (pBuf[1] << 16) + (pBuf[2] << 8) + pBuf[3]);
	}

	// need better way of sharing code..
	uint32_t MakeTag(const uint8_t *buf) const
    {
        uint32_t tmp = 0;
        tmp |= (((uint32_t)(buf[3])) << 24);
        tmp |= (((uint32_t)(buf[2])) << 16);
        tmp |= (((uint32_t)(buf[1])) << 8);
        tmp |= ((uint32_t)(buf[0]));
        return tmp;
    }
	
public:
    XfdSlave(CReadBuffer *pIn)
		: m_pIn(pIn)
    {}
    bool decrunch(CReadBuffer *pOut) = 0;
};

////////// ByteKiller

class XfdByteKiller : public XfdSlave
{
public:
    bool decrunch(CReadBuffer *pOut);
};


////////// Vice

class XfdVice : public XfdSlave
{
public:
    bool decrunch(CReadBuffer *pOut);
};


////////////////////

#endif // XFDSLAVE_H
