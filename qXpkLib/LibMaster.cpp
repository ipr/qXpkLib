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

// reuse type detection,
// multiple variations etc.
//
#include "FileType.h"

#include "XpkLibrarian.h"


///////// protected methods

std::string CLibMaster::getCruncherType(CReadBuffer *pInputBuffer)
{
	// simplify, use std::string and get it done
	std::string szSubType;
	
	// determine file datatype by header information
	CFileType type(pInputBuffer->GetBegin(), pInputBuffer->GetSize());
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
	else if (type.m_enFileType == HEADERTYPE_XPK_GENERIC)
	{
		// TODO: if "PWPK" then XPK-contained PowerPacker crunching?
		// also others:
		// in xpk-dev: NUKE, DUKE, RLEN, HUFF, FAST..
		// others: SMPL, SLZ3, SHRI, PWPK, PPMQ, 
		// IDEA?, MASH, LHLB, ILZR, FRLE, FBR2, 
		// DMCB, DLTA, CBR0, BLZW, BLFH..
		//
		//szSubType.assign(m_InputBuffer.GetAt(8), 4);
		szSubType = "xpk";
		szSubType.append(m_InputBuffer.GetAt(8), 4);
	}
	else if (type.m_enFileType == HEADERTYPE_XFD_GENERIC)
	{
		// XFD-packed file ("XFDD" or "XFDF")
	}
	else if (type.m_enFileType == HEADERTYPE_XPK_SQSH)
	{
		szSubType = "xpkSQSH";
	}
	/* testing generic detection -> commented out
	else if (type.m_enFileType == HEADERTYPE_XPK_NUKE)
	{
	}
	else if (type.m_enFileType == HEADERTYPE_XPK_RLEN)
	{
		// already detected as XPK
		// -> load sub-library (get actual type)
		//szSubType.assign(m_InputBuffer.GetAt(8), 4);
	}
	*/
	else if (type.m_enFileType == HEADERTYPE_GZIP)
	{
		// load sub-library for handling GZIP
		szSubType = "xpkGZIP";
	}
	else if (type.m_enFileType == HEADERTYPE_AMIGAOS)
	{
		// AmigaOS executable (M68k),
		// may be self-extracting application..
		// -> needs more extensive checking
		// if there is "decrunchable" data after loader
		// or if it needs to be run in emulator or natively..
	}
	else if (type.m_enFileType == HEADERTYPE_UNIXELF)
	{
		// Unix-style ELF-binary,
		// PPC Amiga or any Unix-variant or Linux?
		// may be self-extracting application
		// -> needs more extensive checking
		// if there is "decrunchable" data after loader
		// or if it needs to be run in emulator or natively..
	}
	else if (type.m_enFileType == HEADERTYPE_ZCOMPRESS)
	{
		// standalone sub-library available now..
		szSubType = "xfdZCompress";
	}
	else if (type.m_enFileType == HEADERTYPE_SZDD)
	{
		// standalone sub-library available now..
		szSubType = "xfdSZDD";
	}
	else
	{
		// others?
		// -> should be as sub-libraries..
		
		if (m_pXadMaster->isSupported(pInputBuffer) == true)
		{
		}
		
		// detect some more types: files which XFD supports
		// but have "alien" fileformat..
		if (m_pXfdMaster->isSupported(pInputBuffer) == true)
		{
			// handle as "XFD-GENERIC" type..
		}
	}
	
	return szSubType;
}

void CLibMaster::PrepareUnpacker(std::string &subType)
{
	// don't delete since libraries now give "static" instance,
	// change later, for now leave "clean" in case of error..
	if (m_pSubLibrary != nullptr)
	{
		m_pSubLibrary = nullptr;
	}

	if (subType.length() < 4)
	{
		// should throw exception, for testing just skip
		return;
	}
	
	// load suitable sub-library?
	m_pSubLibrary = CXpkLibrarian::getDecruncher(QString::fromStdString(subType), m_SubLib);
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		throw ArcException("Unsupported cruncher type", subType);
	}
}

