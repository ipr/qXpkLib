#include "xpkNONE.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// (see header)
xpkNONE g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
	return &g_Instance;
}


xpkNONE::xpkNONE()
 : xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "NONE";
	m_XpkCaps.m_LibDescription = "XPK (NONE) no-compression implementation";
}

xpkNONE::~xpkNONE()
{
}

bool xpkNONE::Crunch(XpkProgress *pProgress)
{
	// copy input to output and set status
	unsigned char *pSource = pProgress->pInputBuffer->GetBegin();
	
	// append after existing data, keep old,
	// grow buffer if necessary
	//
	pProgress->pOutputBuffer->Append(pSource, pProgress->xp_chunkIn);
	
	return true;
}

// this is dummy: output is same as input
bool xpkNONE::Decrunch(XpkProgress *pProgress)
{
	// copy input to output and set status
	unsigned char *pSource = pProgress->pInputBuffer->GetBegin();
	
	// append after existing data, keep old,
	// grow buffer if necessary
	//
	pProgress->pOutputBuffer->Append(pSource, pProgress->xp_chunkIn);

	return true;
}

