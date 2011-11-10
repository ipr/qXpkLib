//////////////////////////////////
//
// just interface for VDCO (Virtual Dreams) decruncher
//
// Ilkka Prusi <ilkka.prusi@gmail.com>
//

#ifndef VDCO_H
#define VDCO_H

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

// from master-project for buffer-class
#include "AnsiFile.h"

// for helper from master-library..
#include "XfdSlave.h"


class VDCO : public XfdSlave
{
public:
	VDCO(CReadBuffer *pIn)
		: XfdSlave(pIn)
	{}
    virtual bool decrunch(CReadBuffer *pOut);

protected:
	bool DB_VDCO(CReadBuffer *pOut);
};

#endif // VDCO_H
