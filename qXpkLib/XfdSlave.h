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

// simplify asm to c conversion,
// for each register with access methods
struct datareg
{
	union
	{
		int8_t b;
		int16_t w;
		int32_t l;
	};
};

struct addrreg
{
	uint8_t *src;
	
	int8_t b()
	{
		return *src++;
	}
	
	int16_t w()
	{
		int16_t *p = (int16_t*)src;
		src += 2;
		return *p;
	}
	
	int32_t l()
	{
		int32_t *p = (int32_t*)src;
		src += 4;
		return *p;
	}
};


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

	// TODO: keep regs as member?
	// will it be simpler like this?
	//datareg D[8];
	//addrreg A[8];
	
public:
    XfdSlave(CReadBuffer *pIn)
		: m_pIn(pIn)
    {}
    bool decrunch(CReadBuffer *pOut) = 0;
};

////////// ByteKiller

class XfdByteKiller : public XfdSlave
{
protected:
	bool crun(CReadBuffer *pOut, uint8_t *src, uint32_t D0);
	bool crnd(CReadBuffer *pOut, uint8_t *src, uint32_t D0);
	bool marc(CReadBuffer *pOut, uint8_t *src, uint32_t D0);
	
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
