///////////////////////////////////
//
// CXpkLibrarian : sub-library loader&handler
// can load additional decoders as necessary,
// see xpkLibraryBase
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//


#include "XpkLibrarian.h"

//#include <QDir>

// fix missing definition
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif

#include <tchar.h>
#include <Windows.h>


QList<QString> CXpkLibrarian::availableLibraries()
{
	QList<QString> lstTypes;
	
	// enumerate existing decruncher-libraries on disk,
	// add to list each supported file/algorithm type supported
	
	return lstTypes;
}

QString CXpkLibrarian::libNameFromType(tHeaderType enType)
{
	switch (enType)
	{
	}
	return "";
}

xpkLibraryBase *CXpkLibrarian::getDecruncher(QString &szLib, QLibrary &lib)
{
	// we want module-path (where loaded) and not working directory!
	// is there any way to get it on different platforms
	// or must we assume sub-libraries will be in some global path always??

	QString szLibName;

#ifdef _WIN32
	TCHAR szModulePath[_MAX_PATH + 1];
	DWORD dwRes = ::GetModuleFileName(NULL, // TODO: get handle to this lib somewhere..
	                    (LPTSTR)&szModulePath,
	                    _MAX_PATH + 1);
	szModulePath[dwRes] = 0x000; // 

#ifdef _UNICODE
	szLibName = QString::fromWCharArray(szModulePath, dwRes);
#else
	szLibName = QString::fromLocal8Bit(szModulePath, dwRes);
#endif
	szLibName.replace('\\', "/"); // fix MSDOS pathnames if any

	int iIndex = szLibName.lastIndexOf('/');
	szLibName = szLibName.left(iIndex); // remove module (dll) name
	
#else
	// Unix-equivalent?
	// TODO: use plugins-path?
#endif
	
	if (szLibName.at(szLibName.length() -1) != '/')
	{
		szLibName += "/";
	}

#ifndef _WIN32
	// "lib" prefix on non-Win32 binaries..
	szLibName.append("lib");
#endif
	
	szLibName.append(szLib);
	//szFileName.append("xpk");
	//szFileName.append(QString::fromStdString(szType));
	// temp, use dummy for testing
	//szFileName.append("Dummy");

#ifdef AMIGAOS
	// "library" postfix on Amiga
	szLibName.append(".library");
#endif
	
#ifdef _WIN32
	// "DLL" postfix on Win32..
	szLibName.append(".dll");
#endif

	lib.setFileName(szLibName);
	if (lib.load() == false)
	{
		throw ArcException("Failed locating library", szLibName.toStdString());
	}
	
	GetXpkInstance *pGetInstance = (GetXpkInstance*)lib.resolve("GetXpkInstance");
	if (pGetInstance == nullptr)
	{
		QString szError = lib.errorString();
		throw ArcException("Failed locating symbol", szError.toStdString());
	}
	return (xpkLibraryBase*)(*pGetInstance)();

	// temp: use dummy
	//return new XpkDummy();
	
}

