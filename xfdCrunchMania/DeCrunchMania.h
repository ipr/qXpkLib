////////////////////////////////////////
//
// just interface for decruncher
//
// Ilkka Prusi <ilkka.prusi@gmail.com>
//

#ifndef DECRUNCHMANIA_H
#define DECRUNCHMANIA_H

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// for helper from master-library..
#include "XfdSlave.h"


// decruncher
//
class DeCrunchMania : public XfdSlave
{
protected:
	uint32_t m_tag;
	
	uint32_t m_destLen; // 
	uint32_t m_sourceLen; // 
	
	void OverlapDecrunch(CReadBuffer *pOut);
	void NormalDecrunch(CReadBuffer *pOut);
	void FastDecruncher();
	
public:
	DeCrunchMania(CReadBuffer *pIn)
		: XfdSlave(pIn)
	{
		uint8_t *pA0 = m_pIn->GetBegin(); // <- buffer address
		uint32_t size = m_pIn->GetSize(); // <- buffer length
	
		// setup here
		D0.l = size;
		A0.src = pA0+4;
		
		// current type to decrunch
		m_tag = MakeTag((char*)pA0); // reduce check later
	}

	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("CrM!"))
		{
			return true;
		}
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};

#endif // DECRUNCHMANIA_H
