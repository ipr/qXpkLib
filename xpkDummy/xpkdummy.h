#ifndef XPKDUMMY_H
#define XPKDUMMY_H

#include "xpkDummy_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKDUMMYSHARED_EXPORT xpkDummy : public xpkLibraryBase
{
public:
	xpkDummy();
	virtual ~xpkDummy();
	
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

#endif // XPKDUMMY_H
