#include "xpkGZIP.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"


//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xpkGZIP g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}


xpkGZIP::xpkGZIP()
    : xpkLibraryBase()
{
}

xpkGZIP::~xpkGZIP()
{
}

bool xpkGZIP::Crunch(XpkProgress *pProgress)
{
	return false;
}

bool xpkGZIP::Decrunch(XpkProgress *pProgress)
{
	return false;
}


