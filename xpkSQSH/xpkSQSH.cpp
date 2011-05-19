#include "xpkSQSH.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"


// (see header)
xpkSQSH g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}


xpkSQSH::xpkSQSH()
    : xpkLibraryBase()
{
}

xpkSQSH::~xpkSQSH()
{
}

bool xpkSQSH::Crunch(XpkProgress *pProgress)
{
	return false;
}

bool xpkSQSH::Decrunch(XpkProgress *pProgress)
{
	return false;
}

