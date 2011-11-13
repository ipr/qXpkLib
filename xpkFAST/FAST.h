/////////////////////////////////////////////////////////
//
// Rewrite into somewhat-C-like code
// from xpkFAST.a and decompress.a (M68k-assembler)
// as found in XPK source package.
//
// Original author unknown,
// assuming: Dirk Stöcker <stoecker@amigaworld.com>
//
// M68k asm to C-like rewrite by: Ilkka Prusi <ilkka.prusi@gmail.com>
//
// Some interesing info found on module:
// FastName:	dc.b	'fasTcinator',0
// LongName:	dc.b	'FAST alias WOOP or LZRW++',0
// Description:	dc.b	'LZ77-family, windowsize: 4095, no quotecount, 2 streams',0
//

#ifndef FAST_H
#define FAST_H

// from master-project for buffer-class
#include "AnsiFile.h"

// reuse some helper-code for ASM-to-C conversions..
// (better way..? not much help here..)
#include "XfdSlave.h"


// this somewhat differences from normal use-case of XFD:
// we are actually using per-chunk decrunching from XPK-chunks
// instead of decrunchin a whole file at once.
// this was not original intention but helps a bit (maybe..)
//
class CFAST : public XfdSlave
{
protected:
	// satisfy pure virtual method, use different in our case..
	// (chunk-based handling instead of whole file)
    virtual bool decrunch(CReadBuffer *pOut) 
    {
		return false;
	}
public:
	CFAST(void)
		: XfdSlave(nullptr)
	{}
	
	// in&out sizes already known by metadata in file
	// (by XPK-master library)
    virtual bool decrunch(CReadBuffer *pIn, CReadBuffer *pOut, 
				const uint32_t chunkIn, const uint32_t chunkOut);
};

#endif // FAST_H
