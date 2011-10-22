//////////////////////////////////////
//
// IoContext.h
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//
// Defines wrapper for IO:
// buffered-IO, memorymapping or just buffered
//


#ifndef IOCONTEXT_H
#define IOCONTEXT_H

#include <QObject>

#include "AnsiFile.h"
//#include "MemoryMappedFile.h"

#include "XpkCapabilities.h"
#include "XpkProgress.h"


class CIoContext
{
protected:
	QString m_Name; // in/out name
	//QString m_Path; // for multi-file output?
	CReadBuffer m_Buffer;
	
	// temp (optional)
	//CAnsiFile m_File;
	
public:
	/*
    CIoContext(void)
	    : m_Name()
	    , m_Buffer(1024)
	{}
	*/
    CIoContext(size_t nBufferSize = 1024)
	    : m_Name()
	    , m_Buffer(nBufferSize)
	{}
    CIoContext(QString &Name, size_t nBufferSize = 1024)
	    : m_Name(Name)
	    , m_Buffer(nBufferSize)
	{}
    CIoContext(const unsigned char *pBuf, const size_t nSize)
	    : m_Name()
	    , m_Buffer(nSize)
	{
		m_Buffer.Append(pBuf, nSize);
	}
	CReadBuffer *GetBuffer()
	{
		return &m_Buffer;
	}
	void setName(QString &Name)
	{
		m_Name = Name;
	}
	QString getName() const
	{
		return m_Name;
	}

	bool Read(XpkProgress *pProgress)
	{
		CAnsiFile InFile;
		if (InFile.Open(m_Name.toStdString()) == false)
		{
			throw ArcException("Failed to open input", m_Name.toStdString());
		}
		pProgress->xp_WholePackedFileSize = InFile.GetSize();
		
		size_t nReadSize = (1024 < InFile.GetSize()) ? 1024 : InFile.GetSize();
		m_Buffer.PrepareBuffer(nReadSize, false);
		if (InFile.Read(m_Buffer.GetBegin(), nReadSize) == false)
		{
			throw IOException("Failed reading file data");
		}
		InFile.Close(); // not needed any more
		return true;
	}
	
	// write output to file
	bool WriteFile(XpkProgress *pProgress)
	{
		CAnsiFile OutFile;
		if (OutFile.Open(m_Name.toStdString(), true) == false)
		{
			throw ArcException("Failed to open output", m_Name.toStdString());
		}
		
		// buffer may be larger than actual output: write only actual data
		//
		if (OutFile.Write(m_Buffer.GetBegin(), m_Buffer.GetCurrentPos()) == false)
		{
			throw ArcException("Failed to write output", m_Name.toStdString());
		}
		
		if (OutFile.Flush() == false)
		{
			throw ArcException("Failed to flush output", m_Name.toStdString());
		}
		OutFile.Close();
		return true;
	}
};

#endif // IOCONTEXT_H
