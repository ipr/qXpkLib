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


// Purpose of this is to keep track 
// of how much is read/written to/from input/output,
// this should be generic so that "sub-master" instances
// and extension libraries don't need to repeat same stuff..
// Also try to reduce re-opening files etc.
// 
class CIoContext
{
protected:
	QString m_Name; // in/out name
	//QString m_Path; // for multi-file output?
	CReadBuffer m_Buffer;
	
	// temp (optional)
	// this would not be needed in buffer-to-buffer decrunch..
	CAnsiFile m_File;
	
	size_t m_nFileOffset; 
	
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
	    , m_File()
	{}
    CIoContext(QString &Name, size_t nBufferSize = 1024)
	    : m_Name(Name)
	    , m_Buffer(nBufferSize)
	    , m_File()
	{}
    CIoContext(const unsigned char *pBuf, const size_t nSize)
	    : m_Name()
	    , m_Buffer(nSize)
	    , m_File()
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
	CAnsiFile *GetFile()
	{
		return &m_File;
	}

	//bool Read(XpkProgress *pProgress, size_t nMaxRead = 0)
	bool Read(size_t nMaxRead = 0)
	{
		if (m_File.Open(m_Name.toStdString()) == false)
		{
			throw ArcException("Failed to open input", m_Name.toStdString());
		}
		//pProgress->xp_WholePackedFileSize = m_File.GetSize();
		
		if (nMaxRead == 0)
		{
			m_Buffer.PrepareBuffer(InFile.GetSize(), false);
			if (m_File.Read(m_Buffer.GetBegin(), InFile.GetSize()) == false)
			{
				throw IOException("Failed reading file data");
			}
			m_nFileOffset = InFile.GetSize();
		}
		else
		{
			// TODO: better control over reading.. this just for testing
			size_t nReadSize = (nMaxRead < InFile.GetSize()) ? nMaxRead : InFile.GetSize();
			m_Buffer.PrepareBuffer(nReadSize, false);
			if (m_File.Read(m_Buffer.GetBegin(), nReadSize) == false)
			{
				throw IOException("Failed reading file data");
			}
			m_nFileOffset += nReadSize;
		}
		return true;
	}
	
	// write output to file
	//bool WriteFile(XpkProgress *pProgress)
	bool WriteFile()
	{
		if (m_File.Open(m_Name.toStdString(), true) == false)
		{
			throw ArcException("Failed to open output", m_Name.toStdString());
		}
		
		// buffer may be larger than actual output: write only actual data
		//
		if (m_File.Write(m_Buffer.GetBegin(), m_Buffer.GetCurrentPos()) == false)
		{
			throw ArcException("Failed to write output", m_Name.toStdString());
		}
		
		if (m_File.Flush() == false)
		{
			throw ArcException("Failed to flush output", m_Name.toStdString());
		}
		m_File.Close();
		return true;
	}
};

#endif // IOCONTEXT_H
