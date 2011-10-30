///////////////////////////////////
//
// XFD-decrunching support,
// helpers and interface for different decrunchers.
//
// This is base-class only for decrunchers,
// using same interface helps sharing code
// and will simplify later if different library-handling is needed
// (such as static linking all together instead of dynamic runtime linking).
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XFDSLAVE_H
#define XFDSLAVE_H

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

#include "AnsiFile.h"

// status bits for operations,
// does this help or is it just simpler to convert immediately..?
//
// only 5 of user-bits cared about,
// rest of can be ignored..? (system-bits)
// (only 16-bit register in M68k,
// all other regs 32-bits).
//
// "ccr" (condition code register)
// refers to lower byte of status register..?
//
// this should be shared regardless of register(s) operated on..?
//
struct ccreg
{
	//union
	//{
		struct ccrbits
		{
			// bits: 
			// upper 3 unused,
			// lower 5 used
			//unsigned e:3; // empty (3 bits)
			unsigned e1:1; // empty1
			unsigned e2:1; // empty2
			unsigned e3:1; // empty3
			unsigned x:1; // extend
			unsigned n:1; // negative
			unsigned z:1; // zero
			unsigned v:1; // overflow
			unsigned c:1; // carry
		} ccr;
		
		// 4       0
		// X N Z V C 
		//uint8_t m_ccr;
	//};

	/*
	const uint8_t MASK_C = 1;
	const uint8_t MASK_V = (1 << 1);
	const uint8_t MASK_Z = (1 << 2);
	const uint8_t MASK_N = (1 << 3);
	const uint8_t MASK_X = (1 << 4);
	*/
	
	void clear()
	{
		ccr.x = 0;
		ccr.n = 0;
		ccr.z = 0;
		ccr.v = 0;
		ccr.c = 0;
	}
	void init(const uint8_t bits)
	{
		ccr.x = (bits & (1<<4)) ? 1 : 0;
		ccr.n = (bits & (1<<3)) ? 1 : 0;
		ccr.z = (bits & (1<<2)) ? 1 : 0;
		ccr.v = (bits & (1<<1)) ? 1 : 0;
		ccr.c = (bits & 1) ? 1 : 0;
	
		//m_ccr = bits;
	}
};

struct statusreg
{
	union
	{
		uint8_t system; // needs supervisor-mode to access (not in user-mode)
		ccreg ccr; // lower byte, condition code register
	};
};


// TODO: register for program counter (pc)?
// some code depends on pc-relative jumps in code,
// is there better way to convert than "emulating" it?
// need some kinda "jump-table" to label/method..?
//
//typedef uint32_t pcreg;

// simplify asm to c conversion,
// for each register with access methods
struct datareg
{
	union
	{
		int8_t b;
		int16_t w;
		int32_t l;
	};
	//} u;
	
