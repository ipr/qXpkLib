///////////////////////////////
//
// CBzip2 : wrapper for Bzip2 functions
// to be used in C++ library "glue" code.
//
// Author: Ilkka Prusi
// ilkka.prusi@gmail.com
//


#include "CBzip2.h"


/////////// protected methods

bool CBzip2::isValidHeader(CReadBuffer &buf)
{
	uint8_t *pBuf = buf.GetBegin();

	if (pBuf[0] == 'B'
		&& pBuf[1] == 'Z'
		&& pBuf[2] == '0')
	{
		// old (ver 1?) header
		m_nCompressionMode = 0;
		return true;
	}
	else if (pBuf[0] == 'B'
		&& pBuf[1] == 'Z'
		&& pBuf[2] == 'h')
	{
		m_nCompressionMode = (pBuf[3] - 0x30);
		return true;
	}
	
	// not support type
	return false;
}

/////////// public methods

CBzip2::CBzip2(QString &szArchive, QObject *parent)
 : QObject(parent)
 , m_szArchive(szArchive)
 , m_nFileSize(0)
 , m_nCompressionMode(0)
{
}

CBzip2::~CBzip2(void)
{
}

bool CBzip2::Test()
{
	CAnsiFile archive(m_szArchive.toStdString());
	if (archive.IsOk() == false)
	{
		throw IOException("Failed opening archive");
	}
	m_nFileSize = archive.GetSize();
	
	// test: is normal file? (not device or something..?)

	bool isFirst = true; // .. maybe some better way ..
	size_t nRead = 0;
	CReadBuffer buf(5000);
	
	while (nRead < archive.GetSize())
	{
	
		size_t toRead = (5000 < (archive.GetSize() - nRead)) ? 5000 : (archive.GetSize() - nRead);
		if (archive.Read(buf.GetBegin(), toRead) == false)
		{
			throw IOException("Failure reading archive");
		}
		nRead += toRead;
		
		if (isFirst == true)
		{
			if (checkHeader(buf) == false)
			{
				// invalid or not supported type
				throw IOException("Unsupported file type");
			}
		}
		
		isFirst = false;
	
		
	}

	//testStream();

}
	
bool CBzip2::Extract()
{
	CAnsiFile archive(m_szArchive.toStdString());
	if (archive.IsOk() == false)
	{
		throw IOException("Failed opening archive");
	}

}
