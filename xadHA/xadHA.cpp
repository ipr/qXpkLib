/* 
xadHA -- HA archive unpacker
Author: Harri Hirvola 

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

#include "xadHA.h"

#include "HA.h"

// (see header)
xadHA g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadHA::xadHA()
 : xadLibraryBase()
 , m_pArchive(nullptr)
{
	// so far only to/from file(s) supported here,
	// may add more (in progress)
	m_XpkCaps.input.file = true;
	m_XpkCaps.output.file = true;
	m_XpkCaps.m_LibIdentifier = "HA";
	m_XpkCaps.m_LibDescription = "HA archive unpacking";
}

xadHA::~xadHA()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
		m_pArchive = nullptr;
	}
}

bool xadHA::setArchive(QString &file)
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
	}
	m_pArchive = new CHA(file.toStdString());
	//return m_pArchive->List();
}

bool xadHA::archiveInfo(QXpkLib::CArchiveInfo &info)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	
	return true;
}

// set path to uncompress files to
bool xadHA::setExtractPath(QString &szPath)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	//return m_pArchive->SetExtractPath(szPath.toStdString());
}

// test archive integrity
bool xadHA::testArchive()
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	// not implemented yet
	//return m_pArchive->TestArchive();
}

// unpack/decompress
bool xadHA::Decrunch(XpkProgress *pProgress)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
		//return m_pArchive->Extract();
	}
	catch (std::exception &exp) // catch by base-type
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

