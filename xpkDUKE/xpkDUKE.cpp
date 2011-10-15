#include "xpkDUKE.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// use shared helper for this (Delta-NUKE),
// also NUKE wants to use same
//
//#include "Nuke.h"

//TODO: when ready, we can add tracking of created instances
// (when library needs members per-user..)

// (see header)
xpkDUKE g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}


void DoDelta(uchar *dst, uchar *src, int32_t len)
{
	if (len <= 0)
	{
		return;
	}

	uint8_t a=0, b=0;

    switch (len & 7) 
    {
      do {
    case 0: *dst++ = (uint8_t) ((b=*src++) - a);
    case 7: *dst++ = (uint8_t) ((a=*src++) - b);
    case 6: *dst++ = (uint8_t) ((b=*src++) - a);
    case 5: *dst++ = (uint8_t) ((a=*src++) - b);
    case 4: *dst++ = (uint8_t) ((b=*src++) - a);
    case 3: *dst++ = (uint8_t) ((a=*src++) - b);
    case 2: *dst++ = (uint8_t) ((b=*src++) - a);
    case 1: *dst++ = (uint8_t) ((a=*src++) - b);
      } while ((len-=8)>0);
    }
}

void UndoDelta(uchar *dst, uchar *src, int32_t len)
{
	if (len <= 0)
	{
		return;
	}

	uint8_t l = 0;
	
    switch (len & 7) 
    {
     do {
    case 0: *dst++ = (l+=*src++);
    case 7: *dst++ = (l+=*src++);
    case 6: *dst++ = (l+=*src++);
    case 5: *dst++ = (l+=*src++);
    case 4: *dst++ = (l+=*src++);
    case 3: *dst++ = (l+=*src++);
    case 2: *dst++ = (l+=*src++);
    case 1: *dst++ = (l+=*src++);
      } while ((len-=8)>0);
    }
}


xpkDUKE::xpkDUKE()
    : xpkLibraryBase()
{
}

xpkDUKE::~xpkDUKE()
{
}

bool xpkDUKE::Crunch(XpkProgress *pProgress)
{
	//CNuke nuke;
	try
	{
		DoDelta(pProgress->pInputBuffer->GetCurrentPos(),
				pProgress->pOutputBuffer->GetCurrentPos(),
				pProgress->xp_chunkIn);
	
		//nuke.crunch();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}

bool xpkDUKE::Decrunch(XpkProgress *pProgress)
{
	//CNuke nuke;
	try
	{
		//nuke.decrunch();
		
		// TODO: check buffer positions after UnNuke()..
		
		UndoDelta(pProgress->pInputBuffer->GetCurrentPos(),
				pProgress->pOutputBuffer->GetCurrentPos(),
				pProgress->xp_chunkIn);
	}
	catch (std::exception &exp) // catch by base-type
	{
		// debug..
		//std::cerr << "Exception: " << exp.what() << std::end;
	}
	return false;
}

