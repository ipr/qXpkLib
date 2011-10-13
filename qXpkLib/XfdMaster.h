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

#include "XfdSlave.h"

// status information (caller and decruncher)
#include "XpkProgress.h"


class CXfdMaster
{
protected:

	// need better way of sharing code..
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
    //XfdSlave *getXfdSlave( /*type or buffer*/ ) const;
    XfdSlave *m_pXfdSlave;
    
    XfdSlave *loadDecruncher(CReadBuffer *pInputBuffer) const;

	void release();
	
public:
    CXfdMaster();
    ~CXfdMaster();
    
    bool isXfdSupported(CReadBuffer *pInputBuffer) const;
    
	bool decrunch(XpkProgress *pProgress);
};

#endif // XFDMASTER_H
