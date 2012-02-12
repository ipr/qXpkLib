#ifndef RDCN_H
#define RDCN_H

// use standard typedefs
#include <stdint.h>

// from master-project for buffer-class
#include "AnsiFile.h"

// instead of asm-to-c++ library code,
// change original c code to library-suitable
// and skip the asm library code entirely

/*
class CRDCN : public XfdSlave
{
protected:
	// satisfy pure virtual method, use different in our case..
	// (chunk-based handling instead of whole file)
	virtual bool decrunch(CReadBuffer *pOut)
	{
		return false;
	}

public:
	CRDCN(void)
		: XfdSlave(nullptr)
	{}

	// in&out sizes already known by metadata in file
	// (by XPK-master library)
	virtual bool decrunch(CReadBuffer *pIn, CReadBuffer *pOut,
				const uint32_t chunkIn, const uint32_t chunkOut);
};
*/

class CRDCN
{
public:
	CRDCN(void)
	{}

	virtual bool decrunch(CReadBuffer *pIn, CReadBuffer *pOut,
				const uint32_t chunkIn, uint32_t &chunkOut);
};

#endif // RDCN_H
