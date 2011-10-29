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

#ifdef _WIN32
// fix missing definition (both needed)
#ifdef UNICODE
#ifndef _UNICODE
#define _UNICODE
#endif
#endif
// get correct types now
#include <tchar.h>
#include <Windows.h>
#endif // _WIN32


////////////// protected methods

// determine library-path for (de-)compressor libraries/plugins,
// this stuff is platform-dependant..
//
QString CXpkLibrarian::getLibPath()
{
	QString szLibPath;
	
	// we want module-path (where loaded) and not working directory,
	// is there any way to get it on different platforms
	// or must we assume sub-libraries will be in some global path always??
	
#ifdef _WIN32
	TCHAR szModulePath[_MAX_PATH + 1];
	DWORD dwRes = ::GetModuleFileName(NULL, // TODO: get handle to this lib somewhere..
	                    (LPTSTR)&szModulePath,
	                    _MAX_PATH + 1);
	szModulePath[dwRes] = 0x000; // 

#ifdef _UNICODE
	szLibPath = QString::fromWCharArray(szModulePath, dwRes);
#else
	szLibPath = QString::fromLocal8Bit(szModulePath, dwRes);
#endif
	szLibPath.replace('\\', "/"); // fix MSDOS pathnames if any

	int iIndex = szLibPath.lastIndexOf('/');
	szLibPath = szLibPath.left(iIndex); // remove module (dll) name
	
#else
	// Unix-equivalent?
	// TODO: use plugins-path?
#endif

	return szLibPath;
}

QString CXpkLibrarian::getLibName(QString &szLib, QString &szPath)
{
	QString szLibName = szPath;
	if (szLibName.at(szLibName.length() -1) != '/')
	{
		szLibName += "/";
	}

#ifdef UNIX
	// "lib" prefix on Unix-like binaries..
	szLibName.append("lib");
#endif
	
	szLibName.append(szLib);

#ifdef AMIGAOS
	// "library" postfix on Amiga
	szLibName.append(".library");
#endif
#ifdef UNIX
	// "so" postfix on unix-like systems,
	// is this needed?
	szLibName.append(".so");
#endif
#ifdef _WIN32
	// "DLL" postfix on Win32..
	szLibName.append(".dll");
#endif

	return szLibName;

}

////////////// public methods

QList<QString> CXpkLibrarian::availableLibraries()
{
	QList<QString> lstTypes;
	
	// enumerate existing decruncher-libraries on disk,
	// add to list each supported file/algorithm type supported
	
	return lstTypes;
}

xpkLibraryBase *CXpkLibrarian::getXpkInstance(QString &szLib, QLibrary &lib)
{
	QString szLibName = getLibName(szLib, getLibPath());
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
}

xfdLibraryBase *CXpkLibrarian::getXfdInstance(QString &szLib, QLibrary &lib)
{
	QString szLibName = getLibName(szLib, getLibPath());
	lib.setFileName(szLibName);
	if (lib.load() == false)
	{
		throw ArcException("Failed locating library", szLibName.toStdString());
	}
	
	GetXfdInstance *pGetInstance = (GetXfdInstance*)lib.resolve("GetXpkInstance");
	if (pGetInstance == nullptr)
	{
		QString szError = lib.errorString();
		throw ArcException("Failed locating symbol", szError.toStdString());
	}
	return (xfdLibraryBase*)(*pGetInstance)();
}

xadLibraryBase *CXpkLibrarian::getXadInstance(QString &szLib, QLibrary &lib)
{
	QString szLibName = getLibName(szLib, getLibPath());
	lib.setFileName(szLibName);
	if (lib.load() == false)
	{
		throw ArcException("Failed locating library", szLibName.toStdString());
	}
	
	GetXadInstance *pGetInstance = (GetXadInstance*)lib.resolve("GetXpkInstance");
	if (pGetInstance == nullptr)
	{
		QString szError = lib.errorString();
		throw ArcException("Failed locating symbol", szError.toStdString());
	}
	return (xadLibraryBase*)(*pGetInstance)();
}

