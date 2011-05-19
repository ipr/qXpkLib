#include "xpkNUKE.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

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
}

xpkNUKE::~xpkNUKE()
{
}

bool xpkNUKE::Crunch(XpkProgress *pProgress)
{
	return false;
}

bool xpkNUKE::Decrunch(XpkProgress *pProgress)
{
	return false;
}

