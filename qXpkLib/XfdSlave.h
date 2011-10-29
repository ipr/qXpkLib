///////////////////////////////////
//
// XFD-decrunching support:
// slave-decrunchers for use by XFD-master.
//
// (May be used in XpkMaster when XFD-decruncher is needed.)
//
// Ilkka Prusi
// ilkka.prusi@gmail.com
//

#ifndef XFDSLAVE_H
#define XFDSLAVE_H

// use ISO-standard typedefs (platform-independency)
#include <stdint.h>

#include "AnsiFile.h"

// status bits for operations?
// does this help or is it just simpler to convert immediately..?
//
// only 5 of user-bits cared about,
// 15 system-bit can be ignored..?
//
struct statusreg
{
	bool n; // negative
	bool x; // extended
	bool z; // zero
	bool v; // overflow
	bool c; // carry
	
	void clear()
	{
		n = false;
		x = false;
		z = false;
		v = false;
		c = false;
	}
	
};

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

	// less typing this way (instead of R[n]..)
	//
	datareg D0,D1,D2,D3,D4,D5,D6,D7;
	addrreg A0,A1,A2,A3,A4,A5,A6,A7; // SP == A7
	
    XfdSlave(CReadBuffer *pIn)
		: m_pIn(pIn)
		, D0(),D1(),D2(),D3(),D4(),D5(),D6(),D7()
		, A0(),A1(),A2(),A3(),A4(),A5(),A6(),A7()
    {}
public:
    virtual bool decrunch(CReadBuffer *pOut) = 0;
};

////////// Vice

class XfdVice : public XfdSlave
{
public:
	XfdVice(CReadBuffer *pIn)
		: XfdSlave(pIn)
	{}
    virtual bool decrunch(CReadBuffer *pOut);
};

////////// VDCO (Virtual Dreams)

class XfdVDCO : public XfdSlave
{
public:
	XfdVDCO(CReadBuffer *pIn)
		: XfdSlave(pIn)
	{}
    virtual bool decrunch(CReadBuffer *pOut);
};


////////////////////

#endif // XFDSLAVE_H
