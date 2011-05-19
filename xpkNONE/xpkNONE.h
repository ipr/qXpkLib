#ifndef XPKNONE_H
#define XPKNONE_H

#include "xpkNONE_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKNONESHARED_EXPORT xpkNONE : public xpkLibraryBase
{
public:
	xpkNONE();
	virtual ~xpkNONE();
	
	// temp
	/*
	typedef struct XpkMode
	{} XpkMode;
	virtual XpkMode *GetModes()
	{
		return XpkMode;
	}
	
	typedef struct XpkInfo
	{} XpkInfo;
	virtual XpkInfo *GetInfo()
	{
		return XpkInfo;
	}
	*/
	
	//virtual QString GetPackerName()=0;
	//virtual QString GetPackerLongName()=0;

	virtual bool Decrunch(XpkProgress *pProgress);
};

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKNONESHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#endif // XPKNONE_H