	/*	
	// check: does these help
	// should be shared regardless of registers operated on..?
	statusreg s; 
	
	int8_t b() const
	{
		return u.b;
	}
	int16_t w() const
	{
		return u.w;
	}
	int32_t l() const
	{
		return u.l;
	}

	// operator = : move
	// or simpler: move?(const int32_t &v), for example D0.movew(D1)
	
	explicit datareg& operator = (const int8_t &v)
	{
		s.clear();
		u.b = v;
		if (v < 0)
		{
			s.n = true;
		}
		else if (v == 0)
		{
			s.z = true;
		}
		return *this;
	}
	explicit datareg& operator = (const int16_t &v)
	{
		s.clear();
		u.w = v;
		if (v < 0)
		{
			s.n = true;
		}
		else if (v == 0)
		{
			s.z = true;
		}
		return *this;
	}
	explicit datareg& operator = (const int32_t &v)
	{
		s.clear();
		u.l = v;
		if (v < 0)
		{
			s.n = true;
		}
		else if (v == 0)
		{
			s.z = true;
		}
		return *this;
	}
	
	// operator += : add
	explicit datareg& operator += (const int8_t &v)
	{
		s.clear();
		u.l = u.b + v;
		return *this;
	}
	explicit datareg& operator += (const int16_t &v)
	{
		s.clear();
		u.l = u.w + v;
		return *this;
	}
	explicit datareg& operator += (const int32_t &v)
	{
		s.clear();
		u.l = u.l + v;
		return *this;
	}
	
	// operator -= : sub
	explicit datareg& operator -= (const int8_t &v)
	{
		s.clear();
		u.l = u.b - v;
		return *this;
	}
	explicit datareg& operator -= (const int16_t &v)
	{
		s.clear();
		u.l = u.w - v;
		return *this;
	}
	explicit datareg& operator -= (const int32_t &v)
	{
		s.clear();
		u.l = u.l - v;
		return *this;
	}
	
	void lslb(const int32_t &bc)
	{
		s.clear();
		u.b <<= bc;
	}
	void lslw(const int32_t &bc)
	{
		s.clear();
		u.w <<= bc;
	}
	void lsll(const int32_t &bc)
	{
		s.clear();
		u.l <<= bc;
	}

	void lsrb(const int32_t &bc)
	{
		s.clear();
		u.b >>= bc;
	}
	void lsrw(const int32_t &bc)
	{
		s.clear();
		u.w >>= bc;
	}
	void lsrl(const int32_t &bc)
	{
		s.clear();
		u.l >>= bc;
	}
	*/
	
};

// address registers generally need two modes in operations:
// 1) data-operations (value which is pointed to)
// 2) address-operations (change pointer to next element)
//
// for first case, use helper operands.
// for second case, operate directly on the pointer instead.
//
struct addrreg
{
	// actual value of register is just pointer to data:
	// "address-of" storage.
	// data-operations (e.g. (A0)+ ) affect 
	// both pointed data and post-increment of the address,
	// such as: move.l (A0),D0 -> D0.l = A0.l()
	// and: cmp.l D0,(A0)+ -> if (A0.l() == D0.l)
	//
	// address-value operations should use pointer directly from here,
	// such as: cmp.l A1,A0 -> if (A0.src == A1.src)
	// and: movea.l A1,A0 -> A0.src = A1.src 
	//
	//
	uint8_t *src;

	/* no need, use src directly when operating on actual address
	ptrdiff_t operator -(const addrreg& other) const
	{
		return (other.src - src);
	}
	*/

	// need operand size here somewhere..
	// (size of value to copy)
	/*
	addrreg& operator += (const addrreg& other)
	{
	}
	*/
	
	// (An)+,(Am)+ syntax (increment both),
	// value should be "as-is" until value is accessed
	// -> don't swap byteorder unless value is really used also
	void setb(addrreg& other)
	{
		int8_t *p = (int8_t*)src;
		int8_t *o = (int8_t*)other.src;
		src += 1; other.src += 1;
		(*p) = (*o);
	}
	void setw(addrreg& other)
	{
		int16_t *p = (int16_t*)src;
		int16_t *o = (int16_t*)other.src;
		src += 2; other.src += 2;
		(*p) = (*o);
	}
	void setl(addrreg& other)
	{
		int32_t *p = (int32_t*)src;
		int32_t *o = (int32_t*)other.src;
		src += 4; other.src += 4;
		(*p) = (*o);
	}

	
	// Dn,(An)+ syntax, data-value to buffer-position
	// according to size of value
	// -> byteswap also, buffer should always have original byteorder
	void setb(datareg& dreg)
	{
		uint8_t *p = src;
		src += 1; 
		setB(p, dreg.b);
	}
	void setw(datareg& dreg)
	{
		uint8_t *p = src;
		src += 2;
		setW(p, dreg.w);
	}
	void setl(datareg& dreg)
	{
		uint8_t *p = src;
		src += 4; 
		setL(p, dreg.l);
	}
	
