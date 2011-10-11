///////////////////////////////////
//
// XFD-decrunching support:
// should load additional decoders as necessary.
//
// May be called from XpkMaster when XFD-decruncher is needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XFDMASTER_H
#define XFDMASTER_H

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

#include "AnsiFile.h"


// pure virtual interface for slave-implementations
class CXfdSlave
{
protected:
	CReadBuffer *m_pIn;

public:
    CXfdSlave(CReadBuffer *pIn)
		: m_pIn(pIn)
    {}
    
    bool decrunch(CReadBuffer *pOut) = 0;
};


class CXfdMaster
{
protected:
	uint32_t MakeTag(const unsigned char *buf) const
    {
        uint32_t tmp = 0;
        tmp |= (((uint32_t)(buf[3])) << 24);
        tmp |= (((uint32_t)(buf[2])) << 16);
        tmp |= (((uint32_t)(buf[1])) << 8);
        tmp |= ((uint32_t)(buf[0]));
        return tmp;
    }

    // TODO:
    //CXfdSlave *getXfdSlave( /*type or buffer*/ ) const;
    //CXfdSlave *m_pXfdSlave;

public:
    CXfdMaster();
    
    bool isXfdSupported(CReadBuffer *pInputBuffer) const;
    
    
};

#endif // XFDMASTER_H
