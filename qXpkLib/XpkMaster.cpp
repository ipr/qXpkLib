#include "XpkMaster.h"

#include <exception>
#include <string>

#include <QDir>


QList<QString> CXpkLibrarian::availableLibraries()
{
	QList<QString> lstTypes;
	
	// enumerate existing decruncher-libraries on disk,
	// add to list each supported file/algorithm type supported
	
	return lstTypes;
}

xpkLibraryBase *CXpkLibrarian::getDecruncher(std::string &szType, QLibrary &lib)
{
	// load library of given type
	QString szFileName = QDir::currentPath();
	szFileName.replace('\\', "/"); // fix MSDOS pathnames if any
	if (szFileName.at(szFileName.length() -1) != '/')
	{
		szFileName += "/";
	}
	
	// TODO: use plugins-path?
	
	// expected name of DLL..
	//
	std::string szCruncher = "xpk";
	//szCruncher += szType;
	// temp, use dummy for testing
	szCruncher += "Dummy";
	
	szFileName.append(QString::fromStdString(szCruncher));
	szFileName.append(".dll");
	
	
	lib.setFileName(szFileName);
	if (lib.load() == false)
	{
		return nullptr;
	}
	
	// too much fucking pain, use std::string and get it done
	/*	
	return lib.resolve(szClassName.toAscii();
	*/
	
	return (xpkLibraryBase*)lib.resolve(szCruncher.c_str());

	// temp: use dummy
	//return new XpkDummy();
	
}



///////// protected methods

void CXpkMaster::PrepareUnpacker()
{
	// determine file type from header:
	// do we have decrunching-support for it?
	
	//QList<QString> lstLibs = CXpkLibrarian::availableLibraries();
	
	//QString szSubType;
	
	// simplify, use std::string and get it done
	std::string szSubType;
	
	unsigned char *pBuf = m_InputBuffer.GetBegin();
	if (::memcmp(pBuf, "XPKF", 4) == 0)
	{
		// XPK -> sub-library type needed
		//
		//szSubType = QString::fromAscii(m_InputBuffer.GetAt(8), 4);
		char *pBuf = (char*)m_InputBuffer.GetAt(8);
		szSubType.assign(pBuf, 4);
	}
	else if (::memcmp(pBuf, "PP20", 4) == 0)
	{
		// Amiga PowerPacker:
		// not XPK-file but we may have support for it..
		//
		szSubType = "PP20";
	}
	else
	{
		// detect GZIP ? others?
	}
	
	// load suitable sub-library?
	
	m_pSubLibrary = CXpkLibrarian::getDecruncher(szSubType, m_SubLib);
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		throw ArcException("Unsupported cruncher type", szSubType);
	}
	
}

void CXpkMaster::PreparePacker()
{
	// user-given packer-type needed?
}


///////// public

CXpkMaster::CXpkMaster(QObject *parent)
	: QObject(parent)
    , m_SubLib(parent)
    , m_InputName()
    , m_nInputFileSize(0)
    , m_InputBuffer(1024)
    , m_OutputName()
    , m_OutputBuffer(2048)
    , m_pSubLibrary(nullptr)
{
}

CXpkMaster::~CXpkMaster(void)
{
}

bool CXpkMaster::xpkPack(XpkProgress *pProgress)
{
	/*
	PreparePacker();
	  
	CAnsiFile InFile;
	if (InFile.Open(m_InputName.toStdString()) == false)
	{
		throw ArcException("Failed to open", m_InputName.toStdString());
	}
	m_nInputFileSize = InFile.GetSize();
	
	//m_pSubLibrary->crunch(m_InputBuffer.GetBegin());
	*/
	
	return false;
}

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
	PrepareUnpacker();
	
	// read rest of it anyway.. prepare buffer (grow)
	m_InputBuffer.PrepareBuffer(m_nInputFileSize, true);
	if (InFile.Read(m_InputBuffer.GetAt(16), m_nInputFileSize - 16) == false)
	{
		throw ArcException("Failed reading file", (m_nInputFileSize - 16));
	}
	m_InputBuffer.SetCurrentPos(m_nInputFileSize); // info to decruncher
	
	// setup info for decrunch
	pProgress->pInputBuffer = &m_InputBuffer;
	pProgress->pOutputBuffer = &m_OutputBuffer;

	// just decrunch all at once, write file when done
	while (pProgress->xp_PackedProcessed < m_nInputFileSize)
	{
		if (m_pSubLibrary->Decrunch(pProgress) == false)
		{
			throw ArcException("Decrunching failed", "");
		}
		
		// since we have all in buffer already, just update position if necessary
		size_t nPos = pProgress->pInputBuffer->GetCurrentPos();
		if (nPos < m_nInputFileSize)
		{
			pProgress->pInputBuffer->SetCurrentPos(pProgress->xp_PackedProcessed);
		}
	}
	
	// meh.. write all at once when done..
	
	if (m_OutputName.length() == 0)
	{
		// no output-file -> done
		// (user wants buffer-only?)
		return true;
	}
	
	// overwrite existing file?
	if (m_InputName == m_OutputName)
	{
		InFile.Close();
	}
	
	CAnsiFile OutFile;
	if (OutFile.Open(m_OutputName.toStdString(), true) == false)
	{
		throw ArcException("Failed to open output", m_OutputName.toStdString());
	}
	if (OutFile.Write(m_OutputBuffer.GetBegin(), m_OutputBuffer.GetCurrentPos()) == false)
	{
		throw ArcException("Failed to write output", m_OutputName.toStdString());
	}
	if (OutFile.Flush() == false)
	{
		throw ArcException("Failed to flush output", m_OutputName.toStdString());
	}
	OutFile.Close();
		
	return true;
}

