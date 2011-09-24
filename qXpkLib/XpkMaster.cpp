#include "XpkMaster.h"

#include <exception>
#include <string>

// reuse type detection,
// multiple variations etc.
//
#include "FileType.h"


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

xpkLibraryBase *CXpkLibrarian::getDecruncher(std::string &szType, QLibrary &lib)
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
	
	szFileName.append("xpk");
	szFileName.append(QString::fromStdString(szType));
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



///////// protected methods

std::string CXpkMaster::getCruncherType()
{
	// simplify, use std::string and get it done
	std::string szSubType;
	
	// determine file datatype by header information
	CFileType type(m_InputBuffer.GetBegin(), m_InputBuffer.GetSize());
	if (type.m_enFileType == HEADERTYPE_PP20)
	{
		// Amiga PowerPacker:
		// not XPK-file but we may have support for it..
		szSubType = "PP20";
	}
	else if (type.m_enFileType == HEADERTYPE_IMPLODER)
	{
		// Amiga Imploder:
		// multiple identifiers (clones, variations)
		// but can use same unpacking..
		szSubType = "IMPL";
	}
	else if (type.m_enFileType == HEADERTYPE_XPK_SQSH
			 || type.m_enFileType == HEADERTYPE_XPK_NUKE
			 || type.m_enFileType == HEADERTYPE_XPK_RLEN)
	{
		// already detected as XPK
		// -> load sub-library (get actual type)
		szSubType.assign(m_InputBuffer.GetAt(8), 4);
	}
	else if (type.m_enFileType == HEADERTYPE_GZIP)
	{
		// load sub-library for handling GZIP
		szSubType = "GZIP";
	}
	else
	{
		// others?
		// -> should be as sub-libraries..
	}
	return szSubType;
}

void CXpkMaster::PrepareUnpacker(std::string &subType)
{
	if (szSubType.length() > 0)
	{
		// load suitable sub-library?
		m_pSubLibrary = CXpkLibrarian::getDecruncher(szSubType, m_SubLib);
		if (m_pSubLibrary == nullptr)
		{
			// not supported/can't load -> can't decrunch it
			throw ArcException("Unsupported cruncher type", szSubType);
		}
	}
}

/*
void CXpkMaster::PreparePacker()
{
	// user-given packer-type needed?
}
*/

bool CXpkMaster::ForeignDecrunch(XpkProgress *pProgress)
{
	// we depend entirely on sub-library to do decrunching
	// -> pass entire file to sub-library
	
	return m_pSubLibrary->Decrunch(pProgress);
}

bool CXpkMaster::OwnDecrunch(XpkProgress *pProgress)
{
	// XPK-container format in file:
	// we need to process XPK-tags in file
	// and pass chunks to sub-library for decrunching
	// -> not done yet..
	// temp, testing
	
	m_Tags.ParseToNodeList(m_InputBuffer);
	
	bool bRet = true;
	while (pProgress->xp_PackedProcessed < m_nInputFileSize
	       && bRet == true)
	{
		
		// temp, testing
		pProgress->pOutputBuffer->Append(pProgress->pInputBuffer->GetBegin(), pProgress->pInputBuffer->GetCurrentPos());
		pProgress->xp_PackedProcessed = m_nInputFileSize;
		
		// since we have all in buffer already, just update position if necessary
		size_t nPos = pProgress->pInputBuffer->GetCurrentPos();
		if (nPos < m_nInputFileSize)
		{
			pProgress->pInputBuffer->SetCurrentPos(pProgress->xp_PackedProcessed);
		}
		
	}
	
	return bRet;
}


///////// public

CXpkMaster::CXpkMaster(QObject *parent)
	: QObject(parent)
    , m_SubLib(parent)
    , m_InputName()
    , m_nInputFileSize(0)
    , m_InputBuffer(1024)
    , m_Output()
    , m_pSubLibrary(nullptr)
{
}

CXpkMaster::~CXpkMaster(void)
{
}

bool CXpkMaster::xpkInfo(QXpkLib::CXpkFileInfo &info)
{
	// TODO: get info to caller..
	// - compressor (SQSH, NUKE... PP20, IMPL..)
	// - unpacked size -> need sub-library to determine
	// - what else?

	return false;
}

/*
bool CXpkMaster::xpkPack(XpkProgress *pProgress)
{
	PreparePacker();
	  
	CAnsiFile InFile;
	if (InFile.Open(m_InputName.toStdString()) == false)
	{
		throw ArcException("Failed to open", m_InputName.toStdString());
	}
	m_nInputFileSize = InFile.GetSize();
	
	//m_pSubLibrary->crunch(m_InputBuffer.GetBegin());
	
	return false;
}
 */

bool CXpkMaster::xpkUnpack(XpkProgress *pProgress)
{
	CAnsiFile InFile;
	if (InFile.Open(m_InputName.toStdString()) == false)
	{
		throw ArcException("Failed to open input", m_InputName.toStdString());
	}
	
	m_nInputFileSize = InFile.GetSize();
	if (m_nInputFileSize < 16)
	{
		// can't determine file type for unpacking
		// without proper header
		throw ArcException("File too small", m_nInputFileSize);
	}
	
	if (InFile.Read(m_InputBuffer.GetBegin(), 16) == false)
	{
		throw ArcException("Failed reading header", 16);
	}
	
	// determine file type from header, try to load decruncher for it:
	// throws exception on failure
	PrepareUnpacker(getCruncherType());

	// TODO: do we keep entire file in buffer when unpacking in chunks..
	//
	// this is simplest..
	// read rest of it anyway.. prepare buffer (grow)
	m_InputBuffer.PrepareBuffer(m_nInputFileSize, true);
	if (InFile.Read(m_InputBuffer.GetAt(16), m_nInputFileSize - 16) == false)
	{
		throw ArcException("Failed reading file", (m_nInputFileSize - 16));
	}
	m_InputBuffer.SetCurrentPos(m_nInputFileSize); // info to decruncher
	InFile.Close(); // not needed any more
	
	// setup info for decrunch
	pProgress->pInputBuffer = &m_InputBuffer;
	pProgress->pOutputBuffer = m_Output.GetBuffer();
	
	// just decrunch all at once, write file when done
	bool bRet = true;
	if (m_Tags.IsXpkFile(m_InputBuffer) == true)
	{
		// XPK-container, process into tags
		// and chunk-nodes
		bRet = OwnDecrunch(pProgress);
	}
	else
	{
		// something else, 
		// sub-library should know what do with it..
		bRet = ForeignDecrunch(pProgress);
	}
	
	if (bRet == false)
	{
		throw ArcException("Decrunching failed", m_InputName.toStdString());
	}
	
	// write all at once when done..
	if (m_Output.getName().length() == 0)
	{
		// no output-file -> done
		// (user wants buffer-only?)
		return true;
	}
	
	// overwrite existing file?
	if (m_InputName == m_Output.getName())
	{
		InFile.Close();
	}
	m_Output.WriteFile();
		
	return true;
}

