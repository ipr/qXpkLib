#include "qxpklib.h"

#include "XpkMaster.h"


//////////////// protected methods

void QXpkLib::PrepareMaster()
{
	// clear old instance and counters etc.
	if (m_pMaster != nullptr)
	{
		delete m_pMaster;
	}
	if (m_pProgress != nullptr)
	{
		delete m_pProgress;
	}
	
	m_pMaster = new CXpkMaster(this);
	m_pProgress = new XpkProgress();

	// connect emitted signals from master-handler
	// to output from this library
	connect(m_pMaster, SIGNAL(message(QString)), this, SIGNAL(message(QString)));
	connect(m_pMaster, SIGNAL(warning(QString)), this, SIGNAL(warning(QString)));
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

// pack/unpack to/from given input&output
bool QXpkLib::xpkPack()
{
	try
	{
		return m_pMaster->xpkPack(m_pProgress);
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
		return m_pMaster->xpkUnpack(m_pProgress);
	}
	catch (std::exception &exp)
	{
		// output error to user
		emit error(exp.what());
	}
	return false;
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

// if user wants some buffer-to-buffer handling?
void QXpkLib::setInputBuffer(QByteArray *pArray)
{
}

void QXpkLib::setOutputBuffer(QByteArray *pArray)
{
}

