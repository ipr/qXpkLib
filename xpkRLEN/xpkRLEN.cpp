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
	//unsigned char *pIn = pProgress->pInputBuffer->GetBegin();
	//unsigned char *pOut = pProgress->pOutputBuffer->GetBegin();
	
	// keep input size and set to start
	const size_t nInputSize = pProgress->xp_chunkIn;
	//pProgress->pInputBuffer->SetCurrentPos(0); // should be set by master-library..
	
	while (pProgress->pInputBuffer->IsEnd() == false)
	{
		int iVal = *(pProgress->pInputBuffer->GetAtCurrent());
		if (iVal > 0)
		{
			pProgress->pOutputBuffer->Append(pProgress->pInputBuffer->GetAtCurrent(), iVal-1);
			/*
			for(; iVal > 0; iVal--)
			{
				int iCpy = *(pProgress->pInputBuffer->GetAtCurrent());
				*put++ = iCpy;
			}
			*/
		}
		else
		{
			// repeat to output
			int iCpy = *(pProgress->pInputBuffer->GetAtCurrent());
			for (iVal = -iVal; iVal > 0; iVal--)
			{
				unsigned char *pOut = pProgress->pOutputBuffer->GetAtCurrent();
				*pOut++ = (unsigned char)iCpy;
				pProgress->pOutputBuffer->SetCurrentPos(pProgress->pOutputBuffer->GetCurrentPos()+1);
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

