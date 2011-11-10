///////////////////////////
//
// xpk "NUKE" compression decruncher
//
// Based on M68k-assembler code
// by: Dirk Stöcker (SDI) <stoecker@amigaworld.com>
//
// C-like rewrite by: Ilkka Prusi <ilkka.prusi@gmail.com>
//

#ifndef NUKE_H
#define NUKE_H

#include <stdint.h>

// from master-project for buffer-class
#include "AnsiFile.h"

// reuse some helper-code for ASM-to-C conversions..
// (better way..? not much help here..)
#include "XfdSlave.h"

class CNuke : public XfdSlave
{
protected:
	// satisfy pure virtual method, use different in our case..
	// (chunk-based handling instead of whole file)
    virtual bool decrunch(CReadBuffer *pOut) 
    {
		return false;
	}

public:
	CNuke(void)
		: XfdSlave(nullptr)
	{}

	// in&out sizes already known by metadata in file
	// (by XPK-master library)
    virtual bool decrunch(CReadBuffer *pIn, CReadBuffer *pOut, 
				const uint32_t chunkIn, const uint32_t chunkOut);

protected:
	void TestCompressedF();
};

#endif // NUKE_H
