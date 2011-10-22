#include "xpkNUKE.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use shared helper,
// also DUKE (Delta-NUKE) wants to use same
//
#include "Nuke.h"

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xpkNUKE g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}


xpkNUKE::xpkNUKE()
    : xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "NUKE";
	m_XpkCaps.m_LibDescription = "XPK (NUKE) implementation";
}

xpkNUKE::~xpkNUKE()
{
}

bool xpkNUKE::Crunch(XpkProgress *pProgress)
{
	//Nuke();
	
	return false;
}

bool xpkNUKE::Decrunch(XpkProgress *pProgress)
{
	CNuke nuke;
	try
	{
		//nuke.decrunch();
		//UnNuke();
	}
	catch (std::exception &exp) // catch by base-type
	{
		// debug..
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}

