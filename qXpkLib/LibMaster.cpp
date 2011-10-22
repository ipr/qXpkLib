///////////////////////////////////
//
// CLibMaster : handler of various decrunchers and files,
// uses XPK/XFD/XAD masters for additional decrunchers as needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "LibMaster.h"

#include <exception>
#include <string>

#include "XpkLibrarian.h"


///////// protected methods

std::string CLibMaster::getCruncherType(CReadBuffer *pInputBuffer)
{
	// simplify, use std::string and get it done
	std::string szSubType;
	
	// try determine file datatype by header information
	CFileType type(pInputBuffer->GetBegin(), pInputBuffer->GetSize());
	if (type.m_enFileType == HEADERTYPE_POWERPACKER)
	{
		// Amiga PowerPacker:
		// not XPK-file but we support it in standalone-format also.
		szSubType = "xfdPowerPacker";
	}
	else if (type.m_enFileType == HEADERTYPE_IMPLODER)
	{
		// Amiga Imploder:
		// multiple identifiers (clones, variations)
		// but can use same unpacking..
		szSubType = "xfdImploder";
	}
	else if (type.m_enFileType == HEADERTYPE_XPK_GENERIC)
	{
		// TODO: if "PWPK" then XPK-contained PowerPacker crunching?
		// also others:
		// in xpk-dev: NUKE, DUKE, RLEN, HUFF, FAST..
		// others: SMPL, SLZ3, SHRI, PWPK, PPMQ, 
		// IDEA?, MASH, LHLB, ILZR, FRLE, FBR2, 
		// DMCB, DLTA, CBR0, BLZW, BLFH..
		//
		//szSubType.assign(m_InputBuffer.GetAt(8), 4);
		szSubType = "xpk";
		szSubType.append(m_InputBuffer.GetAt(8), 4);
	}
	else if (type.m_enFileType == HEADERTYPE_XFD_GENERIC)
	{
		// XFD-packed file ("XFDD" or "XFDF")
	}
	/* testing generic detection -> commented out
	else if (type.m_enFileType == HEADERTYPE_XPK_SQSH)
	{
		szSubType = "xpkSQSH";
	}
	else if (type.m_enFileType == HEADERTYPE_XPK_NUKE)
	{
	}
	else if (type.m_enFileType == HEADERTYPE_XPK_RLEN)
	{
		// already detected as XPK
		// -> load sub-library (get actual type)
		//szSubType.assign(m_InputBuffer.GetAt(8), 4);
	}
	*/
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
	
	return szSubType;
}

void CLibMaster::PrepareUnpacker(std::string &subType)
{
	// don't delete since libraries now give "static" instance,
	// change later, for now leave "clean" in case of error..
	if (m_pSubLibrary != nullptr)
	{
		m_pSubLibrary = nullptr;
	}

	if (subType.length() < 4)
	{
		// should throw exception, for testing just skip
		return;
	}
	
	// load suitable sub-library?
	m_pSubLibrary = CXpkLibrarian::getDecruncher(QString::fromStdString(subType), m_SubLib);
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		throw ArcException("Unsupported cruncher type", subType);
	}
}


///////// public

