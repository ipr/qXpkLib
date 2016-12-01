//////////////////////////////////////
//
// AnsiFile.h
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef _ANSIFILE_H_
#define _ANSIFILE_H_

#include <cstring>
#include <memory>
#include <string>

class CIOBuffer
{
private:
	enum tSizes
	{
		INITIAL_READ_BUFFER_SIZE = 16384
		//MAX_READ_BUFFER_SIZE = 1024*1024
	};

	unsigned char *m_pReadBuffer;
	size_t m_nReadBufferSize;
	
	//size_t m_nMaxBufferSize; // limit of growing buffer..
	//bool m_bKeepExisting; // keep existing data if growing buffer..
	//bool m_bPageAlign; // page-size aligned allocations

	inline void CreateBuffer(const size_t nMinSize)
	{
		// allocate new
		m_pReadBuffer = new unsigned char[nMinSize];
		m_nReadBufferSize = nMinSize;
		::memset(m_pReadBuffer, 0, m_nReadBufferSize);
	}
	
	inline void GrowBuffer(const size_t nMinSize, bool bKeepData = true)
	{
		if (m_nReadBufferSize >= nMinSize)
		{
			// no change
			return;
		}
		
		size_t nNewSize = nMinSize;
		/* // do we need max size?
		if (nNewSize > m_nMaxBufferSize)
		{
			nNewSize = m_nMaxBufferSize;
		}
		*/
		unsigned char *pNewBuf = new unsigned char[nNewSize];
		if (bKeepData == true)
		{
			::memcpy(pNewBuf, m_pReadBuffer, m_nReadBufferSize); // keep existing data
		}
		delete m_pReadBuffer; // destroy old smaller
		
		// keep new buffer
		m_pReadBuffer = pNewBuf;
		m_nReadBufferSize = nNewSize;
	}
	
protected:
	// helpers for users:
	// user-defined position in buffer
	// (read/write position)
	//
	size_t m_nCurrentPos;

    bool m_bConstBuffer; // disallow alterations (when attached to another), memory-mapped file even..?

	// we could keep amount of buffer actually used
	// here, but accounting for it would be problem
	// in each caller which may use direct buffer access..
	//size_t m_nUsedSize;

public:
	CIOBuffer(void) 
		: m_pReadBuffer(nullptr)
		, m_nReadBufferSize(0)
		, m_nCurrentPos(0)
		, m_bConstBuffer(false)
	{
		CreateBuffer(INITIAL_READ_BUFFER_SIZE);
	}

	CIOBuffer(const size_t nMinsize) 
		: m_pReadBuffer(nullptr)
		, m_nReadBufferSize(0)
		, m_nCurrentPos(0)
		, m_bConstBuffer(false)
	{
		CreateBuffer(nMinsize);
	}

	// "attach" to given buffer or memory-mapped file
	// so same interface can be used
	// TODO: disallow reallocation/growing when attached..
	CIOBuffer(unsigned char *pBuffer, const size_t nBufferSize, bool bKeepConst = true) 
		: m_pReadBuffer(pBuffer)
		, m_nReadBufferSize(nBufferSize)
		, m_nCurrentPos(0)
		, m_bConstBuffer(bKeepConst)
	{}
	
	~CIOBuffer(void) 
	{
		if (m_pReadBuffer != nullptr
			&& m_bConstBuffer == false) // is attached?
		{
			delete m_pReadBuffer;
		}
		m_pReadBuffer = nullptr; // clear in any case
	}

	// allocate or grow if necessary
	void PrepareBuffer(const size_t nMinSize, bool bKeepData = true)
	{
		if (m_bConstBuffer == true)
		{
			// do nothing
			return;
		}
		
		if (m_pReadBuffer == nullptr
			|| m_nReadBufferSize == 0)
		{
			// must create new
			CreateBuffer(nMinSize);
			return;
		}

		// growing buffer if existing is smaller
		GrowBuffer(nMinSize, bKeepData);

		// if we want to clear
		if (bKeepData == false)
		{
			// otherwise just clear existing (keep existing)
			::memset(m_pReadBuffer, 0, m_nReadBufferSize);
			m_nCurrentPos = 0;
		}
	}


