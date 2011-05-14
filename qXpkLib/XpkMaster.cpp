#include "XpkMaster.h"

#include <exception>

#include <QDir>
#include <QLibrary>


static QList<QString> CXpkLibrarian::availableLibraries()
{
	QList<QString> lstTypes;
	
	// enumerate existing decruncher-libraries on disk,
	// add to list each supported file/algorithm type supported
	
	return lstTypes;
}

static xpkLibraryBase *CXpkLibrarian::getDecruncher(QString szType)
{
	// load library of given type
	QString szFileName = QDir::currentPath();
	szFileName.replace('\\', "/"); // fix MSDOS pathnames if any
	if (szFileName.at(szFileName.length() -1) != '/')
	{
		szFileName += "/";
	}
	// expected name of DLL..
	//szFileName.append("xpk").append(szType).append(".dll");
	// temp: use dummy
	szFileName += "xpkDummy.dll"; 

	/* TODO: finish handling
	QLibrary lib(szFileName);
	if (lib.load() == false)
	{
		return nullptr;
	}
	*/

	/*
	// or something simpler..? -> see above
	if (szType == "NUKE")
	{
		//return new xpkNUKE();
	}
	else if (szType == "SQSH")
	{
		//return new xpkSQSH();
	}
	else if (szType == "PP20")
	{
		//return new xpkPowerPacker();
	}
	else if (szType == "GZIP")
	{
		//return new xpkGzip();
	}
	*/
	
	// temp: use dummy
	//return new XpkDummy();
	
	return nullptr;
}



///////// protected methods

void CXpkMaster::PrepareUnpacker()
{
	// determine file type from header:
	// do we have decrunching-support for it?
	
	//QList<QString> lstLibs = CXpkLibrarian::availableLibraries();
	
	QString szSubType;
	
	unsigned char *pBuf = m_InputBuffer.GetBegin();
	if (::memcmp(pBuf, "XPKF", 4) == 0)
	{
		// XPK -> sub-library type needed
		//
		szSubType = QString::fromAscii(m_InputBuffer.GetAt(8), 4);
		
	}
	else if (::memcmp(pbuf, "PP20", 4) == 0)
	{
		// Amiga PowerPacker:
		// not XPK-file but we may have support for it..
		//
		szSubType = "PP20";
	}
	else
	{
		// detect GZIP ?
	}
	
	// load suitable sub-library?
	
	m_pSubLibrary = CXpkLibrarian::getDecruncher(szSubType);
	if (m_pSubLibrary != nullptr)
	{
		return true;
	}
	
	// otherwise not supported, can't decrunch it
	throw ArcException("Unsupprted cruncher", szSubType);
	
	//return false;
}

void CXpkMaster::PreparePacker()
{
	// user-given packer-type needed?
}


///////// public

CXpkMaster::CXpkMaster(QObject *parent)
	: QObject(parent)
    , m_nInputFileSize(0)
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