CLibMaster::CLibMaster(QObject *parent)
	: QObject(parent)
    , m_Input()
    , m_Output()
    , m_SubLib(parent)
    , m_pXpkMaster(nullptr)
    , m_pXfdMaster(nullptr)
    , m_pXadMaster(nullptr)
    , m_InputName()
    , m_nInputFileSize(0)
    , m_InputBuffer(1024)
{
	// temp, check handling later..
	// create when selected archive changed?
	//
	m_pXpkMaster = new CXpkMaster();
	m_pXfdMaster = new CXfdMaster();
	m_pXadMaster = new CXadMaster();

	// non-critical information via signals,
	// critical errors in exceptions (catch in library interface)
	//	
	connect(m_pXpkMaster, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
	connect(m_pXpkMaster, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
	connect(m_pXfdMaster, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
	connect(m_pXfdMaster, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
	connect(m_pXadMaster, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
	connect(m_pXadMaster, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
}

CLibMaster::~CLibMaster(void)
{
	if (m_pXadMaster != nullptr)
	{
		delete m_pXadMaster;
		m_pXadMaster = nullptr;
	}
	if (m_pXfdMaster != nullptr)
	{
		delete m_pXfdMaster;
		m_pXfdMaster = nullptr;
	}
	if (m_pXpkMaster != nullptr)
	{
		delete m_pXpkMaster;
		m_pXpkMaster = nullptr;
	}
}

bool CLibMaster::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	// TODO: get info to caller..
	// - compressor (SQSH, NUKE... PP20, IMPL..)
	// - unpacked size -> need sub-library to determine
	// - what else?

	bool bSupported = false;
	if (m_pXadMaster->isSupported(&m_InputBuffer, type) == true)
	{
		bSupported = m_pXadMaster->archiveInfo(info);
	}
	else if (m_pXfdMaster->isSupported(&m_InputBuffer, type) == true)
	{
		bSupported = m_pXfdMaster->archiveInfo(info);
	}
	else if (m_pXpkMaster->isSupported(&m_InputBuffer, type) == true)
	{
		bSupported = m_pXpkMaster->archiveInfo(info);
	}

	return bSupported;
}

bool CLibMaster::archiveUnpack(XpkProgress *pProgress)
{
	CAnsiFile InFile;
	if (InFile.Open(m_InputName.toStdString()) == false)
	{
		throw ArcException("Failed to open input", m_InputName.toStdString());
	}

	// just read it all to buffer, change later if wanted..
	m_nInputFileSize = InFile.GetSize();
	m_InputBuffer.PrepareBuffer(InFile.GetSize(), false);
	if (InFile.Read(m_InputBuffer.GetBegin(), InFile.GetSize()) == false)
	{
		throw IOException("Failed reading file data");
	}
	
	pProgress->xp_WholePackedFileSize = InFile.GetSize(); // info to decruncher
	InFile.Close(); // not needed any more
	
	// try determine file datatype by header information
	CFileType type(m_InputBuffer->GetBegin(), m_InputBuffer->GetSize());

	// setup info for decrunch
	pProgress->pInputBuffer = &m_InputBuffer;
	pProgress->pOutputBuffer = m_Output.GetBuffer();
	
	// just decrunch all at once, write file when done
	bool bRet = false;
	if (m_pXadMaster->isSupported(&m_InputBuffer, type) == true)
	{
		// in this case, library should manage loading of data
		// per each file-entry being decrunched (possibly many)
		// and format is "alien" (only library might know..)
	
		m_pXadMaster->setExtractPath(m_outputPath);
		bRet = m_pXadMaster->decrunch(pProgress);
	}
	else if (m_pXfdMaster->isSupported(&m_InputBuffer, type) == true)
	{
		// in this case, we need to load whole file before decrunching
		// as format is "alien" (only cruncher might know..)
	
		bRet = m_pXfdMaster->decrunch(pProgress);
	}
	else if (m_pXpkMaster->isSupported(&m_InputBuffer, type) == true)
	{
		// this case is common XPK-style chunk-based format
		// -> we can handle loading chunks as needed
		
		bRet = m_pXpkMaster->decrunch(pProgress);
	}
	
	if (bRet == false)
	{
		throw ArcException("Decrunching failed", m_InputName.toStdString());
	}

	// buffer-output? (no out-file given)
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
	m_Output.WriteFile(pProgress);
		
	return true;
}

// buffer for stuff from user of library,
// can we detect type of compression or do we need user info..?
bool CLibMaster::setInputBuffer(CReadBuffer *buffer)
{
	return false;
}

// input-file given: check what it is
// (for information to caller)
bool CLibMaster::setInputFile(QString &szFile)
{
	m_InputName = szFile;
	m_Input.setName(szFile);
	//m_Input.Read(
	
	CAnsiFile InFile;
	if (InFile.Open(m_InputName.toStdString()) == false)
	{
		throw ArcException("Failed to open input", m_InputName.toStdString());
	}

	// just read it all to buffer, change later if wanted..
	m_nInputFileSize = InFile.GetSize();
	size_t nReadSize = (1024 < m_nInputFileSize) ? 1024 : m_nInputFileSize;
	m_InputBuffer.PrepareBuffer(nReadSize, false);
	if (InFile.Read(m_InputBuffer.GetBegin(), nReadSize) == false)
	{
		throw IOException("Failed reading file data");
	}
	InFile.Close(); // not needed any more
	
	// try determine file datatype by header information
	CFileType type(m_InputBuffer->GetBegin(), nReadSize);
	
	// TODO : keep info on which handling is needed
	// for later
	
	bool bIsSupported = false;
	if (m_pXadMaster->isSupported(&m_InputBuffer, type) == true)
	{
		bIsSupported = true;
	}
	else if (m_pXfdMaster->isSupported(&m_InputBuffer, type) == true)
	{
		bIsSupported = true;
	}
	else if (m_pXpkMaster->isSupported(&m_InputBuffer, type) == true)
	{
		bIsSupported = true;
	}
	return bIsSupported;
}
