//////////////////////////////////
//
// just interfaces for decruncher
// for StoneCracker-versions
//
// Ilkka Prusi <ilkka.prusi@gmail.com>
//


#ifndef STONECRACKER_H
#define STONECRACKER_H

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// for helper from master-library..
#include "XfdSlave.h"


// TODO: is there need for variants..?
// multiple versions exist, is one enough to handle each..?
//
class StoneCracker : public XfdSlave
{
protected:
	uint32_t m_tag;
public:
	StoneCracker(CReadBuffer *pIn)
		: XfdSlave(pIn)
	{
		uint8_t *pA0 = m_pIn->GetBegin(); // <- buffer address
		uint32_t size = m_pIn->GetSize(); // <- buffer length
	
		// setup here
		//D0.l = size;
		//A0.src = pA0+4;
		
		// current type to decrunch
		m_tag = MakeTag((char*)pA0); // reduce check later
	}
	
	virtual bool isSupported() const = 0;
};

class Stc300 : public StoneCracker
{
public:
	Stc300(CReadBuffer *pIn)
		: StoneCracker(pIn)
	{}
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("S300"))
		{
			return true;
		}
		return false;
	}
    virtual bool decrunch(CReadBuffer *pOut);
};

class Stc404 : public StoneCracker
{
public:
	Stc404(CReadBuffer *pIn)
		: StoneCracker(pIn)
	{}
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("S404"))
		{
			return true;
		}
		return false;
	}
    virtual bool decrunch(CReadBuffer *pOut);
};


#endif // STONECRACKER_H
