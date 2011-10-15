///////////////////////////////////
//
// QXpkLib:
// just the library entry-point/interface 
// as Qt-object for loading into an application.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "qxpklib.h"

#include "AnsiFile.h"
#include "XpkMaster.h"

//////////////// protected methods

void QXpkLib::PrepareMaster()
{
	// clear old instance and counters etc.
	if (m_pMaster != nullptr)
	{
		delete m_pMaster;
	}
	
	m_pMaster = new CXpkMaster(this);

	// connect emitted signals from master-handler
	// to output from this library
	connect(m_pMaster, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
	connect(m_pMaster, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
	
	// master-instance throws exception on error -> no need for this
	//connect(m_pMaster, SIGNAL(error(QString)), this, SIGNAL(error(QString)));
}


//////////////// public

QXpkLib::QXpkLib(QObject *parent)
	: QObject(parent)
	, m_pMaster(nullptr)
    , m_pProgress(nullptr)
{
	// TODO: create this instance only when user starts new file:
	// would be simplest to instantiate per pack/unpack file
	//
	PrepareMaster();
}

QXpkLib::~QXpkLib()
{
	if (m_pProgress != nullptr)
	{
		delete m_pProgress;
	}
	if (m_pMaster != nullptr)
	{
		delete m_pMaster;
	}
}

// information on selected file
// (compression, type etc.)
bool QXpkLib::xpkInfo(QXpkLib::CXpkFileInfo &info)
{
	try
	{
		// get info to caller from master-instance
		return m_pMaster->xpkInfo(info);
	}
	catch (std::exception &exp)
	{
		// output error to user
		emit error(exp.what());
	}
	return false;
}

// pack/unpack to/from given input&output
bool QXpkLib::xpkPack()
{
	try
	{
		// no packing (yet)
		
		/*
		// new status-container
		if (m_pProgress != nullptr)
		{
			delete m_pProgress;
		}
		m_pProgress = new XpkProgress();
		
		return m_pMaster->xpkPack(m_pProgress);
		*/
	}
	catch (std::exception &exp)
	{
		// output error to user
		emit error(exp.what());
	}
	return false;
}

bool QXpkLib::xpkUnpack()
{
	try
	{
		// new status-container
		if (m_pProgress != nullptr)
		{
			delete m_pProgress;
		}
		m_pProgress = new XpkProgress();
		
		return m_pMaster->xpkUnpack(m_pProgress);
	}
	catch (std::exception &exp)
	{
		// output error to user
		emit error(exp.what());
	}
	return false;
}

// if user wants result to buffer?
void QXpkLib::getToBuffer(QByteArray &Array)
{
	CReadBuffer *pOut = m_pMaster->getResult();
	
	// reserve out-buffer and copy what we have
	Array.resize(pOut->GetCurrentPos());
	::memcpy(Array.data(), pOut->GetBegin(), pOut->GetCurrentPos());
	
}

//////////////// public slots

// file to process
void QXpkLib::setInputFile(QString &szFile)
{
	// temp, change to on instance creation
	m_pMaster->setInputFile(szFile);
}

// file to write
void QXpkLib::setOutputFile(QString &szFile)
{
	// temp, change to on instance creation
	m_pMaster->setOutputFile(szFile);
}

// destination path for multi-file archive extraction
void QXpkLib::setOutputPath(QString &szPath)
{
	// temp, change to on instance creation
	m_pMaster->setOutputPath(szPath);
}

