///////////////////////////////////////
//
// XPK RLEN sub-library implementation for qXpkLib
//
// Original Written by Dirk Stöcker <stoecker@amigaworld.com>
// UNIX version by Vesa Halttunen <vesuri@jormas.com>
// C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
//

#include "xpkRLEN.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"


//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)


// (see header)
xpkRLEN g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}

xpkRLEN::xpkRLEN()
 : xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "RLEN";
	m_XpkCaps.m_LibDescription = "XPK (RLEN) implementation";
}

xpkRLEN::~xpkRLEN()
{
}

bool xpkRLEN::Crunch(XpkProgress *pProgress)
{
	
	return false;
}

bool xpkRLEN::Decrunch(XpkProgress *pProgress)
{
	// keep input size and set to start
	const size_t nInputSize = pProgress->xp_chunkIn;
	
	while (pProgress->pInputBuffer->IsEnd() == false)
	{
		int iVal = *(pProgress->pInputBuffer->GetAtCurrent());
		if (iVal > 0)
		{
			// copy next N bytes as-is to output
			pProgress->pOutputBuffer->Append(pProgress->pInputBuffer->GetAtCurrent(), iVal-1);
		}
		else
		{
			// repeat BYTE to output
			int iCpy = *(pProgress->pInputBuffer->GetAtCurrent());
			for (iVal = -iVal; iVal > 0; iVal--)
			{
				pProgress->pOutputBuffer->SetNextByte((uint8_t)iCpy);
			}
		}
	}
	
	return true;

/*	
	//UBYTE *get = xpar->xsp_InBuf, *put = xpar->xsp_OutBuf, v;
    LONG i;
  
    while (i = (BYTE) *get++)
	{
      if(i > 0)
	  {
        for(; i > 0; i--)
		{
			*put++ = *get++;
		}
	  }
      else
	  {
        for(i = -i, v = *get++; i > 0; i--)
		{
			*put++ = v;
		}
	  }
	}
*/			
	
}

