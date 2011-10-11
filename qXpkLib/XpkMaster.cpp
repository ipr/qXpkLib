///////////////////////////////////
//
// Actual decoder handling:
// can load additional decoders as necessary,
// see xpkLibraryBase
//
// CXpkMaster : main decrunch handler
// CXpkLibrarian : sub-library loader&handler
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

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

std::string CXpkMaster::getCruncherType(CReadBuffer *pInputBuffer) const
{
	// simplify, use std::string and get it done
	std::string szSubType;
	
	// determine file datatype by header information
	CFileType type(pInputBuffer->GetBegin(), pInputBuffer->GetSize());
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
	else if (type.m_enFileType == HEADERTYPE_XPK_GENERIC)
	{
		// TODO: if "PWPK" then XPK-contained PowerPacker crunching?
		//
		//szSubType.assign(m_InputBuffer.GetAt(8), 4);
	}
	else if (type.m_enFileType == HEADERTYPE_XFD_GENERIC)
	{
		// XFD-packed file ("XFDD" or "XFDF")
	}
	else if (type.m_enFileType == HEADERTYPE_XPK_SQSH
			 || type.m_enFileType == HEADERTYPE_XPK_NUKE
			 || type.m_enFileType == HEADERTYPE_XPK_RLEN)
	{
		// already detected as XPK
		// -> load sub-library (get actual type)
		//szSubType.assign(m_InputBuffer.GetAt(8), 4);
	}
	else if (type.m_enFileType == HEADERTYPE_GZIP)
	{
		// load sub-library for handling GZIP
		szSubType = "GZIP";
	}
	else if (type.m_enFileType == HEADERTYPE_AMIGAOS)
	{
		// AmigaOS executable (M68k),
		// may be self-extracting application..
		// -> needs more extensive checking
		// if there is "decrunchable" data after loader
		// or if it needs to be run in emulator or natively..
	}
	else if (type.m_enFileType == HEADERTYPE_UNIXELF)
	{
		// Unix-style ELF-binary,
		// PPC Amiga or any Unix-variant or Linux?
		// may be self-extracting application
		// -> needs more extensive checking
		// if there is "decrunchable" data after loader
		// or if it needs to be run in emulator or natively..
	}
	else
	{
		// others?
		// -> should be as sub-libraries..
		
		// detect some more types: files which XFD supports
		// but have "alien" fileformat..
		if (m_pXfdMaster->isXfdSupported(pInputBuffer) == true)
		{
			// handle as "XFD-GENERIC" type..
		}
		
	}
	
	
	return szSubType;
}

void CXpkMaster::PrepareUnpacker(std::string &subType)
{
	if (subType.length() < 4)
	{
		// should throw exception, for testing just skip
		return;
	}
	
	// load suitable sub-library?
	m_pSubLibrary = CXpkLibrarian::getDecruncher(subType, m_SubLib);
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		throw ArcException("Unsupported cruncher type", subType);
	}
}

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
	
	XpkChunk *pChunk = m_Tags.getFirst();
	while (pChunk != nullptr)
	{
		// locate data of chunk
		m_InputBuffer.SetCurrentPos(pChunk->m_nDataOffset);
		
		// decrunch chunk into output buffer
		//m_pSubLibrary->Decrunch();

		// next chunk to process..	
		pChunk = pChunk->m_pNext;
	}
	
	/* not this far yet..
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
	*/
	
	return false;
}


///////// public

CXpkMaster::CXpkMaster(QObject *parent)
	: QObject(parent)
    , m_SubLib(parent)
    , m_pXfdMaster(nullptr)
    , m_InputName()
    , m_nInputFileSize(0)
    , m_InputBuffer(1024)
    , m_Output()
    , m_pSubLibrary(nullptr)
{
	// temp, check handling later..
	m_pXfdMaster = new CXfdMaster();
}

CXpkMaster::~CXpkMaster(void)
{
	if (m_pXfdMaster != nullptr)
	{
		delete m_pXfdMaster;
		m_pXfdMaster = nullptr;
	}
}

bool CXpkMaster::xpkInfo(QXpkLib::CXpkFileInfo &info)
{
	// TODO: get info to caller..
	// - compressor (SQSH, NUKE... PP20, IMPL..)
	// - unpacked size -> need sub-library to determine
	// - what else?

	return false;
}

bool CXpkMaster::xpkUnpack(XpkProgress *pProgress)
{
	CAnsiFile InFile;
	if (InFile.Open(m_InputName.toStdString()) == false)
	{
		throw ArcException("Failed to open input", m_InputName.toStdString());
	}

	// fuck it.. read it all at once to stop headaches..
	// other problems to solve anyway
	m_nInputFileSize = InFile.GetSize();
	m_InputBuffer.PrepareBuffer(InFile.GetSize(), false);
	if (InFile.Read(m_InputBuffer.GetBegin(), InFile.GetSize()) == false)
	{
		throw IOException("Failed reading file data");
	}
	m_InputBuffer.SetCurrentPos(InFile.GetSize()); // info to decruncher
	InFile.Close(); // not needed any more
	
	// determine file type from header, try to load decruncher for it:
	// throws exception on failure
	PrepareUnpacker(getCruncherType(&m_InputBuffer));

	// TODO: if XFD-cruncher is needed, switch handling to suitable class..
	
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

