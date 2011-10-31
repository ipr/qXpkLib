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


//////// protected methods

// destroy decruncher when necessary
void CXpkMaster::release()
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


///////// public

CXpkMaster::CXpkMaster(QObject *parent)
	: QObject(parent)
    , m_SubLib(parent)
    , m_pSubLibrary(nullptr)
{
}

CXpkMaster::~CXpkMaster(void)
{
	release();
}

bool CXpkMaster::isSupported(CReadBuffer *pInputBuffer, CFileType &type)
{
	/*	
	if (type.m_enFileType == HEADERTYPE_XPK_GENERIC)
	{
		// TODO: if "PWPK" then XPK-contained PowerPacker crunching?
		// also others:
		// in xpk-dev: NUKE, DUKE, RLEN, HUFF, FAST..
		// others: SMPL, SLZ3, SHRI, PWPK, PPMQ, 
		// IDEA?, MASH, LHLB, ILZR, FRLE, FBR2, 
		// DMCB, DLTA, CBR0, BLZW, BLFH..
		//
		szSubType = "xpk";
		szSubType.append(m_InputBuffer.GetAt(8), 4);
	}
	*/
	
	if (m_Tags.ParseHeader(pInputBuffer) == false)
	{
		return false;
	}

	std::string szSubType = m_Tags.getTypename();
	if (szSubType.length() < 4)
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
	
	// try to load suitable sub-library?
	QString szLib = "xpk";
	szLib.append(QString::fromStdString(subType));
	
	m_pSubLibrary = CXpkLibrarian::getXpkInstance(szLib, m_SubLib);
	if (m_pSubLibrary == nullptr)
	{
		// not supported/can't load -> can't decrunch it
		//throw ArcException("Unsupported cruncher type", subType);
		return false; // don't throw here, trying to determine if supported..
	}
	return true;
}

bool CXpkMaster::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	// TODO: check that file header has been processed already
	// and that it's supported..
	
	// get XPK-archive descriptor (file-header)
	// 
	XpkStreamHeader *pHead = m_Tags.getHeader();
	
	info.m_bIsMultifile = false;
	info.m_bIsMultiVolume = false;
	info.m_archiveInfo.m_packing = QString::fromStdString(m_Tags.getTypename());
	info.m_archiveInfo.m_ulPackedSize = pHead->xsh_CompressedLen;
	info.m_archiveInfo.m_ulUnpackedSize = pHead->xsh_UnpackedLen;
	
	return true;
}

bool CXpkMaster::decrunch(XpkProgress *pProgress)
{
	// just decrunch all at once, write file when done
	// XPK-container, process into tags
	// and chunk-nodes
	m_Tags.ParseChunks(pProgress->pInputBuffer);
	
	// result unpacked size
	pProgress->xp_UnpackedSize = m_Tags.getHeader()->xsh_UnpackedLen;

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


