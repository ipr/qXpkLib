///////////////////////////////////
//
// XPK-decrunching support:
// should load additional decoders as necessary.
//
// Support for decrunching "native" chunk-based crunched files
// via loadable libraries for given compression method.
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#include "XpkMaster.h"

// reuse librarian for loading decrunchers
#include "XpkLibrarian.h"

// for parent-container, access helpers
#include "LibMaster.h"

#include "IoContext.h"


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

bool CXpkMaster::isSupported(CIOBuffer *pInputBuffer, CFileType &type)
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
	szLib.append(QString::fromStdString(szSubType));
	
	m_pSubLibrary = m_SubLib.getXpkInstance(szLib);
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
	const XpkStreamHeader *pHead = m_Tags.getHeader();

	// all necessary metadata known by XPK-master,
	// sub-libraries only see compressed chunks of data..
	//
	info.m_bIsMultifile = false;
	info.m_bIsMultiVolume = false;
	info.m_archiveInfo.m_ulPackedSize = pHead->xsh_CompressedLen;
	info.m_archiveInfo.m_ulUnpackedSize = pHead->xsh_UnpackedLen;
	info.m_archiveInfo.m_packing = QString::fromStdString(m_Tags.getTypename());
	
	return true;
}

// XPK-container format in file:
// process chunks in file
// and pass to sub-library for decrunching
bool CXpkMaster::decrunch(XpkProgress *pProgress)
{
	// just decrunch all at once, write file when done
	// XPK-container, process into tags
	// and chunk-nodes
	m_Tags.ParseChunks(pProgress->pInputBuffer);
	
	// might as well check it..
	//CLibMaster *plm = dynamic_cast<CLibMaster>(parent());
	
	CLibMaster *plm = (CLibMaster*)parent();
	CIoContext *pIn = plm->getCurrentInput();
	CIoContext *pOut = plm->getCurrentOutput();

	// result unpacked size
	pProgress->xp_UnpackedSize = m_Tags.getHeader()->xsh_UnpackedLen;
	
	// TODO: just give ptr as method param instead?
	// should simplify whole lot of handling..
	pProgress->pOutputBuffer = pOut->getBuffer();
	
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
			pOut->getBuffer()->Append(
						pIn->getBuffer()->GetAt(pChunk->m_nDataOffset), 
						pChunk->m_ChunkLength);
						
			if (m_Tags.verifyChecksum(pChunk, pProgress->pOutputBuffer) == false)
			{
			}
			pOut->write(pChunk->m_ChunkLength);
			
			// accounting in master-library
			pProgress->xp_PackedProcessed += pProgress->xp_chunkIn;
			pProgress->xp_UnpackedProcessed += pProgress->xp_chunkOut;
		}
		else if (pChunk->m_Type == XPKCHUNK_PACKED) // packed
		{
			pProgress->xp_chunkIn = pChunk->m_ChunkLength;
			pProgress->xp_chunkOut = pChunk->m_UnLen;
		
			// get simple accessor for chunk-part of file wanted to be processed
			pProgress->pInputBuffer = pIn->getBufferAtOffset(pChunk->m_nDataOffset);
			
			// prepare space for uncompressed data
			if (pChunk->m_UnLen > 0)
			{
				pProgress->pOutputBuffer->Reserve(pChunk->m_UnLen);
			}
			
			// decrunch chunk into output buffer
			if (m_pSubLibrary->Decrunch(pProgress) == false)
			{
				// .. or throw exception now
				throw ArcException("Decrunching failed", pIn->getName().toStdString());
				//return false;
			}
			
			// TODO: need checksum somewhere around here..
			if (m_Tags.verifyChecksum(pChunk, pProgress->pOutputBuffer) == false)
			{
			}
			pOut->write(pChunk->m_ChunkLength); // amount decrunched
			delete pProgress->pInputBuffer; // destroy old accesssor

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
	
	return true;
}