	// note: don't make it const:
	// allow modify to read into it..
	//
	unsigned char *GetBegin() const
	{
		return m_pReadBuffer;
	}
	
	// reduce repeated code -> count end ptr
	unsigned char *GetEnd() const
	{
		return m_pReadBuffer + m_nReadBufferSize;
	}

	// reduce repeated code -> count to given offset from start
	unsigned char *GetAt(const size_t nOffset) const
	{
#ifdef _DEBUG
		// debug-case, access beyond buffer
		if (m_nReadBufferSize <= nOffset)
		{
			return nullptr;
		}
#endif	
		return m_pReadBuffer + nOffset;
	}
	
	// inlinable helper
	unsigned char *GetAtCurrent() const
	{
		return GetAt(m_nCurrentPos);
	}

	// inlinable helper to track reads
	unsigned char *GetNext(const size_t nBytes)
	{
		
#ifdef _DEBUG
		//if ((m_nReadBufferSize - m_nCurrentPos) < nBytes)
		
		// better for unsigned variables?
		if ((m_nReadBufferSize - nBytes) < m_nCurrentPos)
		{
			return nullptr;
		}
#endif
		
		unsigned char *pCur = GetAt(m_nCurrentPos);
		m_nCurrentPos += nBytes;
		return pCur;
	}
	
	bool IsEnd() const
	{
		if (m_nCurrentPos < m_nReadBufferSize)
		{
			return false;
		}
		return true;
	}
	
	// current allocation,
	// used size may be less (see current pos)
	size_t GetSize() const
	{
		return m_nReadBufferSize;
	}

	// amount of space after current position
	// (note: we can grow automatically..).
	// note: if caller has different idea of position
	// (not using wrapper-methods)
	// then this will not be usable value (position different)
	//
	size_t GetSpaceSize() const
	{
		return (GetSize() - GetCurrentPos());
	}

	// user-defined position in buffer
	// (read/write position)
	size_t GetCurrentPos() const
	{
		return m_nCurrentPos;
	}
	void SetCurrentPos(const size_t nCurrentPos)
	{
		m_nCurrentPos = nCurrentPos;
	}
	
	// reserve additional space
	bool Reserve(const size_t nSize)
	{
		if (m_bConstBuffer == true)
		{
			// do nothing
			return false;
		}
	
		if (nSize <= (m_nReadBufferSize - m_nCurrentPos))
		{
			// already enough unused space
			return true;
		}
		
		// allocate more, keep existing
		GrowBuffer(m_nReadBufferSize + nSize, true);
		return true;
	}
	
	// copy given, start at current
	bool Append(const unsigned char *pData, const size_t nSize)
	{
		if (m_bConstBuffer == true)
		{
			// do nothing
			return false;
		}
	
	/* if explicit reserve is needed..
#ifdef _DEBUG
		if ((m_nCurrentPos + nSize) > m_nReadBufferSize)
		{
			// exception, access beyond allocated buffer
		}
#endif	
	*/
		if (nSize > GetSpaceSize())
		{
			// not enough space in buffer
			// -> grow buffer, reserve some 
			// extra space for further needs
			GrowBuffer(m_nReadBufferSize + nSize, true);
		}

		unsigned char *pBuf = GetAt(m_nCurrentPos);
		::memcpy(pBuf, pData, nSize);
		m_nCurrentPos += nSize;
		return true;
	}
	
	// helpers for read/write single character:
	// use instead of direct-IO
	unsigned char GetNextByte()
	{
		unsigned char *pBuf = GetAt(m_nCurrentPos);
		m_nCurrentPos++;
		return (*pBuf);
	}
	void SetNextByte(const unsigned char ucValue)
	{
		/*
		if (GetSpaceSize() < 1)
		{
			// grow&keep existing
			// TODO: should allocate more at once for better efficiency..
			GrowBuffer(m_nReadBufferSize + 1, true);
		}
		*/
	
		unsigned char *pBuf = GetAt(m_nCurrentPos);
		(*pBuf) = ucValue;
		m_nCurrentPos++;
	}
	
