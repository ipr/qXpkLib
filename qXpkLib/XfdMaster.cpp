///////////////////////////////////
//
// XFD-decrunching support:
// should load additional decoders as necessary.
//
// Support for decrunching "foreign"/"alien" whole-file compressions
// via loadable libraries.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "XfdMaster.h"

// reuse librarian for loading decrunchers
#include "XpkLibrarian.h"

// for parent-container, access helpers
#include "LibMaster.h"

#include "IoContext.h"


//////// protected methods

// destroy decruncher when necessary
void CXfdMaster::release()
{
	if (m_pSubLibrary != nullptr)
	{
		// don't delete for now, 
		// static instance given in libraries..
		// change later
		//delete m_pSubLibrary;
		m_pSubLibrary = nullptr;
	}
}

//////// public methods

CXfdMaster::CXfdMaster(QObject *parent)
	: QObject(parent)
	, m_SubLib(parent)
	, m_pSubLibrary(nullptr)
{
}

CXfdMaster::~CXfdMaster(void)
{
	release();
}

// detect XFD-supported decrunching from buffer..
//
bool CXfdMaster::isSupported(CIOBuffer *pInputBuffer, CFileType &type)
{
	std::string szSubType;

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
	else if (type.m_enFileType == HEADERTYPE_ZCOMPRESS)
	{
		// standalone sub-library available now..
		szSubType = "xfdZCompress";
	}
	else if (type.m_enFileType == HEADERTYPE_GZIP)
	{
		// load sub-library for handling GZIP
		szSubType = "xfdGZIP";
	}
	else if (type.m_enFileType == HEADERTYPE_SZDD)
	{
		// standalone sub-library available now..
		szSubType = "xfdSZDD";
	}

	release(); // release existing if necessary
	
	// TODO: enumerate suitable sub-libraries,
	// ask each one if this is supported there..
	
	if (szSubType.length() == 0)
	{
		// not supported/not known..
		return false;
	}
	// try loading it
	// load suitable sub-library?
	m_pSubLibrary = m_SubLib.getXfdInstance(QString::fromStdString(szSubType));
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		//throw ArcException("Unsupported archive type", subType);
		return false; // don't throw here, trying to determine if supported..
	}
	return m_pSubLibrary->isSupported(pInputBuffer);
}

bool CXfdMaster::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	// only sub-library knows necessary metadata
	// which caller might want..
	// -> get it from library-code
	// also need to give input to library..
	//
	CLibMaster *plm = (CLibMaster*)parent();
	CIoContext *pIn = plm->getCurrentInput();
	return m_pSubLibrary->archiveInfo(pIn, info);
}

bool CXfdMaster::decrunch(XpkProgress *pProgress)
{
	// might as well check it..
	//CLibMaster *plm = dynamic_cast<CLibMaster>(parent());
	
	CLibMaster *plm = (CLibMaster*)parent();
	CIoContext *pIn = plm->getCurrentInput();
	CIoContext *pOut = plm->getCurrentOutput();

	// get simple accessor for whole file to be processed:
	// XFD wants whole file access usually and outputs whole file decrunched
	//	
	// TODO: just give ptr as method param instead?
	// should simplify whole lot of handling..
	//
	pProgress->pInputBuffer = pIn->getBuffer();
	pProgress->pOutputBuffer = pOut->getBuffer();
	pProgress->xp_chunkIn = pProgress->xp_WholePackedFileSize;

	if (m_pSubLibrary->Decrunch(pProgress) == false)
	{
		throw ArcException("Decrunching failed", pIn->getName().toStdString());
	}
	
	pOut->write(pProgress->xp_chunkOut); // amount decrunched (whole file)
	return true;
}

