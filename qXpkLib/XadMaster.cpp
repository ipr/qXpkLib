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

//////// protected methods

// destroy decruncher when necessary
void CXadMaster::release()
{
	if (m_pSubLibrary != nullptr)
	{
		// only close for now,
		// expecting static-instance from sub-lib now..
		// change later
		//delete m_pSubLibrary;
		m_pSubLibrary = nullptr;
	}
}

////////// public methods

CXadMaster::CXadMaster(QObject *parent)
	: QObject(parent)
    , m_SubLib(parent)
	, m_pSubLibrary(nullptr)
{
}

CXadMaster::~CXadMaster(void)
{
	release();
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
	}
	else if (type.m_enFileType == HEADERTYPE_LZX)
	{
		szSubType = "xadLZX";
	}
	else if (type.m_enFileType == HEADERTYPE_ZOO)
	{
		szSubType = "xadZOO";
	}
	else if (type.m_enFileType == HEADERTYPE_HA)
	{
		szSubType = "xadHA";
	}
	else if (type.m_enFileType == HEADERTYPE_ACE)
	{
		szSubType = "xadACE";
	}
	else if (type.m_enFileType == HEADERTYPE_ARJ)
	{
		szSubType = "xadARJ";
	}
	/*
	else if (type.m_enFileType == HEADERTYPE_ARC)
	{
		szSubType = "xadARC";
	}
	
	// add other decrunchers:
	// TAR
	// SHAR, CPIO ?
	// ACE, ARC, ARJ
	// ZIP, RAR
	// StuffIt, PackIt
	// MSCAB
	
	*/
	
	
	// TODO: also try loading sub-library 
	// before we know that we can unpack it?
	
	
	if (subType.length() == 0)
	{
		// should throw exception, for testing just skip
		return false;
	}

	// don't delete since libraries now give "static" instance,
	// change later, for now leave "clean" in case of error..
	if (m_pSubLibrary != nullptr)
	{
		m_pSubLibrary = nullptr;
	}
	
	// load suitable sub-library?
	m_pSubLibrary = CXpkLibrarian::getXadInstance(QString::fromStdString(subType), m_SubLib);
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		//throw ArcException("Unsupported archive type", subType);
		return false; // don't throw here, trying to determine if supported..
	}
	//return m_pSubLibrary->isSupported(pInputBuffer);
	return true;
}

bool CXadMaster::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	return m_pSubLibrary->archiveInfo(info);
}

void CXadMaster::setExtractPath(QString &szPath)
{
	m_pSubLibrary->setExtractPath(szPath);
}

bool CXadMaster::decrunch(XpkProgress *pProgress)
{
	return m_pSubLibrary->Decrunch(pProgress);
}

