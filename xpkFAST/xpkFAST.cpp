#include "xpkFAST.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xpkFAST g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xpkFAST::xpkFAST()
    : xpkLibraryBase()
{
}

xpkFAST::~xpkFAST()
{
}

bool xpkFAST::Crunch(XpkProgress *pProgress)
{
	return false;
}

bool xpkFAST::Decrunch(XpkProgress *pProgress)
{
	return false;
}
