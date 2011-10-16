///////////////////////////////////
//
// XAD-library support:
// should load additional decoders as necessary.
//
// Support multi-file archives unpacking via loadable libraries.
//
// May be called from XpkMaster when needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//


#include "XadMaster.h"

#include "FileType.h"

// reuse librarian for loading decrunchers
#include "XpkLibrarian.h"


////////// public methods

CXadMaster::CXadMaster(QObject *parent)
	: QObject(parent)
	, m_pSubLibrary(nullptr)
{
}

CXadMaster::~CXadMaster(void)
{
	if (m_pSubLibrary != nullptr)
	{
		delete m_pSubLibrary;
		m_pSubLibrary = nullptr;
	}
}

bool CXadMaster::isSupported(CReadBuffer *pInputBuffer)
{
	// TODO: enumerate available clients to handle..
	
	// if lha -> qLhaLib (already done separately)
	// if lzx -> qLZXLib (already done separately)
	
	// add other decrunchers:
	// ZOO
	// TAR
	// SHAR, CPIO ?
	// ACE, ARC, ARJ
	// ZIP, RAR
	// StuffIt, PackIt
	// MSCAB

	return false;
}

void CXadMaster::setExtractPath(QString &szPath)
{
	m_pSubLibrary->setExtractPath(szPath);
}

bool CXadMaster::extractArchive(XpkProgress *pProgress)
{
	return m_pSubLibrary->Decrunch(pProgress);
}

