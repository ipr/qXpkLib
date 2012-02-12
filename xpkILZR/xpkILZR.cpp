#include "xpkILZR.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"


// (see header)
xpkILZR g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xpkILZR::xpkILZR()
	: xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "ILZR";
	m_XpkCaps.m_LibDescription = ".. implementation";
}

xpkILZR::~xpkILZR()
{
}

bool xpkILZR::Decrunch(XpkProgress *pProgress)
{
	//CILZR decr; // pre-chunk decrunching -> no need to keep as member

	// we assume master-library allocated suitable buffer
	// for unpacking chunk (known by XPK metadata)
	return decr.decrunch(pProgress->pInputBuffer, pProgress->pOutputBuffer, pProgress->xp_chunkIn, pProgress->xp_chunkOut);
}

