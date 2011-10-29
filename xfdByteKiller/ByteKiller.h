//////////////////////////////////
//
// just interfaces for various decrunchers
// for ByteKiller-variants:
// some are combined, others are separate.
//
// Ilkka Prusi <ilkka.prusi@gmail.com>
//

#ifndef BYTEKILLER_H
#define BYTEKILLER_H

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// for helper from master-library..
#include "XfdSlave.h"


///// base for ByteKiller variants

class ByteKiller : public XfdSlave
{
protected:
	uint32_t m_tag;
	
	// shared for all instances
	void Eoruj();
	void D_CRUN();
	
public:
	ByteKiller(CReadBuffer *pIn)
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
	
	virtual bool isSupported() const = 0;
};

///// implementations

class crun : public ByteKiller
{
public:
	crun(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}
	
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("CRUN"))
		{
			return true;
		}
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};

class crnd : public ByteKiller
{
public:
	crnd(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}
	
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("CRND"))
		{
			return true;
		}
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};

class marc : public ByteKiller
{
public:
	marc(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}
	
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("MARC")
			|| m_tag == MakeTag("TMB!")
			|| m_tag == MakeTag("TXIC")
			|| m_tag == MakeTag("SCC!"))
		{
			return true;
		}
		
		if (m_tag == MakeTag("ons ")
			|| m_tag == MakeTag(" Sym")
			|| m_tag == MakeTag("Gary"))
		{
			// non-supported variants of marc() ?
			return false;
		}
		
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};

class xvdg : public ByteKiller
{
public:
	xvdg(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}
	
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("xVdg"))
		{
			return true;
		}
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};

/*
class amos : public ByteKiller
{
public:
	amos(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}

	// compiled AMOS-program,
	// starts with normal executable hunk
	//
	virtual bool isSupported() const
	{
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};
*/

class arpf : public ByteKiller
{
public:
	arpf(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}
	
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("ARPF"))
		{
			return true;
		}
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};

class arp3 : public ByteKiller
{
public:
	arp3(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}
	
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("ARP3"))
		{
			return true;
		}
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};

class ace : public ByteKiller
{
public:
	ace(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}
	
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("ACE!"))
		{
			return true;
		}
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};

class pack : public ByteKiller
{
public:
	pack(CReadBuffer *pIn)
		: ByteKiller(pIn)
	{}
	
	virtual bool isSupported() const
	{
		if (m_tag == MakeTag("PACK"))
		{
			return true;
		}
		return false;
	}
	
    virtual bool decrunch(CReadBuffer *pOut);
};


#endif // BYTEKILLER_H
