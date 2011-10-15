/////////////////////////////////////
//
// Windows FILETIME-timestamp conversion helper
//
// Ilkka Prusi 2011
//

#ifndef _FILETIMEHELPER_H_
#define _FILETIMEHELPER_H_

#include <stdint.h>

#ifdef _WIN32

// for typedefs used here
#include <Windows.h>

#else

// for compatibility in other platforms
struct FILETIME 
{
    uint32_t dwLowDateTime;
    uint32_t dwHighDateTime;
};

// conversion helpers for alignment
struct ULARGE_PAIR
{
	uint32_t LowPart;
	uint32_t HighPart;
};
union ULARGE_INTEGER 
{
	struct ULARGE_PAIR u;
    uint64_t QuadPart;
};

#endif

class CFiletimeHelper
{
private:
	uint64_t m_u64Stamp;
	
protected:
	uint64_t GetFiletimeValue(const FILETIME &ftStamp) const
	{
		ULARGE_INTEGER ulTemp;
		ulTemp.HighPart = ftStamp.dwHighDateTime;
		ulTemp.LowPart = ftStamp.dwLowDateTime;
		return ulTemp.QuadPart;
	}
	
	FILETIME GetValueAsFiletime(const uint64_t u64Stamp) const
	{
		FILETIME ftStamp;
		ULARGE_INTEGER ulTemp;
		ulTemp.QuadPart = u64Stamp;
		ftStamp.dwHighDateTime = ulTemp.HighPart;
		ftStamp.dwLowDateTime = ulTemp.LowPart;
		return ftStamp;
	}
	
public:
    CFiletimeHelper()
		: m_u64Stamp(0)
	{}
    CFiletimeHelper(const uint64_t u64Stamp)
		: m_u64Stamp(u64Stamp)
	{}
    CFiletimeHelper(const FILETIME &ftStamp)
		: m_u64Stamp(0)
	{
		m_u64Stamp = GetFiletimeValue(ftStamp);
	}
    CFiletimeHelper(const uint32_t ulHiPart, const uint32_t ulLoPart)
		: m_u64Stamp(0)
	{
		FILETIME ft;
		ft.dwHighDateTime = ulHiPart;
		ft.dwLowDateTime = ulLoPart;
		m_u64Stamp = GetFiletimeValue(ft);
	}
	CFiletimeHelper(const CFiletimeHelper &Helper)
		: m_u64Stamp(Helper.GetValue())
	{}
    ~CFiletimeHelper()
	{}
	
	// note: time_t is __time64_t (64-bit) 
	// unless otherwise define'd with _USE_32BIT_TIME_T on Win32
	//
	operator time_t() const
	{
		return (time_t)GetAsUnixTime();
	}

	operator FILETIME() const
	{
		return GetValueAsFiletime(m_u64Stamp);
	}

	bool operator ==(const CFiletimeHelper &Helper) const
	{
		if (this == &Helper)
		{
			return true;
		}
		return (m_u64Stamp == (uint64_t)Helper);
	}
	
	bool operator !=(const CFiletimeHelper &Helper) const
	{
		return (!this->operator ==(Helper));
	}

	/*
	bool operator ==(const FILETIME &ftStamp) const
	{
		return (m_u64Stamp == GetFiletimeValue(ftStamp));
	}
	
	bool operator !=(const FILETIME &ftStamp) const
	{
		return (!this->operator ==(ftStamp));
	}
	*/
	
	CFiletimeHelper& operator =(const FILETIME &ftStamp)
	{
		m_u64Stamp = GetFiletimeValue(ftStamp);
		return *this;
	}

	CFiletimeHelper& operator =(const CFiletimeHelper &Helper)
	{
		// check self-assignment
		if (this == &Helper)
		{
			return *this;
		}
		
		m_u64Stamp = (uint64_t)Helper;
		return *this;
	}

	uint64_t operator +(const CFiletimeHelper &Helper) const
	{
		return (m_u64Stamp + (uint64_t)Helper);
	}
	
	uint64_t operator -(const CFiletimeHelper &Helper) const
	{
		return (m_u64Stamp - (uint64_t)Helper);
	}
	
	uint64_t GetValue() const
	{
		return m_u64Stamp;
	}

	uint64_t GetAsUnixTime() const
	{
		// 1. epoch-conversion
		// 2. hectonanoseconds to seconds
		return ((m_u64Stamp - 116444736000000000ULL) / 10000000ULL);
	}
	
	CFiletimeHelper& SetNow()
	{
		FILETIME ftNow;
		::GetSystemTimeAsFileTime(&ftNow);
		
		m_u64Stamp = GetFiletimeValue(ftNow);
		return *this;
	}
	
};

#endif // _FILETIMEHELPER_H_


