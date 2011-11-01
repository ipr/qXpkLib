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
// IoContext:
// Purpose of this is to keep track 
// of how much is read/written to/from input/output,
// this should be generic so that "sub-master" instances
// and extension libraries don't need to repeat same stuff..
// Also try to reduce re-opening files etc.
//

#ifndef IOCONTEXT_H
#define IOCONTEXT_H

#include <QObject>
#include <QString>
#include <QFile> // cross-platform mmap() implementation

#include "AnsiFile.h"

#include "XpkCapabilities.h"
#include "XpkProgress.h"


// abstract interface only 
// (add virtual methods..)
//
class CIoContext
{
protected:
	size_t m_nFileOffset; 
	
public:
    CIoContext(void)
	    : m_nFileOffset(0)
	{}
	
};

// refactoring

class CMemoryMappedIO : public CIoContext
{
protected:
	QString m_Name; // in/out name
	QFile *m_pFile;
	qint64 m_fileSize; // total size of file
	uchar *m_pView; // mapped view
	
	// for "attaching" to buffer:
	// give same helper-interface to sub-libs always
	CReadBuffer *m_pAttach;
	
	bool init()
	{
		m_pFile = new QFile(m_Name);
		if (m_pFile->open(QIODevice::ReadOnly) == false)
		{
			// failed opening file
			return false;
		}
		m_fileSize = m_pFile->size();
		m_pView = m_pFile->map(0, m_fileSize);
		if (m_pView == NULL) // map() uses old-style defs
		{
			// failed mapping view
			return false;
		}
		m_pAttach = new CReadBuffer(m_pView, m_fileSize, true);
		return true;
	}
	void clear()
	{
		if (m_pAttach != nullptr)
		{
			delete m_pAttach;
			m_pAttach = nullptr;
		}
		if (m_pFile != nullptr)
		{
			m_pFile->close();
			delete m_pFile;
			m_pFile = nullptr;
		}
		m_fileSize = 0;
		m_pView = nullptr;
	}

public:
    CMemoryMappedIO(QString &Name)
	    : CIoContext()
	    , m_Name(Name)
	    , m_pFile(nullptr)
	    , m_fileSize(0)
	    , m_pView(nullptr)
	    , m_pAttach(nullptr)
	{
		if (init() == false)
		{
			// detect if could not init
			clear();
		}
	}
	~CMemoryMappedIO()
	{
		clear();
	}
	QString getName() const
	{
		return m_Name;
	}
	// give same access to data always
	CReadBuffer *GetBuffer()
	{
		return m_pAttach;
	}
	void Read() // whole file
	{
		m_nFileOffset = m_fileSize;
	}
	void Read(const size_t nMaxRead) // chunk of data
	{
		size_t remaining = 0;
		if (m_fileSize > m_nFileOffset)
		{
			remaining = m_fileSize - m_nFileOffset;
		}
		size_t nReadSize = (nMaxRead < remaining) ? nMaxRead : remaining;
		m_nFileOffset += nReadSize;
		m_pAttach->SetCurrentPos(m_nFileOffset);
	}
	
};

class CBufferIO : public CIoContext
{
protected:
	CReadBuffer *m_pBuffer;

public:
	// "attach" only
	CBufferIO(CReadBuffer *buffer)
		: CIoContext()
		, m_pBuffer(buffer)
	{}
	QString getName() const
	{
		// unnamed
		return "";
	}
	CReadBuffer *GetBuffer()
	{
		return m_pBuffer;
	}
	
};

class CBufferedFileIO : public CIoContext
{
protected:
	CReadBuffer m_Buffer;
	CAnsiFile m_File;
	
	QString m_Name; // in/out name

public:
    CBufferedFileIO(QString &Name, size_t nBufferSize = 1024)
	    : CIoContext()
	    , m_Name(Name)
	    , m_Buffer(nBufferSize)
	    , m_File()
	    , m_nFileOffset(0)
	{}
	CReadBuffer *GetBuffer()
	{
		return &m_Buffer;
	}
	CAnsiFile *GetFile()
	{
		return &m_File;
	}
	QString getName() const
	{
		return m_Name;
	}

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


#endif // IOCONTEXT_H
