///////////////////////////////////
//
// Pure virtual interface-class
// for each decoder-library.
//


#ifndef XPKLIBRARYBASE_H
#define XPKLIBRARYBASE_H

//class CReadBuffer;
struct XpkProgress;

class xpkLibraryBase
{
protected:
	// only inherited can be made instance of
	xpkLibraryBase(void) 
	{}
	virtual ~xpkLibraryBase(void)
	{}
	
public:

	// temp
	/*
	typedef struct XpkMode
	{} XpkMode;
	virtual XpkMode *GetModes()=0;
	
	typedef struct XpkInfo
	{} XpkInfo;
	virtual XpkInfo *GetInfo()=0;
	*/
	
	//virtual QString GetPackerName()=0;
	//virtual QString GetPackerLongName()=0;

	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
};


#endif // XPKLIBRARYBASE_H