bool CLibMaster::OwnDecrunch(XpkProgress *pProgress)
{
	// XPK-container format in file:
	// we need to process XPK-tags in file
	// and pass chunks to sub-library for decrunching
	// -> not done yet..
	// temp, testing
	
	XpkChunk *pChunk = m_Tags.getFirst();
	while (pChunk != nullptr)
	{
		if (pChunk->m_Type == XPKCHUNK_END) // end of file
		{
			break;
		}
		else if (pChunk->m_Type == XPKCHUNK_RAW) // unpacked raw data
		{
			// unpacked raw data:
			// direct copy to output
			pProgress->pOutputBuffer->Append(
						pProgress->pInputBuffer->GetAt(pChunk->m_nDataOffset), 
						pChunk->m_ChunkLength);
		}
		else if (pChunk->m_Type == XPKCHUNK_PACKED) // packed
		{
			pProgress->xp_chunkIn = pChunk->m_ChunkLength;
			pProgress->xp_chunkOut = pChunk->m_UnLen;
		
			// locate data of chunk
			pProgress->pInputBuffer->SetCurrentPos(pChunk->m_nDataOffset);
			
			// prepare space for uncompressed data
			if (pChunk->m_UnLen > 0)
			{
				pProgress->pOutputBuffer->Reserve(pChunk->m_UnLen);
			}
			
			// decrunch chunk into output buffer
			if (m_pSubLibrary->Decrunch(pProgress) == false)
			{
				// .. or throw exception now
				return false;
			}
		}
		else
		{
			// unknown type, error or skip it?
			throw ArcException("Unknown chunk type", pChunk->m_Type);
		}

		// next chunk to process..	
		pChunk = pChunk->m_pNext;
	}
	
	return true;
}


///////// public

CLibMaster::CLibMaster(QObject *parent)
	: QObject(parent)
    , m_SubLib(parent)
    , m_pXpkMaster(nullptr)
    , m_pXfdMaster(nullptr)
    , m_pXadMaster(nullptr)
    , m_InputName()
    , m_nInputFileSize(0)
    , m_InputBuffer(1024)
    , m_Output()
{
	// temp, check handling later..
	// create when selected archive changed?
	//
	m_pXpkMaster = new CXpkMaster();
	m_pXfdMaster = new CXfdMaster();
	m_pXadMaster = new CXadMaster();

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
}

bool CLibMaster::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	// TODO: get info to caller..
	// - compressor (SQSH, NUKE... PP20, IMPL..)
	// - unpacked size -> need sub-library to determine
	// - what else?

	return false;
}

bool CLibMaster::archiveUnpack(XpkProgress *pProgress)
{
	CAnsiFile InFile;
	if (InFile.Open(m_InputName.toStdString()) == false)
	{
		throw ArcException("Failed to open input", m_InputName.toStdString());
	}

	// just read it all to buffer, change later if wanted..
	m_nInputFileSize = InFile.GetSize();
	m_InputBuffer.PrepareBuffer(InFile.GetSize(), false);
	if (InFile.Read(m_InputBuffer.GetBegin(), InFile.GetSize()) == false)
	{
		throw IOException("Failed reading file data");
	}
	
	pProgress->xp_WholePackedFileSize = InFile.GetSize(); // info to decruncher
	InFile.Close(); // not needed any more
	

	// setup info for decrunch
	pProgress->pInputBuffer = &m_InputBuffer;
	pProgress->pOutputBuffer = m_Output.GetBuffer();
	
	// just decrunch all at once, write file when done
	bool bRet = false;
	if (m_pXadMaster->isSupported(&m_InputBuffer) == true)
	{
		m_pXadMaster->setExtractPath(m_outputPath);
		bRet = m_pXadMaster->extractArchive(pProgress);
	}
	else if (m_pXfdMaster->isSupported(&m_InputBuffer) == true)
	{
		bRet = m_pXfdMaster->decrunch(pProgress);
	}
	else if (m_pXpkMaster->IsXpkFile(m_InputBuffer) == true)
	{
		bRet = m_pXpkMaster->decrunch(pProgress);
	}
	else
	{
	
		// determine file type from header, try to load decruncher for it:
		// throws exception on failure
		//
		// TODO: don't read entire file before this,
		// only after it is known to be supported..
		//
		PrepareUnpacker(getCruncherType(&m_InputBuffer));
	
		// something else, 
		// sub-library should know what do with it..
		// we depend entirely on sub-library to do decrunching
		// -> pass entire file to sub-library
		//
		// whole file decompression only in this case
		pProgress->xp_chunkIn = pProgress->xp_WholePackedFileSize;
		pProgress->xp_chunkOut = 0; // we don't know this until library detects this..
		pProgress->pInputBuffer->SetCurrentPos(0);
		
		bRet = m_pSubLibrary->Decrunch(pProgress);
	}
	
	if (bRet == false)
	{
		throw ArcException("Decrunching failed", m_InputName.toStdString());
	}
	
	// write all at once when done..
	if (m_Output.getName().length() == 0)
	{
		// no output-file -> done
		// (user wants buffer-only?)
		return true;
	}
	
	// overwrite existing file?
	if (m_InputName == m_Output.getName())
	{
		InFile.Close();
	}
	m_Output.WriteFile();
		
	return true;
}

// input-file given: check what it is
// (for information to caller)
void CLibMaster::setInputFile(QString &szFile)
{
	m_InputName = szFile;
}