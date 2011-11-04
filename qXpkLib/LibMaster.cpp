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
    , m_pInput(nullptr)
    , m_pOutput(nullptr)
    , m_pXpkMaster(nullptr)
    , m_pXfdMaster(nullptr)
    , m_pXadMaster(nullptr)
    , m_fileType()
    , m_info()
    , m_pProgress(nullptr)
{
	// temp, check handling later..
	// create when selected archive changed?
	//
	m_pXpkMaster = new CXpkMaster(this);
	m_pXfdMaster = new CXfdMaster(this);
	m_pXadMaster = new CXadMaster(this);

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
	if (m_pInput != nullptr)
	{
		delete m_pInput;
		m_pInput = nullptr;
	}
	if (m_pOutput != nullptr)
	{
		delete m_pOutput;
		m_pOutput = nullptr;
	}

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
	if (m_pProgress != nullptr)
	{
		delete m_pProgress;
		m_pProgress = nullptr;
	}
}

bool CLibMaster::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	// try determine file datatype by header information
	// (TODO: keep somewhere to reduce repeated needs)
	m_fileType = CFileType(m_pInput->getBuffer()->GetBegin(), m_pInput->getBuffer()->GetSize());

	bool bSupported = false;
	if (m_pXadMaster->isSupported(m_pInput->getBuffer(), m_fileType) == true)
	{
		bSupported = m_pXadMaster->archiveInfo(info);
	}
	else if (m_pXfdMaster->isSupported(m_pInput->getBuffer(), m_fileType) == true)
	{
		bSupported = m_pXfdMaster->archiveInfo(info);
	}
	else if (m_pXpkMaster->isSupported(m_pInput->getBuffer(), m_fileType) == true)
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
bool CLibMaster::archiveUnpack()
{
	// overwrite existing file?
	// -> 
	if (m_pInput->getName() == m_pOutput->getName())
	{
		//m_pInput->Close();
		// error for now..
		throw ArcException("Output same as input", m_pInput->getName().toStdString());
	}

	bool bRet = false;
	if (m_pXadMaster->isSupported(m_pInput->getBuffer(), m_fileType) == true)
	{
		// in this case, library should manage loading of data
		// per each file-entry being decrunched (possibly many)
		// and format is "alien" (only library might know..)
		
		// we (currently) expect xad-libraries to handle IO,
		// change later if necessary (various issues to solve there..)
	
		m_pXadMaster->setExtractPath(m_outputPath);
		bRet = m_pXadMaster->decrunch(m_pProgress);
	}
	else if (m_pXfdMaster->isSupported(m_pInput->getBuffer(), m_fileType) == true)
	{
		// in this case, we need to load whole file before decrunching
		// as format is "alien" (only cruncher might know..)

		bRet = m_pXfdMaster->decrunch(m_pProgress);
	}
	else if (m_pXpkMaster->isSupported(m_pInput->getBuffer(), m_fileType) == true)
	{
		// this case is common XPK-style chunk-based format
		// -> we can handle loading chunks as needed

		bRet = m_pXpkMaster->decrunch(m_pProgress);
	}
	
	if (bRet == false)
	{
		throw ArcException("Decrunching failed", m_pInput->getName().toStdString());
	}
	
	// overwrite existing file?
	// -> 
	if (m_pInput->getName() == m_pOutput->getName())
	{
		// close input before writing from output..
		//m_pInput->Close();
	}
	
	return true;
}

// buffer for stuff from user of library,
// can we detect type of compression or do we need user info..?
bool CLibMaster::setInputBuffer(CIOBuffer *buffer)
{
	// TODO: buffer as input:
	//m_Input->setInput(buffer);
	if (m_pInput != nullptr)
	{
		delete m_pInput;
	}
	
	m_pInput = new CBufferIO(buffer);
	
	if (m_pProgress != nullptr)
	{
		delete m_pProgress;
	}
	// setup info for decrunch later
	m_pProgress = new XpkProgress(m_pInput);
	
	return true;
}

// input-file given: check what it is
// (for information to caller)
bool CLibMaster::setInputFile(QString &szFile)
{
	if (m_pInput != nullptr)
	{
		delete m_pInput;
	}
	m_pInput = new CMemoryMappedIO(szFile);

	if (m_pProgress != nullptr)
	{
		delete m_pProgress;
	}
	
	// setup info for decrunch later
	m_pProgress = new XpkProgress(m_pInput);

	m_pProgress->xp_WholePackedFileSize = m_pInput->getFullSize(); // info to decruncher

	// check if file is supported
	bool isSupported = archiveInfo(m_info);
	if (isSupported == false)
	{
		// cleanup?
	}
	return isSupported;
}

// TODO: check what to do with these, might not be slots..
bool CLibMaster::setOutputBuffer(CIOBuffer *buffer)
{
	if (m_pOutput != nullptr)
	{
		delete m_pOutput;
	}
	m_pOutput = new CBufferIO(buffer);
	if (m_pProgress != nullptr)
	{
		m_pProgress->pOutputIo = m_pOutput;
	}
	return true;
}

// TODO: might need access to this from sub-library
// for multi-file archives..
bool CLibMaster::setOutputFile(QString &szFile)
{
	if (m_pOutput != nullptr)
	{
		delete m_pOutput;
	}
	m_pOutput = new CBufferedFileIO(szFile);
	if (m_pProgress != nullptr)
	{
		m_pProgress->pOutputIo = m_pOutput;
	}
	return true;
}

void CLibMaster::setOutputPath(QString &szPath)
{
	m_outputPath = szPath;
	if (m_pProgress != nullptr)
	{
		m_pProgress->m_extractPath = m_pOutput;
	}
	
	// TODO: ?
	//m_pOutput = new CVolumeIO(szPath);
}
