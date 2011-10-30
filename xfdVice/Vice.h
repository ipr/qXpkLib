//////////////////////////////////
//
// just interface for Vice-decruncher
//
// Ilkka Prusi <ilkka.prusi@gmail.com>
//

#ifndef VICE_H
#define VICE_H

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

// from master-project for buffer-class
#include "AnsiFile.h"

// for helper from master-library..
#include "XfdSlave.h"


class Vice : public XfdSlave
{
public:
	Vice(CReadBuffer *pIn)
		: XfdSlave(pIn)
	{}
    virtual bool decrunch(CReadBuffer *pOut);
};

#endif // VICE_H

