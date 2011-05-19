//////////////////////////////////////
//
// AnsiFile.h
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef _ANSIFILE_H_
#define _ANSIFILE_H_

#include <string>
#include <exception>

// exception-classes for error cases
class IOException : public std::exception
{
public:
	IOException(const char *szMessage)
		: std::exception(szMessage)
	{
	}
};

class ArcException : public std::exception
{
protected:
	std::string m_szData;
public:
	ArcException(const char *szMessage, const std::string &szData)
		: std::exception(szMessage)
		, m_szData(szData)
	{
	}
	ArcException(const char *szMessage, const size_t nData)
		: std::exception(szMessage)
		, m_szData()
	{
		// TODO:
		//m_szData = std::lexical_cast<std::string>(nData);
	}
	std::string GetData()
	{
		return m_szData;
	}
};


class CReadBuffer
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

public:
	CReadBuffer(void) 
		: m_pReadBuffer(nullptr)
		, m_nReadBufferSize(0)
		, m_nCurrentPos(0)
	{
		CreateBuffer(INITIAL_READ_BUFFER_SIZE);
	}
	
	CReadBuffer(const size_t nMinsize) 
		: m_pReadBuffer(nullptr)
		, m_nReadBufferSize(0)
		, m_nCurrentPos(0)
	{
		CreateBuffer(nMinsize);
	}
	
	~CReadBuffer(void) 
	{
		if (m_pReadBuffer != nullptr)
		{
			delete m_pReadBuffer;
		}
	}

	// allocate or grow if necessary
	void PrepareBuffer(const size_t nMinSize, bool bKeepData = true)
	{
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
		}
	}


	// note: don't make it const:
	// allow modify to read into it..
	//
	unsigned char *GetBegin()
	{
		return m_pReadBuffer;
	}
	
	// reduce repeated code -> count end ptr
	unsigned char *GetEnd()
	{
		return m_pReadBuffer + m_nReadBufferSize;
	}

	// reduce repeated code -> count to given offset from start
	unsigned char *GetAt(const size_t nOffset)
	{
		return m_pReadBuffer + nOffset;
	}
	
	// inlinable helper
	unsigned char *GetAtCurrent()
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
			return true;
		}
		return false;
	}
	
	// current allocation,
	// used size may be less (see current pos)
	size_t GetSize() const
	{
		return m_nReadBufferSize;
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
	
	// copy given, start at current
	bool Append(const unsigned char *pData, const size_t nSize)
	{
		if (nSize > (m_nReadBufferSize - m_nCurrentPos))
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
};


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

class CPathHelper
{
public:
	static bool MakePath(const std::string &szPath);
	static bool MakePathToFile(const std::string &szOutFile);
};

#endif // ifndef _ANSIFILE_H_

