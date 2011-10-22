#include "xfdGZIP.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"


//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xfdGZIP g_Instance;
xfdLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}


xfdGZIP::xfdGZIP()
    : xfdLibraryBase()
{
	// only to/from buffer supported here
	//m_XpkCaps.input.buffer = true;
	//m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "GZIP";
	m_XpkCaps.m_LibDescription = "GZIP uncompressing implementation";
}

xfdGZIP::~xfdGZIP()
{
}

bool xfdGZIP::Decrunch(XpkProgress *pProgress)
{
	return false;
}


