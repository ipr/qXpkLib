///////////////////////////////////
//
// CXpkLibrarian : sub-library loader&handler
// can load additional decoders as necessary,
// see related sub-library base for interface:
// - xpkLibraryBase, chunk-based XPK-format
// - xfdLibraryBase, single file decrunching
// - xadLibraryBase, archive unpacking
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "XpkLibrarian.h"

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
	
	// we want module-path (where master-library was loaded) 
	// and NOT working directory of application:
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
#endif // _WIN32
	
#ifdef UNIX
	// Unix-equivalent?
	// TODO: use plugins-path?
#endif

#ifdef AMIGAOS
	// global "LIBS:" with sub-directory for sub-libraries,
	// at least used to be..
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

bool CXpkLibrarian::loadLib(QString &szLib)
{
	if (m_SubLib.isLoaded() == true)
	{
		m_SubLib.unload();
	}

	m_libName = getLibName(szLib, getLibPath());
	m_SubLib.setFileName(m_libName);
	if (m_SubLib.load() == false)
	{
		throw ArcException("Failed locating library", m_libName.toStdString());
	}
	return true;
}

////////////// public methods

CXpkLibrarian::CXpkLibrarian(QObject *parent)
 : QObject(parent)
 , m_SubLib(parent)
 , m_libName()
{
}

CXpkLibrarian::~CXpkLibrarian()
{
	m_SubLib.unload();
}

/*
// TODO: enumeration of available sub-libraries
QList<QString> CXpkLibrarian::availableLibraries()
{
	QList<QString> lstTypes;
	
	// enumerate existing decruncher-libraries on disk,
	// add to list each supported file/algorithm type supported
	
	return lstTypes;
}
*/

///////
//
// Notes about library interface handling for "getXYZInstance" methods:
//
// * on Windows, DLL ABI has name decorations, which are a pain to deal with
//  - if "stub" linking is used it's not an issue
//  - downside of "stub" linking is that we would effectively require
//   each and every sub-library to exist when using master-library
//   -> this would be impossible to extend without dependencies on sub-libraries
//  - instead of trying to access C++ objects directly from DLL
//   we only need single method to give us that object
//   -> simple and should work just about always on other platforms also
//
// * on Linux ..
// * on Mac OS ..
//

xpkLibraryBase *CXpkLibrarian::getXpkInstance(QString &szLib)
{
	loadLib(szLib);
	
	// get function pointer by symbol for accessing object from library
	// (note type)
	GetXpkInstance *pGetInstance = (GetXpkInstance*)m_SubLib.resolve("GetXpkInstance");
	if (pGetInstance == nullptr)
	{
		QString szError = m_SubLib.errorString();
		throw ArcException("Failed locating symbol", szError.toStdString());
	}
	
	// call library method and get instance of contained object from library
	// (note type)
	return (xpkLibraryBase*)(*pGetInstance)();
}

xfdLibraryBase *CXpkLibrarian::getXfdInstance(QString &szLib)
{
	loadLib(szLib);
	
	// get function pointer by symbol for accessing object from library
	// (note type)
	GetXfdInstance *pGetInstance = (GetXfdInstance*)m_SubLib.resolve("GetXpkInstance");
	if (pGetInstance == nullptr)
	{
		QString szError = m_SubLib.errorString();
		throw ArcException("Failed locating symbol", szError.toStdString());
	}
	
	// call library method and get instance of contained object from library
	// (note type)
	return (xfdLibraryBase*)(*pGetInstance)();
}

xadLibraryBase *CXpkLibrarian::getXadInstance(QString &szLib)
{
	loadLib(szLib);
	
	// get function pointer by symbol for accessing object from library
	// (note type)
	GetXadInstance *pGetInstance = (GetXadInstance*)m_SubLib.resolve("GetXpkInstance");
	if (pGetInstance == nullptr)
	{
		QString szError = m_SubLib.errorString();
		throw ArcException("Failed locating symbol", szError.toStdString());
	}
	
	// call library method and get instance of contained object from library
	// (note type)
	return (xadLibraryBase*)(*pGetInstance)();
}

