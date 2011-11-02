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
public:
    CIoContext(void)
	{}
	
	virtual size_t getFullSize() = 0;

	// give same access to data always
	virtual CReadBuffer *getBuffer() = 0;

	// access specific part (get "view of segment" on "view of file"),
	// this might need improvements later..
	virtual CReadBuffer *getBufferOffset(const size_t nOffset) = 0;

	virtual QString getName() = 0;
	
	// on chunk/file decrunching, write to destination (if suitable)
	virtual void write(const size_t chunkSize) {}
};

////////////

// memory-mapping, file as memory-region:
// - OS handles buffering, reads page of file on pagefault (access to region)
// - requires virtual memory/pagefault support, without you're screwed:
// what OS does _not_ support it nowadays?? it's simple MMU-stuff basically..
//
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

	virtual size_t getFullSize()
	{
		return m_fileSize;
	}
	virtual CReadBuffer *getBuffer()
	{
		return m_pAttach;
	}
	
	virtual CReadBuffer *getBufferOffset(const size_t nOffset)
	{
		// create accessor for caller (wrap part of mapped file..)
		return new CReadBuffer(m_pAttach->GetAt(nOffset), m_pAttach->GetSize()-nOffset, true);
	}
	
	virtual QString getName()
	{
		return m_Name;
	}
	
	virtual void write(const size_t chunkSize)
	{
		// read-only for now -> do nothing yet
	}
	
};

////////////

// decrunching to/from user-given buffer only 
// (no actual file read/write)
//
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
	
	virtual size_t getFullSize()
	{
		return m_pBuffer->GetSize();
	}
	virtual CReadBuffer *getBuffer()
	{
		return m_pBuffer;
	}
	
	virtual CReadBuffer *getBufferOffset(const size_t nOffset)
	{
		// create accessor for caller (wrap part of buffer..)
		return new CReadBuffer(m_pBuffer->GetAt(nOffset), m_pBuffer->GetSize()-nOffset, true);
	}
	
	virtual QString getName()
	{
		// unnamed
		return "";
	}
	
};

////////////

/* meh, drop this, too much bother to finish..
just use memory-mapped IO or buffer and get it done
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
	
	virtual size_t getFullSize()
	{
		return m_File.GetSize();
	}
	virtual CReadBuffer *getBuffer()
	{
		return &m_Buffer;
	}
	virtual QString getName()
	{
		return m_Name;
	}
	
	CAnsiFile *GetFile() // should not need this
	{
		return &m_File;
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
*/

#endif // IOCONTEXT_H