	/*
	will it be more trouble to use operators..? 
	(implicit use by mistake..)
	operator int8_t() const
	{
		//int8_t *p = (int8_t*)src;
		return b(0);
	}
	operator int16_t() const
	{
		//int16_t *p = (int16_t*)src;
		return w(0);
	}
	operator int32_t() const
	{
		//int32_t *p = (int32_t*)src;
		return l(0);
	}
	// could be made simpler with these.. ?	
	operator int8_t*() const
	{
		return (int8_t*)src;
	}
	operator int16_t*() const
	{
		return (int16_t*)src;
	}
	operator int32_t*() const
	{
		return (int32_t*)src;
	}
	*/
	
	/**/

	// value access methods:
	//
	// no offset = get element of specified size,
	// increment address also according to size.
	//
	// off = address relative offset,
	// this allows "peeking" by offset
	// without modifying current address.
	//
	int8_t b(void) // with auto-increment
	{
		int8_t *p = src;
		src += 1;
		return *p;
	}
	int8_t b(const size_t off) const
	{
		return *(src + off);
	}

	int16_t w(void) // with auto-increment
	{
		//int16_t *p = (int16_t*)src;
		//src += 2;
		//return *p;
		int8_t *p = src;
		src += 2;
		return getW(p);
	}
	int16_t w(const size_t off) const
	{
		return getW(src + off);
	}
	
	int32_t l(void) // with auto-increment
	{
		//int32_t *p = (int32_t*)src;
		//src += 4;
		//return *p;
		int8_t *p = src;
		src += 4;
		return getL(p);
	}
	int32_t l(const size_t off) const
	{
		return getL(src + off);
	}

	int8_t bM(void) // with auto-decrement
	{
		int8_t *p = src;
		src -= 1;
		return *p;
	}
	int16_t wM(void) // with auto-decrement
	{
		int8_t *p = src;
		src -= 2;
		return getW(p);
	}
	int32_t lM(void) // with auto-decrement
	{
		int8_t *p = src;
		src -= 4;
		return getL(p);
	}
	
	// byteordering helpers (big-endian data)
	uint8_t getB(const uint8_t *pBuf) const
	{
		return (pBuf[0]);
	}
	uint16_t getW(const uint8_t *pBuf) const
	{
		return ((pBuf[0] << 8) + pBuf[1]);
	}
	uint32_t getL(const uint8_t *pBuf) const
	{
		return ((pBuf[0] << 24) + (pBuf[1] << 16) + (pBuf[2] << 8) + pBuf[3]);
	}
	
	// byteordering helpers (big-endian data)
	void setB(uint8_t *pBuf, const uint8_t val) const
	{
		pBuf[0] = val;
	}
	void setW(uint8_t *pBuf, const uint16_t val) const
	{
		//pBuf[0] = ((val & (0xFF << 8)) >> 8);
		pBuf[0] = ((val & 0xFF00) >> 8);
		pBuf[1] = (val & 0xFF);
	}
	void setL(uint8_t *pBuf, const uint32_t val) const
	{
		pBuf[0] = ((val & (0xFF << 24)) >> 24);
		pBuf[1] = ((val & (0xFF << 16)) >> 16);
		pBuf[2] = ((val & (0xFF << 8)) >> 8);
		pBuf[3] = (val & 0xFF);
	}
	
	// constructor
	addrreg()
	 : src(nullptr)
	{}
};

// address register,
// handle addressing modes and value-modes..
/*
class Areg
{
public:
	addrreg m;
	
	datareg& value()
	{
	}
	operator -> ()
	{
		return datareg(m.l)
	}

};
*/

// pure virtual interface 
// for XFD-slave decruncher implementations
//
class XfdSlave
{
protected:
	CReadBuffer *m_pIn;

	// need better way of sharing code..
	uint32_t MakeTag(const char *buf) const
    {
        uint32_t tmp = 0;
        tmp |= (((uint32_t)(buf[3])) << 24);
        tmp |= (((uint32_t)(buf[2])) << 16);
        tmp |= (((uint32_t)(buf[1])) << 8);
        tmp |= ((uint32_t)(buf[0]));
        return tmp;
    }
    
