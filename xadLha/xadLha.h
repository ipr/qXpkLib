/* xadLHA : LHa extracting for qXpkLib
 *
 * Based on Unix LhA source code: lha-1.14i-ac20040929
 * Copyrights:
 * LHarc    for UNIX  V 1.02  Copyright(C) 1989  Y.Tagawa
 * LHx      for MSDOS V C2.01 Copyright(C) 1990  H.Yoshizaki
 * LHx(arc) for OSK   V 2.01  Modified     1990  Momozou
 * LHa      for UNIX  V 1.00  Copyright(C) 1992  Masaru Oki
 * LHa      for UNIX  V 1.14  Modified     1995  Nobutaka Watazaki
 * LHa      for UNIX  V 1.14i Modified     2000  Tsugio Okamoto
 *                    Autoconfiscated 2001-2004  Koji Arai
 * 
 * C++ library versions (qLhaLib and xadLHA) by Ilkka Prusi (2011).
 *
 */


#ifndef XADLHA_H
#define XADLHA_H

#include "xadLha_global.h"

#include <QObject>

// base for library interface
#include "xadLibraryBase.h"

#include "qxpklib.h"

// fwd. decl.
class CLhArchive;

class XADLHASHARED_EXPORT xadLha : public xadLibraryBase
{
public:
    xadLha();
    virtual ~xadLha();

    virtual bool isSupported(CIOBuffer *pInputBuffer);
	virtual bool archiveInfo(CIoContext *pInput, QXpkLib::CArchiveInfo &info);
	virtual bool testArchive(CIoContext *pInput);
	
	// unpack/decompress
	virtual bool Decrunch(XpkProgress *pProgress);
	
private:
	CLhArchive *m_pArchive;
};


#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XADLHASHARED_EXPORT xadLibraryBase *GetXpkInstance(void);


#ifdef __cplusplus
}
#endif


#endif // XADLHA_H