	// move buffer contents to beginning
	bool MoveToBegin(const size_t nStartOffset)
	{
		if (m_bConstBuffer == true)
		{
			// do nothing
			// or just adjust pointer?
			// (when attached, we don't need to destroy it..)
			//m_pReadBuffer = GetAt(nStartOffset);
			return false;
		}
	
		// should support overlapped IO..
		::memmove(GetBegin(), 
				GetAt(nStartOffset), 
				m_nReadBufferSize - nStartOffset);
		m_nCurrentPos -= nStartOffset;
		
		// TODO: we might want actual amount of buffer used
		// in this but might not know it depending on how caller is using buffer..
		return true;
	}
};

// for backwards-compatibility until replaced
class CReadBuffer : public CIOBuffer
{};

// ANSI-C style file-API helper
class CAnsiFile
{
protected:
	FILE *m_pFile;
	size_t m_nFileSize;
	bool m_bIsWritable;

	size_t GetSizeOfFile()
	{
		if (m_pFile == NULL)
		{
			return 0;
		}

		// should be at start (0)
		long lCurrent = ftell(m_pFile);

		// locate end
		if (fseek(m_pFile, 0L, SEEK_END) != 0)
		{
			return 0;
		}
		size_t nSize = ftell(m_pFile);

		// return to start
		fseek(m_pFile, lCurrent, SEEK_SET);
		return nSize;
	}

public:
	CAnsiFile(void)
		: m_pFile(NULL)
		, m_nFileSize(0)
		, m_bIsWritable(false)
	{}
	CAnsiFile(const std::string &szFile, bool bWritable = false)
		: m_pFile(NULL)
		, m_nFileSize(0)
		, m_bIsWritable(bWritable)
	{
		Open(szFile, bWritable);
	}
	~CAnsiFile(void)
	{
		Close();
	}

	operator FILE *() const
	{
		return m_pFile;
	}

	bool Open(const std::string &szFile, bool bWritable = false)
	{
		Close(); // close previous (if any)

		if (bWritable == false)
		{
			m_pFile = fopen(szFile.c_str(), "rb");
			m_nFileSize = GetSizeOfFile();
		}
		else
		{
			// size zero if new file..
			m_pFile = fopen(szFile.c_str(), "wb");
		}
		m_bIsWritable = bWritable;
		return IsOk();
	}

	void Close()
	{
		if (m_pFile != NULL)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}
	}
	
	// user may want to check..?
	bool IsOpen()
	{
		if (m_pFile != NULL)
		{
			return true;
		}
		return false;
	}

	size_t GetSize()
	{
		// get size as it was in start
		if (m_bIsWritable == false)
		{
			return m_nFileSize;
		}
		// determine current size
		return GetSizeOfFile();
	}

	bool IsOk()
	{
		if (m_pFile == NULL)
		{
			return false;
		}

		if (ferror(m_pFile) != 0) 
		{
			return false;
		}
		return true;
	}

	bool Flush()
	{
		if (fflush(m_pFile) != 0)
		{
			return false;
		}
		return true;
	}

	bool Write(const void *pBuffer, const size_t nBytes)
	{
		size_t nWritten = fwrite(pBuffer, 1, nBytes, m_pFile);
		if (IsOk() == false
			|| nWritten < nBytes)
		{
			return false;
		}
		return Flush();
	}

	bool Read(void *pBuffer, const size_t nBytes)
	{
		size_t nRead = fread(pBuffer, 1, nBytes, m_pFile);
		if (IsOk() == false
			|| nRead < nBytes)
		{
			return false;
		}
		return true;
	}

	/*
	bool Read(CIOBuffer &Buffer)
	{
		return Read(Buffer.GetBegin(), Buffer.GetSize());
	}
	*/
	
	bool Tell(long &lPos)
	{
		lPos = ftell(m_pFile);
		if (lPos < 0)
		{
			return false;
		}
		return true;
	}

	bool Seek(const size_t nBytes, const int iOrigin)
	{
		if (fseek(m_pFile, nBytes, iOrigin) != 0)
		{
			return false;
		}
		return true;
	}
};


#endif // ifndef _ANSIFILE_H_

