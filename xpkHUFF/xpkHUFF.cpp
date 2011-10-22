#include "xpkHUFF.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xpkHUFF g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

// TODO: determine implementation,
// there's much of M68k-asm code
// which has weird license (can we re-implement?)
// and similar stuff already in many other packers..
// is there any reason to implement it ??

xpkHUFF::xpkHUFF()
    : xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "HUFF";
	m_XpkCaps.m_LibDescription = "XPK (HUFF) implementation";
}

xpkHUFF::~xpkHUFF()
{
}

bool xpkHUFF::Decrunch(XpkProgress *pProgress)
{
	return false;
}

