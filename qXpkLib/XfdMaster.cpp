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


//////// protected methods

XfdSlave *CXfdMaster::loadDecruncher(CReadBuffer *pInputBuffer)
{
	// TODO: implement..
	// some stuff which may be added (libraries? just classes?)
	// are "decrunch-slaves" for some "alien" crunchers (non-XPK/XFD fileformat)
	//
	// needs huge work to rewrite all XFD-supported from M68k ASM to portable C/C++
	// so might take a while.. (being optimistic here)
	//
	// note:
	// PowerPacker and Imploder "standalone" format 
	// already supported in XPK-style library, are they needed here as well..? 
	//
	// - ByteKiller (1.3, 2.0, 3.0, Pro 1.0..)
	// - CrunchMania (...)
	// - TetraPack (...)
	// - MasterCruncher?
	// ... etc.
	// 
	// TODO: should ask "slave" decruncher directly if supported..
	// so many variations..
	//
	
	if (::memcmp(pInputBuffer->GetBegin(), "LPAK", 4) == 0)
	{
		// "LPAK data cruncher", 
		// similar to "SZDD" except less headers etc.
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "SZDD", 4) == 0)
	{
		// "Microsoft SZDD data cruncher",
		// The compressed data format is basically LZSS
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "CRUN", 4) == 0)
	{
		// ByteKiller clones
		return new XfdByteKiller(pInputBuffer);
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "CRND", 4) == 0)
	{
		// ByteKiller clones
		return new XfdByteKiller(pInputBuffer);
	}
	/* // more ByteKiller clones..
		cmp.l	#'MARC',(A0)
		beq.b	.OK
		cmp.l	#'TMB!',(A0)
		beq.b	.OK
		cmp.l	#'TXIC',(A0)
		beq.b	.OK
		cmp.l	#'SCC!',(A0)
		beq.b	.OK
		cmp.l	#'ons ',(A0)
		bne.b	.Exit
		cmp.l	#' Sym',-(A0)
		bne.b	.Exit
		cmp.l	#'Gary',-(A0)
		bne.b	.Exit
		... and so on..
	*/
	else if (::memcmp(pInputBuffer->GetBegin(), "Vice", 4) == 0)
	{
		// Vice cruncher
		return new XfdVice(pInputBuffer);
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "VDCO", 4) == 0)
	{
		// VDCO (Virtual Dreams)
		return new XfdVDCO(pInputBuffer);
	}
	else if (::memcmp(pInputBuffer->GetBegin(), "ZbL!", 4) == 0)
	{
		// one of Zeeball's filetypes (many more it seems..)
	}
	/*
	else if (::memcmp(pInputBuffer->GetAt(0), "TPWM", 4) == 0)
	{
	}
	*/
	// .. and so on ..
	
	return nullptr;
}

// destroy decruncher when necessary
void CXfdMaster::release()
{
	if (m_pSubLibrary != nullptr)
	{
		// don't delete for now, 
		// static instance given in libraries..
		// change later
		m_pSubLibrary = nullptr;
	}
	if (m_pXfdSlave != nullptr)
	{
		delete m_pXfdSlave;
		m_pXfdSlave = nullptr;
	}
}

//////// public methods

CXfdMaster::CXfdMaster(QObject *parent)
	: QObject(parent)
	, m_pXfdSlave(nullptr) // TODO: move to a library?
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
	
	if (szSubType.length() > 0)
	{
		// try loading it
		// load suitable sub-library?
		m_pSubLibrary = CXpkLibrarian::getXadInstance(QString::fromStdString(subType), m_SubLib);
		if (m_pSubLibrary == nullptr)
		{
			// not supported/can't load -> can't decrunch it
			//throw ArcException("Unsupported archive type", subType);
			return false; // don't throw here, trying to determine if supported..
		}
	}
	else
	{
		// try internally known slaves for now
		
		// TODO: move to a library?
		
		m_pXfdSlave = loadDecruncher(pInputBuffer);
		if (m_pXfdSlave == nullptr)
		{
			// not supported/can't load -> can't decrunch it
			//throw ArcException("Unsupported archive type", subType);
			return false; // don't throw here, trying to determine if supported..
		}
	}
	return true;
}

bool CXfdMaster::decrunch(XpkProgress *pProgress)
{
	release(); // release existing if necessary
	
	// TODO: switch to external libraries..
	
	m_pXfdSlave = loadDecruncher(pProgress->pInputBuffer);
	if (m_pXfdSlave != nullptr)
	{
		// decruncher located -> supported
		return m_pXfdSlave->decrunch(pProgress->pOutputBuffer);
	}
	return false;
}

