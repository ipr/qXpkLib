///////////////////////////////////
//
// XAD-library support:
// should load additional decoders as necessary.
//
// Support multi-file archives unpacking via loadable libraries.
//
// May be called from XpkMaster when needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//


#ifndef XADMASTER_H
#define XADMASTER_H

#include <QString>

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

#include "AnsiFile.h"

#include "XpkProgress.h"
#include "xadLibraryBase.h"


class CXadMaster
{
protected:

	xadLibraryBase *m_pSubLibrary;
	
	//QString m_archive;
	
public:
    CXadMaster();
    ~CXadMaster();
    
    bool isSupported(CReadBuffer *pInputBuffer);
    
	//void setArchive(QString &szArchive);
	void setExtractPath(QString &szPath);
	bool extractArchive(XpkProgress *pProgress);
};

#endif // XADMASTER_H
