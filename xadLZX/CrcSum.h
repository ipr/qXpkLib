//////////////////////////////////////
//
// CrcSum.h
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//
// Based on: 
// unlzx.c 1.1 (03.4.01, Erik Meusel)
//


#ifndef CRCSUM_H
#define CRCSUM_H

class CRCSum
{
protected:
	static const unsigned int m_crc_table[256];
	
	// calculated CRC-sum
	unsigned int m_sum;
	
public:
	CRCSum(void) 
		: m_sum(0)
	{}

	// get final sum
	unsigned int GetSum() const
	{
		return m_sum;
	}
	
	// if same instance is reused..
	void ResetSum()
	{
		m_sum = 0;
	}
	
	/* Possible problems with 64 bit machines here. It kept giving warnings   */
	/* for people so I changed back to ~.                                     */
	inline void crc_calc(const unsigned char *memory, const unsigned int length)
	{
		// putting more likely case first seems to be 
		// most efficient for CPU branch-prediction 
		// when 'likely' keyword isn't supported..
		// -> changed back
		if (length)
		{
			unsigned int tmplen = length;
			unsigned int temp = ~m_sum; /* was (sum ^ 4294967295) */
			do
			{
				temp = m_crc_table[((*memory) ^ temp) & 255] ^ (temp >> 8);
				memory++;
			} while(--tmplen);
			m_sum = ~temp; /* was (temp ^ 4294967295) */
		}
	}
};


#endif // CRCSUM_H
