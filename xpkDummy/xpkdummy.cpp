#include "xpkdummy.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// (see header)
xpkDummy g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
	return &g_Instance;
}


xpkDummy::xpkDummy()
 : xpkLibraryBase()
{
}

xpkDummy::~xpkDummy()
{
}

// this is dummy: no change in buffers
bool xpkDummy::Crunch(XpkProgress *pProgress)
{
	return true;
}

// this is dummy: no change in buffers
bool xpkDummy::Decrunch(XpkProgress *pProgress)
{
	return true;
}

