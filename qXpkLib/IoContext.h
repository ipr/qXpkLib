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
#include "arcexception.h"

#include "XpkCapabilities.h"
#include "XpkProgress.h"


// abstract interface only 
// (add virtual methods..)
//
class CIoContext
{
protected:
    bool m_isReadOnly;

public:
    CIoContext(bool isReadOnly = true)
        : m_isReadOnly(isReadOnly)
	{}
	virtual ~CIoContext(void)
	{}
	
	virtual size_t getFullSize() = 0;

	// give same access to data always
	virtual CIOBuffer *getBuffer() = 0;

	// access specific part (get "view of segment" on "view of file"),
	// this might need improvements later..
	virtual CIOBuffer *getBufferAtOffset(const size_t nOffset) = 0;

	// associated name (if any)
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
	CIOBuffer *m_pAttach;
	
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
		m_pAttach = new CIOBuffer(m_pView, m_fileSize, true);
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
	virtual ~CMemoryMappedIO(void)
	{
		clear();
	}

	virtual size_t getFullSize()
	{
		return m_fileSize;
	}
	virtual CIOBuffer *getBuffer()
	{
		m_pAttach->SetCurrentPos(0); // always reset on access?
		return m_pAttach;
	}
	
	virtual CIOBuffer *getBufferAtOffset(const size_t nOffset)
	{
		// create accessor for caller (wrap part of mapped file..)
		return new CIOBuffer(m_pAttach->GetAt(nOffset), m_pAttach->GetSize()-nOffset, true);
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
	CIOBuffer *m_pBuffer;

public:
	// "attach" only
	CBufferIO(CIOBuffer *buffer)
		: CIoContext()
		, m_pBuffer(buffer)
	{}
	virtual ~CBufferIO(void)
	{}
	
	virtual size_t getFullSize()
	{
		return m_pBuffer->GetSize();
	}
	virtual CIOBuffer *getBuffer()
	{
		//m_pBuffer->SetCurrentPos(0); // always reset on access?
		return m_pBuffer;
	}
	
	virtual CIOBuffer *getBufferAtOffset(const size_t nOffset)
	{
		// create accessor for caller (wrap part of buffer..)
		return new CIOBuffer(m_pBuffer->GetAt(nOffset), m_pBuffer->GetSize()-nOffset, true);
	}
	
	virtual QString getName()
	{
		// unnamed
		return "";
	}
	
	virtual void write(const size_t chunkSize)
	{
		// do nothing: collect into buffer only
	}
};

////////////

// need to use buffered-IO for output,
// libraries may need very small output
// and it's generally better to use larger chunks anyway..
// MMIO isn't best choice in this case.
//
class CBufferedFileIO : public CIoContext
{
protected:
	CIOBuffer m_Buffer;
	CAnsiFile m_File;
	
	QString m_Name; // in/out name

public:
    CBufferedFileIO(QString &Name, size_t nBufferSize = 1024)
	    : CIoContext()
	    , m_Buffer(nBufferSize) // prepare minimum, grow when needed
	    , m_File()
        , m_Name(Name)
    {}
	virtual ~CBufferedFileIO(void)
	{
		m_File.Close();
	}
	
	virtual size_t getFullSize()
	{
		return m_File.GetSize();
	}
	virtual CIOBuffer *getBuffer()
	{
		//m_Buffer.SetCurrentPos(0); // always reset on access?
		return &m_Buffer;
	}
	virtual CIOBuffer *getBufferAtOffset(const size_t nOffset)
	{
		// also allow user to modify existing buffer via this "view" ?
		return new CIOBuffer(m_Buffer.GetAt(nOffset), m_Buffer.GetSize()-nOffset, false);
	}
	virtual QString getName()
	{
		return m_Name;
	}
	
	// write output to file as single buffered chunk of data
	// after decrunching.
	// TODO: multiple chunks for very large files,
	// 
    virtual void write(const size_t chunkSize);
};

// TODO: something like this for multi-file/multi-volume IO ?
/*
class CVolumeIO : public CIoContext
{
public:
	QString m_Path; // or file pattern?
	
	std::vector<CIoContext*> m_IO;
	
	CIoContext *addIO();
	CIoContext *getIO();

};
*/

#endif // IOCONTEXT_H
