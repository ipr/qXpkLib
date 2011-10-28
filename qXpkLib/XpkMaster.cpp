///////////////////////////////////
//
// Actual decoder handling:
// can load additional decoders as necessary,
// see xpkLibraryBase
//
// CXpkMaster : main decrunch handler
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "XpkMaster.h"

#include <exception>
#include <string>

// reuse type detection,
// multiple variations etc.
//
#include "FileType.h"

// reuse librarian for loading decrunchers
#include "XpkLibrarian.h"


///////// public

CXpkMaster::CXpkMaster(QObject *parent)
	: QObject(parent)
    , m_SubLib(parent)
    , m_InputName()
    , m_nInputFileSize(0)
    , m_InputBuffer(1024)
    , m_pSubLibrary(nullptr)
{
}

CXpkMaster::~CXpkMaster(void)
{
}

bool CXpkMaster::isSupported(CReadBuffer *pInputBuffer, CFileType &type)
{
	// simplify, use std::string and get it done
	std::string szSubType;
	
	if (type.m_enFileType == HEADERTYPE_XPK_GENERIC)
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
		
		// TODO: try loading library to see if we can decrunch it too
	}
	/* testing generic detection -> commented out
	else if (type.m_enFileType == HEADERTYPE_XPK_SQSH)
	{
		szSubType = "xpkSQSH";
	}
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
	
	if (m_Tags.IsXpkFile(pInputBuffer) == false)
	{
		// should have detected already..
		return false;
	}

	if (subType.length() < 4)
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
	m_pSubLibrary = CXpkLibrarian::getXpkInstance(QString::fromStdString(subType), m_SubLib);
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		//throw ArcException("Unsupported cruncher type", subType);
		return false; // don't throw here, trying to determine if supported..
	}
	return true;
}

bool CXpkMaster::decrunch(XpkProgress *pProgress)
{
	// TODO: use from parent instead..
	//m_Input.GetBuffer()
	
	CAnsiFile InFile;
	if (InFile.Open(m_InputName.toStdString()) == false)
	{
		throw ArcException("Failed to open input", m_InputName.toStdString());
	}

	// just read it all to buffer, change later if wanted..
	pProgress->xp_WholePackedFileSize = InFile.GetSize();
	pProgress->pInputBuffer->PrepareBuffer(pProgress->xp_WholePackedFileSize, false);
	if (InFile.Read(m_InputBuffer.GetBegin(), pProgress->xp_WholePackedFileSize) == false)
	{
		throw IOException("Failed reading file data");
	}
	
	InFile.Close(); // not needed any more
	
	// determine file type from header, try to load decruncher for it:
	// throws exception on failure
	//
	// TODO: don't read entire file before this,
	// only after it is known to be supported..
	//
	PrepareUnpacker(getCruncherType(&m_InputBuffer));

	// TODO: if XFD-cruncher is needed, switch handling to suitable class..
	
	// setup info for decrunch
	pProgress->pInputBuffer = &m_InputBuffer;
	pProgress->pOutputBuffer = m_Output.GetBuffer();
	
	// just decrunch all at once, write file when done
	// XPK-container, process into tags
	// and chunk-nodes
	m_Tags.ParseChunks(m_InputBuffer);
	
	// result unpacked size
	pProgress->xp_UnpackedSize = m_Tags.getHeader()->xsh_ULen;

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
						
			if (m_Tags.verifyChecksum(pChunk, pProgress->pOutputBuffer) == false)
			{
			}
						
			// accounting in master-library
			pProgress->xp_PackedProcessed += pProgress->xp_chunkIn;
			pProgress->xp_UnpackedProcessed += pProgress->xp_chunkOut;
		}
		else if (pChunk->m_Type == XPKCHUNK_PACKED) // packed
		{
			pProgress->xp_chunkIn = pChunk->m_ChunkLength;
			pProgress->xp_chunkOut = pChunk->m_UnLen;
		
			// locate data of chunk,
			// currently we have entire file in buffer
			// so move data to beginning for simplicity in sub-library..
			//pProgress->pInputBuffer->SetCurrentPos(pChunk->m_nDataOffset);
			pProgress->pInputBuffer->MoveToBegin(pChunk->m_nDataOffset);
			pProgress->pInputBuffer->SetCurrentPos(0);
			
			// prepare space for uncompressed data
			if (pChunk->m_UnLen > 0)
			{
				pProgress->pOutputBuffer->Reserve(pChunk->m_UnLen);
			}
			
			// decrunch chunk into output buffer
			if (m_pSubLibrary->Decrunch(pProgress) == false)
			{
				// .. or throw exception now
				throw ArcException("Decrunching failed", m_InputName.toStdString());
				//return false;
			}
			
			// TODO: need checksum somewhere around here..
			if (m_Tags.verifyChecksum(pChunk, pProgress->pOutputBuffer) == false)
			{
			}

			// keep accounting in master,
			// no need to bother sub-libraries with it			
			pProgress->xp_PackedProcessed += pProgress->xp_chunkIn;
			pProgress->xp_UnpackedProcessed += pProgress->xp_chunkOut;
			
			// TODO: if we have output to file,
			// append it now? (IOContext in parent..)
		}
		else
		{
			// unknown type, error or skip it?
			throw ArcException("Unknown chunk type", pChunk->m_Type);
		}

		// next chunk to process..	
		pChunk = pChunk->m_pNext;
	}
	
	// flush output now? (lib-parent does it?)
	
	return true;
}


