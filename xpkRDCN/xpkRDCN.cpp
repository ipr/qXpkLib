#include "xpkRDCN.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

#include "RDCN.h"

// (see header)
xpkRDCN g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xpkRDCN::xpkRDCN()
	: xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "RDCN";
	m_XpkCaps.m_LibDescription = ".. implementation";
}

xpkRDCN::~xpkRDCN()
{
}

bool xpkRDCN::Decrunch(XpkProgress *pProgress)
{
	CRDCN decr; // pre-chunk decrunching -> no need to keep as member

	// we assume master-library allocated suitable buffer
	// for unpacking chunk (known by XPK metadata)
	return decr.decrunch(pProgress->pInputBuffer, pProgress->pOutputBuffer, pProgress->xp_chunkIn, pProgress->xp_chunkOut);
}

