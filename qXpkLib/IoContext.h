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
#include <QString>
#include <QFile>

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
// TODO: change to pure virtual (abstract) interface only
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
	
	// TODO: memory-mapped file would be simpler 
	// and generally better choice..
	// -> switch later..
	
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
	    , m_nFileOffset(0)
	{}
    CIoContext(QString &Name, size_t nBufferSize = 1024)
	    : m_Name(Name)
	    , m_Buffer(nBufferSize)
	    , m_File()
	    , m_nFileOffset(0)
	{}
    CIoContext(const unsigned char *pBuf, const size_t nSize)
	    : m_Name()
	    , m_Buffer(nSize)
	    , m_File()
	    , m_nFileOffset(0)
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
	
	void Read() // whole file
	{
		if (m_File.IsOpen() == false)
		{
			if (m_File.Open(m_Name.toStdString()) == false)
			{
				throw ArcException("Failed to open input", m_Name.toStdString());
			}
		}
		long lPos = 0;
		if (m_File.Tell(lPos) == false)
		{
			throw ArcException("Failed to tell positions", m_Name.toStdString());
		}
		if (lPos != 0)
		{
			if (m_File.Seek(0, SEEK_SET) == false)
			{
				throw ArcException("Failed to seek position", m_Name.toStdString());
			}
		}
		m_Buffer.PrepareBuffer(InFile.GetSize(), false);
		if (m_File.Read(m_Buffer.GetBegin(), InFile.GetSize()) == false)
		{
			throw IOException("Failed reading file data");
		}
		m_nFileOffset = InFile.GetSize();
	}
	
	void Read(const size_t nMaxRead) // chunk of data
	{
		if (m_File.IsOpen() == false)
		{
			if (m_File.Open(m_Name.toStdString()) == false)
			{
				throw ArcException("Failed to open input", m_Name.toStdString());
			}
		}
		
		size_t remaining = 0;
		if (m_File.GetSize() > m_nFileOffset)
		{
			remaining = m_File.GetSize() - m_nFileOffset;
		}
		
		// TODO: better control over reading.. this just for testing
		size_t nReadSize = (nMaxRead < remaining) ? nMaxRead : remaining;
		
		m_Buffer.PrepareBuffer(nReadSize, false);
		if (m_File.Read(m_Buffer.GetBegin(), nReadSize) == false)
		{
			throw IOException("Failed reading file data");
		}
		m_nFileOffset += nReadSize;
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

// refactoring

class CMemoryMappedIO : public CIoContext
{
protected:
	QFile *m_file;
	QString m_Name; // in/out name

public:
	CMemoryMappedIO()
		: CIoContext()
	{}
	
};

class CBufferIO : public CIoContext, public CReadBuffer
{
public:
	CBufferIO()
		: CIoContext()
	{}
	
};

class CBufferedFileIO : public CIoContext
{
protected:
	CReadBuffer m_Buffer;
	CAnsiFile m_File;
	
	QString m_Name; // in/out name

public:
	CBufferedFileIO()
		: CIoContext()
	{}
	
};


#endif // IOCONTEXT_H
