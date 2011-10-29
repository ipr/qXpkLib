/* 
xpkSHSC -- smart delta with HA (ASC/HSC)
Author: Harri Hirvola and chinoclast@softhome.net (Gaelan Griffin)

Modified for C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
for qXpkLib project.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "xpkSHSC.h"

// from master-project for buffer-class
#include "AnsiFile.h"

// from master-project (fulfill fwd. decl.)
#include "XpkProgress.h"

// (see header)
xpkSHSC g_Instance;
xpkLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to dynamic allocation..: new xpkRLEN() when there are members..
	return &g_Instance;
}

xpkSHSC::xpkSHSC()
    : xpkLibraryBase()
{
	// only to/from buffer supported here
	m_XpkCaps.input.buffer = true;
	m_XpkCaps.output.buffer = true;
	m_XpkCaps.m_LibIdentifier = "SHSC";
	m_XpkCaps.m_LibDescription = "XPK (SHSC) implementation";
}

xpkSHSC::~xpkSHSC()
{
}

/*
bool xpkSHSC::Crunch(XpkProgress *pProgress)
{
	// not supported (yet)
	return false;
}
*/

bool xpkSHSC::Decrunch(XpkProgress *pProgress)
{
	
	return true;
}

