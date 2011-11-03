///////////////////////////////////
//
// XFD-decrunching support:
// should load additional decoders as necessary.
//
// May be called from XpkMaster when XFD-decruncher is needed.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "XfdMaster.h"

#include "FileType.h"

// reuse librarian for loading decrunchers
#include "XpkLibrarian.h"

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
bool CXfdMaster::isSupported(CReadBuffer *pInputBuffer, CFileType &type)
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
	m_pSubLibrary = CXpkLibrarian::getXfdInstance(QString::fromStdString(subType), m_SubLib);
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		//throw ArcException("Unsupported archive type", subType);
		return false; // don't throw here, trying to determine if supported..
	}
	return m_pSubLibrary->isSupported(pInputBuffer);
}

bool CXfdMaster::decrunch(XpkProgress *pProgress)
{
	CIoContext *pIn = pProgress->pInputIo;
	CIoContext *pOut = pProgress->pOutputIo;

	// get simple accessor for whole file to be processed:
	// XFD wants whole file access usually and outputs whole file decrunched
	pProgress->pInputBuffer = pIn->getBuffer();
	pProgress->pOutputBuffer = pOut->getBuffer();
	pProgress->xp_chunkIn = pProgress->xp_WholePackedFileSize;

	if (m_pSubLibrary->Decrunch(pProgress) == false)
	{
		throw ArcException("Decrunching failed", m_InputName.toStdString());
	}
	
	pOut->write(pProgress->xp_chunkOut); // amount decrunched (whole file)
	return true;
}

