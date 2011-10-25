/*
 * "generic time" format handling wrapper,
 * parse timestamp to more usable Unix-style time_t format.
 *
 * same format is used in various archivers (Lha, ZOO, ARJ..)
 * in essentially same way
 */

#ifndef GENERICTIME_H
#define GENERICTIME_H

/*
 * Generic (MS-DOS style) time stamp format (localtime):
 *
 *  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16
 * |<---- year-1980 --->|<- month ->|<--- day ---->|
 *
 *  15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0
 * |<--- hour --->|<---- minute --->|<- second/2 ->|
 *
 */
/*
  The time/date stamp (bytes $000F-$0012), is broken down as follows:

      Bytes:$000F-0010: Time of last modification:
                        BITS  0- 4: Seconds divided by 2
                                    (0-58, only even numbers)
                        BITS  5-10: Minutes (0-59)
                        BITS 11-15: Hours (0-23, no AM or PM)
      Bytes:$0011-0012: Date of last modification:
                        BITS  0- 4: Day (1-31)
                        BITS  5- 9: Month (1-12)
                        BITS 10-15: Year minus 1980
*/

#include <stdint.h>
#include <time.h>

class CGenericTime
{
protected:
	time_t m_time;
	
	// inline conversion helpers
	inline int subbits(int n, int off, int len) const
	{
		return (((n) >> (off)) & ((1 << (len))-1));
	}

	// note: time_t is __time64_t (64-bit) 
	// unless otherwise define'd with _USE_32BIT_TIME_T on Win32
	
	inline time_t toUnixtime(const uint16_t date, const uint16_t time) const
	{
		struct tm tmp;
	
	    tmp.tm_year  = ((date >>  9) & 0x7f) + 80;
	    tmp.tm_mon   = ((date >>  5) & 0x0f) - 1;
	    tmp.tm_mday  = ((date      ) & 0x1f);
	    
		tmp.tm_hour  = ((time >> 11) & 0x1f);
		tmp.tm_min   = ((time >>  5) & 0x3f);
		tmp.tm_sec   = ((time      ) & 0x1f) * 2;
		
		tmp.tm_isdst = -1;
	
		return mktime(&tmp);
	}
	
	inline time_t toUnixtime(const long t) const
	{
		struct tm tmp;
	
		tmp.tm_sec  = subbits(t,  0, 5) * 2;
		tmp.tm_min  = subbits(t,  5, 6);
		tmp.tm_hour = subbits(t, 11, 5);
		
		tmp.tm_mday = subbits(t, 16, 5);
		tmp.tm_mon  = subbits(t, 21, 4) - 1;
		tmp.tm_year = subbits(t, 25, 7) + 80;
		
		tmp.tm_isdst = -1;
	
		return mktime(&tmp);
	}
	
public:
	CGenericTime(void)
		: m_time(0)
	{}
	CGenericTime(const uint16_t date, const uint16_t time)
		: m_time(0)
	{
		m_time = toUnixtime(date, time);
	}
	CGenericTime(const long lTime)
		: m_time(0)
	{
		m_time = toUnixtime(lTime);
	}
	CGenericTime(const CGenericTime &Helper)
		: m_time((time_t)Helper)
	{}
	
	operator time_t () const
	{
		return m_time;
	}

	CGenericTime& operator =(const CGenericTime &Helper)
	{
		// check self-assignment
		if (this == &Helper)
		{
			return *this;
		}
		
		m_time = (time_t)Helper;
		return *this;
	}
};

#endif // GENERICTIME_H
