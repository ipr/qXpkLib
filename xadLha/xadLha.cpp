/* xadLHA : LHa extracting for qXpkLib
 *
 * Based on Unix LhA source code: lha-1.14i-ac20040929
 * Copyrights:
 * LHarc    for UNIX  V 1.02  Copyright(C) 1989  Y.Tagawa
 * LHx      for MSDOS V C2.01 Copyright(C) 1990  H.Yoshizaki
 * LHx(arc) for OSK   V 2.01  Modified     1990  Momozou
 * LHa      for UNIX  V 1.00  Copyright(C) 1992  Masaru Oki
 * LHa      for UNIX  V 1.14  Modified     1995  Nobutaka Watazaki
 * LHa      for UNIX  V 1.14i Modified     2000  Tsugio Okamoto
 *                    Autoconfiscated 2001-2004  Koji Arai
 * 
 * C++ library versions (qLhaLib and xadLHA) by Ilkka Prusi (2011).
 *
 */


#include "xadLha.h"

#include "LhArchive.h"
#include "LhHeader.h"


// (see header)
xadLha g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new lib() when there are members..
	return &g_Instance;
}

xadLha::xadLha()
 : xadLibraryBase()
 , m_pArchive(nullptr)
{
	// only from file supported here
	m_XpkCaps.input.file = true;
	
	// output (optionally) to buffer
	m_XpkCaps.output.file = true;
	m_XpkCaps.output.buffer = true;
	
	m_XpkCaps.m_LibIdentifier = "LhA";
	m_XpkCaps.m_LibDescription = "LhA unpacking implementation";
}

xadLha::~xadLha()
{
	if (m_pArchive != nullptr)
	{
		delete m_pArchive;
		m_pArchive = nullptr;
	}
}

bool xadLha::isSupported(CIOBuffer *pInputBuffer)
{
}

bool xadLha::archiveInfo(CIoContext *pInput, QXpkLib::CArchiveInfo &info)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}
	
	try
	{
		QList<LzHeader*> lstArchiveInfo;
		m_pArchive->List(lstArchiveInfo);
		
		info.m_bIsMultifile = true;
		info.m_archiveInfo.m_fileName = m_pArchive->GetArchiveFileName();
		info.m_archiveInfo.m_ulFileSize = m_pArchive->GetArchiveFileSize();
		info.m_archiveInfo.m_packing = "LhA";
		
		auto it = lstArchiveInfo.begin();
		auto itEnd = lstArchiveInfo.end();
		while (it != itEnd)
		{
			LzHeader *pHeader = (*it);
			
			// if it's directory-entry -> nothing more to do here
			if (pHeader->isDir())
			{
				++it;
				continue;
			}
			
			info.m_fileList.push_back(QXpkLib::CEntryInfo());
			
			QXpkLib::CEntryInfo &Entry = info.m_fileList.back();
			Entry.m_fileName = pHeader->filename;
			Entry.m_ulPackedSize = pHeader->packed_size;
			Entry.m_ulUnpackedSize = pHeader->original_size;
			Entry.m_packing = pHeader->pack_method;
			Entry.m_Stamp = pHeader->last_modified_stamp;
			Entry.m_szComment = pHeader->file_comment;
			
			// update archive statistics
			info.m_archiveInfo.m_ulPackedSize += pHeader->packed_size;
			info.m_archiveInfo.m_ulUnpackedSize += pHeader->original_size;
			
			++it;
		}
		
		return true;
	}
	catch (std::exception &exp)
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

// test archive integrity
bool xadLha::testArchive(CIoContext *pInput)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
		return m_pArchive->Test();
	}
	catch (std::exception &exp)
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

// unpack/decompress
bool xadLha::Decrunch(XpkProgress *pProgress)
{
	if (m_pArchive == nullptr)
	{
		return false;
	}

	try
	{
		return m_pArchive->Extract();
	}
	catch (std::exception &exp)
	{
		//emit fatal_error(exp.what());
	}
	return false;
}

