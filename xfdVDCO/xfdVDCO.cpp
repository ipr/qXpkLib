#include "xfdVDCO.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use decrunching helper for actual decoding of data
#include "VDCO.h"

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xfdVDCO g_Instance;
xfdLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xfdVDCO::xfdVDCO()
	: xfdLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "VDCO"; //
	m_XpkCaps.m_LibDescription = "VDCO (Virtual Dreams) decruncher";
}

xfdVDCO::~xfdVDCO()
{
}

bool xfdVDCO::isSupported(CReadBuffer *pInputBuffer)
{
	if (::memcmp(pInputBuffer->GetBegin(), "VDCO", 4) == 0)
	{
		// VDCO crunched file
		return true;
	}
	return false;
}

bool xfdVDCO::archiveInfo(QXpkLib::CArchiveInfo &info)
{
}

// decrunching (unpacking) only supported for this format
bool xfdVDCO::Decrunch(XpkProgress *pProgress)
{
	// only whole file decompression for now..
	if (pProgress->xp_chunkIn < pProgress->xp_WholePackedFileSize)
	{
		return false;
	}
	
	try
	{
		// create decruncher for unpacking
		// (tODO: keep somewhere..)
		VDCO *pSlave = new VDCO(pProgress->pInputBuffer);
		bool bRes = pSlave->decrunch(pProgress->pOutputBuffer);
		if (bRes == true)
		{
			// need to update status for master-library
			// so it knows how much of data is in buffer..
			//pProgress->xp_chunkOut = ;
		}
		return bRes;
	}
	catch (std::exception &exp) // catch by base-type
	{
		// debug..
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}