    // some instructions for quick&dirty convert
    // without addressing-mode variations.. (must do manually)
    void moveq(const uint32_t val, datareg &reg) const
    {
		reg.l = val;
    }

	// swap register halves (32-bit swap of 16-bit values)
    void swap(datareg &reg) const
    {
		int16_t tmp = (reg.l & 0xFFFF);
		reg.l = ((tmp << 16) | ((reg.l >> 16) & 0xFFFF));
    }

    void btst(const uint32_t bitix, datareg &reg)
    {
		// if ccr is necessary to keep..?
		ccr.ccr.z = ((reg.l & (1 << bitix)) ? 0 : 1); // isZero
		//return ((reg.l & (1 << bitix)) ? true : false);
    }
    bool bne()
    {
		// "not-equal"
		return ((ccr.ccr.z) ? true : false);
    }


	// simulate "rotate-with-extend" using ccr-bits as buffer,
	// "right-shift" rotate, depends on ccr (init before this)
    void roxr(const int32_t count, datareg &reg)
    {
		for (int32_t i = 0; i < count; i++)
		{
			//ccr.ccr.n = (reg.l & (1 << 31)) ? 1 : 0;
		
			// keep low bit (to be rotated)
			ccr.ccr.c = (reg.l & 1) ? 1 : 0;
			
			// rotate bit
			reg.l >>= 1;

			// highest bit to last rotated out
			reg.l &= ((ccr.ccr.x) << 31);
			ccr.ccr.x = ccr.ccr.c;
		}
    }

	// simulate "rotate-with-extend" using ccr-bits as buffer,
	// "left-shift" rotate, depends on ccr (init before this)
    void roxl(const int32_t count, datareg &reg)
    {
		for (int32_t i = 0; i < count; i++)
		{
			//ccr.ccr.n = (reg.l & (1 << 31)) ? 1 : 0;
		
			// keep high bit (to be rotated)
			ccr.ccr.c = (reg.l & (1 << 31)) ? 1 : 0;
			
			// rotate bit
			reg.l <<= 1;

			// lowest bit to previous rotated out
			reg.l &= ((ccr.ccr.x) ? 1 : 0);
			ccr.ccr.x = ccr.ccr.c;
		}
    }

	// "rotate-right" without extend
    void ror(const int32_t count, datareg &reg)
    {
		for (int32_t i = 0; i < count; i++)
		{
			// keep low bit (to be rotated)
			ccr.ccr.c = (reg.l & 1) ? 1 : 0;
			
			// rotate bit
			reg.l >>= 1;
			
			// highest bit to lowest rotated out
			reg.l &= ((ccr.ccr.c) << 31);
		}
    }

	// "rotate-left" without extend
    void rol(const int32_t count, datareg &reg)
    {
		for (int32_t i = 0; i < count; i++)
		{
			// keep high bit (to be rotated)
			ccr.ccr.c = (reg.l & (1 << 31)) ? 1 : 0;
			
			// rotate bit
			reg.l <<= 1;
			
			// lowest bit to highest rotated out
			reg.l &= ((ccr.ccr.c) ? 1 : 0);
		}
    }

	// less typing this way (instead of R[n]..)
	//
	datareg D0,D1,D2,D3,D4,D5,D6,D7;
	addrreg A0,A1,A2,A3,A4,A5,A6,A7; // SP == A7

	//statusreg sr;
	ccreg ccr; // keep this at least?
	
    XfdSlave(CReadBuffer *pIn)
		: m_pIn(pIn)
		, D0(),D1(),D2(),D3(),D4(),D5(),D6(),D7()
		, A0(),A1(),A2(),A3(),A4(),A5(),A6(),A7()
    {}
public:
    virtual bool decrunch(CReadBuffer *pOut) = 0;
};


////////////////////

#endif // XFDSLAVE_H
