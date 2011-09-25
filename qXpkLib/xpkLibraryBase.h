///////////////////////////////////
//
// Pure virtual interface-class
// for each decoder-library.
//


#ifndef XPKLIBRARYBASE_H
#define XPKLIBRARYBASE_H

#include "XpkProgress.h"
#include "XpkTags.h"

#include "AnsiFile.h"

//class CReadBuffer;
//struct XpkProgress;
//class XpkTags;


class xpkLibraryBase
{
private:

	// XPK nodelist if wanted/needed by decoder:
	// can remain null if not used by inherited
	// (if XPK type file-format).
	//
	//XpkTags *m_pTagList;
	
protected:
	// only inherited can be made instance of
	xpkLibraryBase(void) 
	    //: m_pTagList(nullptr)
	{}
	virtual ~xpkLibraryBase(void)
	{
		/*
		if (m_pTagList != nullptr)
		{
			delete m_pTagList;
		}
		*/
	}
	
	// done in XpkTags in master, decide what to do..
	void ParseToNodeList(CReadBuffer &Buffer)
	{
		/*
		if (m_pTagList != nullptr)
		{
			if (m_pTagList->IsXpkFile(Buffer) == false)
			{
				return;
			}
			m_pTagList->ParseToNodeList(Buffer);
		}
		*/
	}
	
	
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

	// pack/compress
	virtual bool Crunch(XpkProgress *pProgress)=0;
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress)=0;
	
	
};


#endif // XPKLIBRARYBASE_H
