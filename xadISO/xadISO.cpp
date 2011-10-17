#include "xadISO.h"

#include <QFile>


// (see header)
xadISO g_Instance;
xadLibraryBase *GetXpkInstance(void)
{
  // TODO: switch to: new xpkRLEN() when there are members..
	return &g_Instance;
}


xadISO::xadISO()
 : xadLibraryBase()
 , m_pFile(nullptr)
{
}

xadISO::~xadISO()
{
	if (m_pFile != nullptr)
	{
		delete m_pFile; // should close itself in destructor
		m_pFile = nullptr;
	}
}

// set path to uncompress files to
bool xadISO::setExtractPath(QString &szPath)
{
	//m_pArchive->SetExtractPath(szPath.toStdString());
	return true;
}

// test archive integrity
bool xadISO::testArchive()
{
	// not implemented yet
	return false;
}

// unpack/decompress
bool xadISO::Decrunch(XpkProgress *pProgress)
{
	m_pFile = new QFile(m_filename);
	qint64 size = m_pFile->size();
	uchar *pView = m_pFile->map(0, size);
	uchar *pos = pView;
	
	while (pos < (pView + size))
	{
	}

	return true;
}

