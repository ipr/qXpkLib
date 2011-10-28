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
	if (m_pXadMaster->isSupported(m_Input.GetBuffer(), type) == true)
	{
		bSupported = m_pXadMaster->archiveInfo(info);
	}
	else if (m_pXfdMaster->isSupported(m_Input.GetBuffer(), type) == true)
	{
		bSupported = m_pXfdMaster->archiveInfo(info);
	}
	else if (m_pXpkMaster->isSupported(m_Input.GetBuffer(), type) == true)
	{
		bSupported = m_pXpkMaster->archiveInfo(info);
	}

	return bSupported;
}

// Note: will need additional handling 
// in case of multi-volume archives (archive spanning/split into multiple files),
// at least need some pattern matching of which files belong to same volume..?
//
// At least RAR and ACE support multi-volume archives?
//
bool CLibMaster::archiveUnpack(XpkProgress *pProgress)
{
	// just read it all to buffer, change later..
	m_Input.Read();

	m_nInputFileSize = m_Input.GetFile()->GetSize();
	pProgress->xp_WholePackedFileSize = m_Input.GetFile()->GetSize(); // info to decruncher
	
	// try determine file datatype by header information
	CFileType type(m_Input.GetBuffer()->GetBegin(), m_Input.GetBuffer()->GetSize());

	// setup info for decrunch
	pProgress->pInputBuffer = m_Input.GetBuffer();
	pProgress->pOutputBuffer = m_Output.GetBuffer();
	
	// just decrunch all at once, write file when done
	bool bRet = false;
	if (m_pXadMaster->isSupported(m_Input.GetBuffer(), type) == true)
	{
		// in this case, library should manage loading of data
		// per each file-entry being decrunched (possibly many)
		// and format is "alien" (only library might know..)
	
		m_pXadMaster->setExtractPath(m_outputPath);
		bRet = m_pXadMaster->decrunch(pProgress);
	}
	else if (m_pXfdMaster->isSupported(m_Input.GetBuffer(), type) == true)
	{
		// in this case, we need to load whole file before decrunching
		// as format is "alien" (only cruncher might know..)
	
		bRet = m_pXfdMaster->decrunch(pProgress);
	}
	else if (m_pXpkMaster->isSupported(m_Input.GetBuffer(), type) == true)
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
	m_Input.Read(1024);
	
	// just read it all to buffer, change later if wanted..
	m_nInputFileSize = m_Input.GetFile()->GetSize();
	
	// try determine file datatype by header information
	CFileType type(m_Input.GetBuffer()->GetBegin(), nReadSize);
	
	// TODO : keep info on which handling is needed
	// for later
	
	bool bIsSupported = false;
	if (m_pXadMaster->isSupported(m_Input.GetBuffer(), type) == true)
	{
		bIsSupported = true;
	}
	else if (m_pXfdMaster->isSupported(m_Input.GetBuffer(), type) == true)
	{
		bIsSupported = true;
	}
	else if (m_pXpkMaster->isSupported(m_Input.GetBuffer(), type) == true)
	{
		bIsSupported = true;
	}
	return bIsSupported;
}
