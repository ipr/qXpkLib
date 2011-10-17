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

bool CXadMaster::isSupported(CReadBuffer *pInputBuffer, CFileType &type)
{
	std::string szSubType;

	// TODO: enumerate available clients to handle..

	// Note: these are all "foreign"/"alien" fileformats
	// that we are dealing with here 
	// -> no commonalities except library interface usully
	// -> each library separate..
	//
	if (type.m_enFileType == HEADERTYPE_LHA)
	{
		szSubType = "xadLha";
		return true;
	}
	else if (type.m_enFileType == HEADERTYPE_LZX)
	{
		szSubType = "xadLZX";
		return true;
	}
	else if (type.m_enFileType == HEADERTYPE_ZOO)
	{
		szSubType = "xadZOO";
		return true;
	}
	
	// TODO: also try loading sub-library 
	// before we know that we can unpack it?
	
	
	// add other decrunchers:
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

