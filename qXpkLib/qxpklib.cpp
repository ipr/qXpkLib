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
#include "LibMaster.h"

//////////////// protected methods

void QXpkLib::PrepareMaster()
{
	// clear old instance and counters etc.
	if (m_pMaster != nullptr)
	{
		delete m_pMaster;
	}
	
	m_pMaster = new CLibMaster(this);

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
{
	// TODO: create this instance only when user starts new file:
	// would be simplest to instantiate per pack/unpack file
	//
	PrepareMaster();
}

QXpkLib::~QXpkLib()
{
	if (m_pMaster != nullptr)
	{
		delete m_pMaster;
	}
}

// information on selected file
// (compression, type etc.)
bool QXpkLib::xpkInfo(QXpkLib::CArchiveInfo &info)
{
	try
	{
		// get info to caller from master-instance
		return m_pMaster->archiveInfo(info);
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
		return m_pMaster->xpkPack();
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
		return m_pMaster->archiveUnpack();
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

	// if so, selection needed for multi-file support..
void QXpkLib::getToBuffer(QString &entry, QByteArray &Array)
{
}

//////////////// public slots

// file to process
void QXpkLib::setInputFile(QString &szFile)
{
	// keep path, fix crap from MSDOS if that shit still exists..
	// it's not needed since 1999 for fucks sake..
	// even W2k doesn't need it any more: kill it already!
	//
	szFile.replace('\\', "/"); // fix path-separator

	m_pMaster->setInputFile(szFile);
}

// file to write
void QXpkLib::setOutputFile(QString &szFile)
{
	// keep path, fix crap from MSDOS if that shit still exists..
	// it's not needed since 1999 for fucks sake..
	// even W2k doesn't need it any more: kill it already!
	//
	szFile.replace('\\', "/"); // fix path-separator

	m_pMaster->setOutputFile(szFile);
}

// destination path for multi-file archive extraction
void QXpkLib::setOutputPath(QString &szPath)
{
	// keep path, fix crap from MSDOS if that shit still exists..
	// it's not needed since 1999 for fucks sake..
	// even W2k doesn't need it any more: kill it already!
	//
	szPath.replace('\\', "/"); // fix path-separator

	// verify path ends correctly	
	if (szPath.at(szPath.length() - 1) != '/')
	{
		// end path with separator
		szPath += "/";
	}

	// temp, change to on instance creation
	m_pMaster->setOutputPath(szPath);
}

