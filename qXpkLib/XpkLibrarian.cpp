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
	
	TCHAR szModulePath[_MAX_PATH + 1];
	DWORD dwRes = ::GetModuleFileName(NULL, // TODO: get handle to this lib somewhere..
	                    (LPTSTR)&szModulePath,
	                    _MAX_PATH + 1);
	szModulePath[dwRes] = 0x000; // 
	
	QString szFileName;
#ifdef _UNICODE
	szFileName = QString::fromWCharArray(szModulePath, dwRes);
#else
	szFileName = QString::fromLocal8Bit(szModulePath, dwRes);
#endif

	// NOT THIS: we want dll path not working path
	//
	// load library of given type
	//QString szFileName = QDir::currentPath();
	szFileName.replace('\\', "/"); // fix MSDOS pathnames if any
	
	int iIndex = szFileName.lastIndexOf('/');
	szFileName = szFileName.left(iIndex); // remove module name
	if (szFileName.at(szFileName.length() -1) != '/')
	{
		szFileName += "/";
	}
	
	// TODO: use plugins-path?
	
	szFileName.append(szLib);
	//szFileName.append("xpk");
	//szFileName.append(QString::fromStdString(szType));
	// temp, use dummy for testing
	//szFileName.append("Dummy");
	szFileName.append(".dll");

	lib.setFileName(szFileName);
	if (lib.load() == false)
	{
		throw ArcException("Failed locating library", szFileName.toStdString());
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

