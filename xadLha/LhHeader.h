//////////////////////////////////
//
// structure and code for handling archive-file header:
// there are various different headers and extensions
//
// Ilkka Prusi 2011
//

#ifndef LZHEADER_H
#define LZHEADER_H

#include <QObject>
#include <QString>
#include <QByteArray>
#include <QTextCodec>
#include <QList>
#include <QDateTime>

//#include <QDebug> // test

// use standard typedefs where it matters
#include <stdint.h>

#include "AnsiFile.h"
#include "LhaTypeDefs.h"

#include "crcio.h"

// timestamp helpers
#include "GenericTime.h"
#include "FiletimeHelper.h"

// misc. helper-structures for file-modes
#include "FilemodeFlags.h"

// renamed for clarity..
#define PACKMETHOD_TYPE_LENGTH ((int)5)

// OS type identification (user information mostly)
// VC++ 2010 does not yet support fully 'enum class' it seems,
// should allow this: 
//
// enum class EOSType : unsigned char 
//
// TODO: enable when C++11 compilers mature..
// that would give us proper namespacing of enums
//
enum EOSType : unsigned char
{
	EXTEND_GENERIC   = 0,
	EXTEND_UNIX      = 'U',
	EXTEND_AMIGA     = 'A',
	EXTEND_MACOS     = 'm',
	EXTEND_MSDOS     = 'M',
	EXTEND_OS9       = '9',
	EXTEND_OS2       = '2',
	EXTEND_OS68K     = 'K',
	EXTEND_OS386     = '3', // OS-9000??? 
	EXTEND_HUMAN     = 'H', // Human68k
	EXTEND_CPM       = 'C', // CP/M (PMarc?)
	EXTEND_FLEX      = 'F',
	EXTEND_RUNSER    = 'R',
	EXTEND_TOWNSOS   = 'T',
	EXTEND_XOSK      = 'X', // OS-9 for X68000 (?)
	EXTEND_JAVA      = 'J',
	EXTEND_WinNT     = 'W',
	EXTEND_W9598     = 'w'
};

// Unix-like values in headers
// (extended values)
//
class LzUnixHeader 
{
public:
	LzUnixHeader()
		: os_type(EXTEND_UNIX)
		, minor_version(0)
		, UnixMode()
		, unix_gid(0)
		, unix_uid(0)
		, unix_user()
		, unix_group()
	{}
	
    unsigned char   os_type; /* OS type, single character, used in extensions */
    unsigned char   minor_version; // only when EXTEND_UNIX
	
	UnixModeFlags   UnixMode;
    unsigned short  unix_uid;
    unsigned short  unix_gid;
	QString         unix_user;
	QString         unix_group;
};

// msdos and other crap like that..
class LzDosHeader 
{
public:
	LzDosHeader()
		: msdos_attributes()
	{}
	
	// MS-DOS attribute-flags
    MsdosFlags      msdos_attributes;
};


class LzHeader 
{
protected:
	// note: only 16-bit crc..
    unsigned int    file_crc;   // file CRC, 0 if not found in file
    bool            has_crc;    // if file CRC was given/found in file
    unsigned int    header_crc; // header CRC, in extended-header only

public:
	// constructor
	LzHeader()
		: file_crc(0)
		, has_crc(false)
		, header_crc(0)
		, m_enCompression(LZ_UNKNOWN)
		, pack_method()
		, header_size(0)
		, extend_size(0)
		, size_field_length(0)
		, packed_size(0)
		, original_size(0)
		, header_level(0)
		, lf_packedsize(0)
		, lf_originalsize(0)
		, header_pos(0)
		, data_pos(0)
		, os_type(EXTEND_UNIX)
		, filename()
		, dirname()
		, realname()
		, file_comment()
		, m_pUnixHeader(nullptr)
		, m_pDosHeader(nullptr)
	{}
	// destructor
	~LzHeader()
	{
		if (m_pUnixHeader != nullptr)
		{
			delete m_pUnixHeader;
		}
		if (m_pDosHeader != nullptr)
		{
			delete m_pDosHeader;
		}
		m_pUnixHeader = nullptr;
		m_pDosHeader = nullptr;
	}

    tCompressionMethod m_enCompression; // enumerated compression of entry
	QString         pack_method; // method as-is from file, e.g. -lh0-..-lh7-, -lhd-, -lzs-, -lz5-, -lz4-
	
    size_t          header_size; // size of header (level 0/1/2/3)
	long            extend_size; // size of extended-header (if any)
    int             size_field_length; // "size" field may have different sizes (extended headers)
	
    size_t          packed_size; // compressed size of entry
    size_t          original_size; // uncompressed size of entry
    unsigned char   header_level; // level/type of header (0..3)
    
    uint64_t		lf_packedsize; // if 64-bit filesize is given, compressed size of file
    uint64_t		lf_originalsize; // if 64-bit filesize is given, uncompressed size of file

	// keep offset of data in file for locating later..
	long            header_pos;
	long            data_pos;
    
    unsigned char   os_type; /* OS type, single character, used in extensions */

	// note on paths: directory maybe given in filename or it may be given separately,
	// either way we will join directoy to filename and keep full path in it finally.
	QString         filename;
	QString         dirname; // temp until header for entry handled
	QString         realname; // real name for symbolic link (unix)
	
	QString			file_comment; // usually Amiga-packed files have comment or found in extended header

    QDateTime       creation_stamp;
    QDateTime       last_modified_stamp; // generic
    QDateTime       last_access_stamp;
    
    LzUnixHeader	*m_pUnixHeader;

    LzDosHeader		*m_pDosHeader;

	// just to see easily where it is modified
	void setFileCrc(const uint16_t crc)
	{
		has_crc = true;
		file_crc = crc;
	}
	uint16_t getFileCrc() const
	{
		// should be zero if not found from file
		return file_crc;
	}
	bool isFileCrc(const uint16_t crc) const
	{
		if (has_crc == true && file_crc != crc)
		{
			// crc was found but is different from given
			// -> error somewhere
			return false;
		}
		// equal or not found in file
		// -> ok
		return true;
	}
	void setHeaderCrc(const uint16_t crc)
	{
		header_crc = crc;
	}
	bool isHeaderCrc(const uint16_t crc) const
	{
		if (header_crc == crc)
		{
			// match -> ok
			return true;
		}
		// does not match
		return false;
	}
	
	bool isDir()
	{
		if (m_pUnixHeader != nullptr)
		{
			return m_pUnixHeader->UnixMode.isDir;
		}
		if (m_enCompression == LZHDIRS_METHOD_NUM)
		{
			return true;
		}
		return false;
	}
	
	// helper for adding unix-related values
	// into header information
	//
	LzUnixHeader *GetUnixHeader()
	{
		if (m_pUnixHeader != nullptr)
		{
			return m_pUnixHeader;
		}
		m_pUnixHeader = new LzUnixHeader();
		return m_pUnixHeader;
	}

	LzDosHeader *GetDosHeader()
	{
		if (m_pDosHeader != nullptr)
		{
			return m_pDosHeader;
		}
		m_pDosHeader = new LzDosHeader();
		return m_pDosHeader;
	}

	// get suitable method for extraction:
	// check string if it is supported.
	//
	tCompressionMethod GetMethod() const
	{
		if (pack_method.contains("-lh") == true
			&& pack_method.at(4) == '-')
		{
			// -lh?-
			// LHarc -> LZHUFF
			const char level = pack_method.at(3).toAscii();
			switch (level)
			{
			case '0':
				return LZHUFF0_METHOD_NUM;
			case '1':
				return LZHUFF1_METHOD_NUM;
			case '2':
				return LZHUFF2_METHOD_NUM;
			case '3':
				return LZHUFF3_METHOD_NUM;
			case '4':
				return LZHUFF4_METHOD_NUM;
			case '5':
				return LZHUFF5_METHOD_NUM;
			case '6':
				return LZHUFF6_METHOD_NUM;
			case '7':
				return LZHUFF7_METHOD_NUM;
				
			case 'd': // -lhd-
				return LZHDIRS_METHOD_NUM;
				
				/*
				// Joe Jared extensions
			case '8': // -lh8-, same as -lh7-?
				return LZHUFF7_METHOD_NUM;
			case '9': // -lh9-
			case 'a': // -lha-
			case 'b': // -lhb-
			case 'c': // -lhc-
			case 'e': // -lhe-
				break;
				*/
				
				/*
				// UNLHA32
			case 'x': // -lhx-
				// test-compress only?
				// dictionary size 128k-512k
				break;
				*/
			}
		}
		else if (pack_method.contains("-lz") == true
			&& pack_method.at(4) == '-')
		{
			// -lz?-
			// LArc extensions
			const char level = pack_method.at(3).toAscii();
			switch (level)
			{
			case 's': // -lzs-
				return LARC_METHOD_NUM;
			case '5': // -lz5-
				return LARC5_METHOD_NUM;
			case '4': // -lz4-
				return LARC4_METHOD_NUM;

				/*
				// LArc extensions
			case '2': // -lz2-
				// extended -lzs-
			case '3': // -lz3-
			case '7': // -lz7-
			case '8': // -lz8-
				break;
				*/
			}
		}
		/*
		// CP/M PMarc variations
		else if (pack_method == "-pc1-")
		{
			// PopCom compressed executable archive
		}
		else if (pack_method == "-pm0-")
		{
			// no compression, store-only
		}
		else if (pack_method == "-pm1-")
		{
		}
		else if (pack_method == "-pm2-")
		{
		}
		else if (pack_method == "-pms-")
		{
			// PMarc self-extracting
		}
		*/

		// fallthrough: unknown/unsupported
		return LZ_UNKNOWN;
	}
	
	// this is stuff for user-display
	// -> make it clear what one-character codes mean
	QString GetOSTypeName() const
	{
		if (os_type == EXTEND_GENERIC)
		{
			return QString("Generic");
		}
	
		QString type = " (" + QString((char)os_type) + ")"; // for logging
		switch (os_type)
		{
		case EXTEND_UNIX:
			return QString("Unix" + type);
		case EXTEND_AMIGA:
			// unofficial?
			return QString("Amiga" + type);
		case EXTEND_MACOS:
			return QString("MacOS" + type);
		case EXTEND_MSDOS:
			return QString("MSDOS" + type);
		case EXTEND_OS2:
			return QString("OS/2" + type);
		case EXTEND_OS9:
			return QString("OS-9" + type);
		case EXTEND_OS68K:
			return QString("OS-9/68K" + type);
		case EXTEND_OS386:
			// OS-9000 ?? 
			return QString("OS-386" + type);
		case EXTEND_HUMAN:
			return QString("Human68K" + type);
		case EXTEND_CPM:
			return QString("CP/M" + type);
		case EXTEND_FLEX:
			return QString("FLEX" + type);
		case EXTEND_RUNSER:
			return QString("Runser" + type);
		case EXTEND_TOWNSOS:
			// not official..
			return QString("Townsos" + type);
		case EXTEND_XOSK:
			// OS-9 for X68000 ? not official..?
			return QString("XOSK" + type);
		case EXTEND_JAVA:
			return QString("Java" + type);
		case EXTEND_WinNT:
			return QString("Windows NT" + type);
		case EXTEND_W9598:
			return QString("Windows 95/98" + type);
		default:
			// silence some compilers
			break;
		}
		// fallthrough
		return QString("Unknown" + type);
	}
};


// indices of values of header in file
enum tHeaderIndices
{
	I_HEADER_SIZE          = 0,     /* level 0,1,2   */
	I_HEADER_CHECKSUM      = 1,     /* level 0,1     */
	I_METHOD               = 2,     /* level 0,1,2,3 */
	I_PACKED_SIZE          = 7,     /* level 0,1,2,3 */
	I_ATTRIBUTE            = 19,    /* level 0,1,2,3 */
	I_HEADER_LEVEL         = 20,    /* level 0,1,2,3 */
	COMMON_HEADER_SIZE     = 21,    /* size of common part */
	I_GENERIC_HEADER_SIZE  = 24,    /* + name_length */
	I_LEVEL0_HEADER_SIZE   = 36,    /* + name_length (unix extended) */
	I_LEVEL1_HEADER_SIZE   = 27,    /* + name_length */
	I_LEVEL2_HEADER_SIZE   = 26,    /* + padding */
	I_LEVEL3_HEADER_SIZE   = 32
};

// extended header attributes:
// attribute-type code of information in exteded-header
//
enum tExtendedAttribs
{
	EXTH_CRC      = 0x00, // 16-bit CRC
	EXTH_FILENAME = 0x01, // file name
	EXTH_PATHNAME = 0x02, // path name
	
	// 0x03-0x38 : reserved (common)
	//EXTH_MULTIVOLUME     = 0x39, // (reserved) multi-disk/multi-volume header
	// 0x3A-0x3E : reserved (common)
	
	EXTH_COMMENT          = 0x3f, // file comment (Amiga-style?), uncompressed
	EXTH_MSDOSATTRIBS     = 0x40,
	EXTH_WINDOWSTIMES     = 0x41, // Windows timestamps of file
	EXTH_LARGEFILE        = 0x42, // 64-bit filesize
	
	// 0x43-0x47 : reserved (msdos?)
	// 0x48-0x4F : reserved, for authenticity verification?
	
	EXTH_UNIXPERMISSIONS  = 0x50, // UNIX permission
	EXTH_UNIXGIDUID       = 0x51, // UNIX gid and uid
	EXTH_UNIXGROUP        = 0x52, // UNIX group name
	EXTH_UNIXUSER         = 0x53, // UNIX user name
	EXTH_UNIXLASTMODIFIED = 0x54, // UNIX last modified time 

	// 0x55-0x5F : reserved (Unix)
	// 0x60-0x7C : reserved (others)

	EXTH_CAPSULEHEADER = 0x7D, // encapsulation, MacBinary-like format-information??
	EXTH_PLATFORMINFORMATION = 0x7E, // attributes for OS/2 ?
	
	// permission, owner-id and timestamp, (level 3 on OS/2)
	EXTH_NEWATTRIBUTES = 0x7F, // Unix and msdos attribute information? (level 3 header(1))

	// 0x80-0xC3 : reserved (others)
	
	// 0xC4-0xC8 : compressed comments (different dictionary-sizes)
	EXTH_COMPCOMMENT1  = 0xC4, // dict size: 4096
	EXTH_COMPCOMMENT2  = 0xC5, // dict size: 8192
	EXTH_COMPCOMMENT3  = 0xC6, // dict size: 16384
	EXTH_COMPCOMMENT4  = 0xC7, // dict size: 32768
	EXTH_COMPCOMMENT5  = 0xC8, // dict size: 65536
	
	// 0xC9-0xF3 : reserved (others), 
	// 0xD0-0xDF OS-specific information?
	EXTH_CAPSULEHEADER2 = 0xFC, // encapsulation, another option?
	EXTH_PLATFORMINFORMATION2 = 0xFE, // another option?
	
	EXTH_UNIX_NEWATTRIBUTES = 0xFF // Unix file information? (level 3 on UNLHA32 ?)
};

class CLhHeader : public QObject
{
	Q_OBJECT

private:
	
	
	// TODO: these buffer handlings REALLY need to be fixed..
	// change methods later, move these to buffer-class or something
	//
	//char    *m_get_ptr;
	uint8_t *m_get_ptr; // position pointer to a buffer..

	inline void incrementPtr(const size_t nLen)
	{
		m_get_ptr = (m_get_ptr + nLen);
	}
	
	inline uint8_t get_byte()
	{
		uint8_t tmp = (m_get_ptr[0] & 0xFF);
		incrementPtr(1);
		return tmp;
	}
	
	inline uint16_t get_word()
	{
		uint16_t tmp = 0;
		for (int i = 2; i > 0; i--)
		{
			tmp = (tmp << 8);
			tmp += (m_get_ptr[i-1] & 0xFF);
		}
		incrementPtr(2);
		return tmp;
	}
	
	// note: sizeof(long) differs on 64-bit Linux and Windows
	// -> use ISO-standard typedef instead..
	inline uint32_t get_longword()
	{
		// avoid numerous temps
		// and possible shifting in smaller type
		// (loss of bits)
		uint32_t tmp = 0;
		for (int i = 4; i > 0; i--)
		{
			tmp = (tmp << 8);
			tmp += (m_get_ptr[i-1] & 0xFF);
		}
		incrementPtr(4);
		return tmp;
	}

	// only for 64-bit filesize information (if used?)
	inline uint64_t get_longlong()
	{
		// avoid numerous temps
		// and possible shifting in smaller type
		// (loss of bits)
		uint64_t tmp = 0;
		for (int i = 8; i > 0; i--)
		{
			tmp = (tmp << 8);
			tmp += (m_get_ptr[i-1] & 0xFF);
		}
		incrementPtr(8);
		return tmp;
	}
	
	// note: string isn't null-terminated in file.
	//
	// note: read-buffer is deallocated/overwritten
	// later when next chunk is read/done.
	//
	QString get_string(int len)
	{
		QString szVal;
		if (m_pTextCodec == nullptr)
		{
			szVal = QString::fromAscii((char*)m_get_ptr, len);
		}
		else
		{
			szVal = m_pTextCodec->toUnicode((char*)m_get_ptr, len);
		}
		incrementPtr(len);
		return szVal;
	}
	
	// get pathname (file/dir) and fix path-separator:
	// don't modify source-buffer as 
	// that affects CRC-calculation..
	// 	
	QString getPathname(int len, bool fixPath = true)
	{
		QByteArray tmp((char*)m_get_ptr, len);
		
		for (int i = 0; i < len; i++)
		{
			// fix '\\' path-separator 
			// since it may be handled as Yen-sign
			// after conversion to Unicode..
			if (tmp[i] == (char)0xFF
				&& fixPath == true)
			{
				tmp[i] = '/';
			}
		}
		incrementPtr(len);
		
		if (m_pTextCodec == nullptr)
		{
			return QString(tmp);
		}
		else
		{
			return m_pTextCodec->toUnicode(tmp);
		}
	}
	
	// get string upto NULL (if found),
	// used in case there are filename and file-comment
	// in same string (use them separately)
	//
	int getStringToNULL(int len, QString &string, bool fixPath = false)
	{
		for (int i = 0; i < len; i++)
		{
			// fix '\\' path-separator before reading
			// since Unicode-conversion may assume it is Yen-sign..
			//
			if (m_get_ptr[i] == (char)0xFF
				&& fixPath == true)
			{
				m_get_ptr[i] = '/';
			}
		
			// if there is NULL on the string
			// we should stop reading there:
			// in case of filename there may be file-comment after that
			// (such as Amiga file comment)
			// 
			if (m_get_ptr[i] == 0x00)
			{
				//char *pTmp = (char*)m_get_ptr; // for debug
				len = i;
				break;
			}
		}
		
		if (m_pTextCodec == nullptr)
		{
			string = QString::fromAscii((char*)m_get_ptr, len);
		}
		else
		{
			string = m_pTextCodec->toUnicode((char*)m_get_ptr, len);
		}
		
		incrementPtr(len);
		return len;
	}
	
	// just get value:
	// stored as 32-bit long only
	time_t get_unixtime()
	{
		uint32_t ltime = get_longword();
		return (time_t)ltime;
	}
	
	// get conversion helper
	// TODO:
	// CMacHfsTimeHelper get_mactime()
	time_t get_mactime()
	{
		// this isn't too far off..?
		// (see comments regarding extended area..)
		uint32_t ltime = get_longword();
		return (time_t)ltime;
		// TODO:
		//return CMacHfsTimeHelper(ltime);
	}
	
	// get conversion helper
	CGenericTime get_generictime()
	{
		uint32_t ltime = get_longword();
		return CGenericTime(ltime);
	}

	// get conversion helper
	CFiletimeHelper get_wintime()
	{
		uint64_t ultime = get_longlong();
		return CFiletimeHelper(ultime);
	
		/* old style
		uint32_t ulHiPart = get_longword();
		uint32_t ulLoPart = get_longword();
		return CFiletimeHelper(ulHiPart, ulLoPart);
		*/
	}
	

protected:
	
	typedef QList<LzHeader*> tFileList;
	tFileList m_HeaderList;
	
	CCrcIo m_crcio;
	
	QTextCodec *m_pTextCodec;
    CIOBuffer *m_pReadBuffer;

	int calc_sum(unsigned char *p, size_t len) const
	{
		int sum = 0;
		while (len--) 
		{
			sum += *p++;
		}
		return sum & 0xff;
	}
	

public:
	CLhHeader(QObject *parent = 0)
		: QObject(parent)
		, m_get_ptr(nullptr)
		, m_HeaderList()
		, m_crcio()
		, m_pTextCodec(nullptr)
		, m_pReadBuffer(nullptr)
	{}
	~CLhHeader(void)
	{
		// cleanup when destroyed
		Clear();
	}
	
	void Clear()
	{
		auto it = m_HeaderList.begin();
		auto itEnd = m_HeaderList.end();
		while (it != itEnd)
		{
			LzHeader *pHeader = (*it);
			delete pHeader;
			
			++it;
		}
		m_HeaderList.clear();
	}
	
	void SetConversionCodec(QTextCodec *pCodec)
	{
		m_pTextCodec = pCodec;
	}

	// If true, archive is msdos SFX (executable)
    bool IsMsdosSFX1(CIOBuffer &Buffer) const
	{
		// "MZ..", 0x4D5A9000
		// -> MSDOS, OS/2 or MS-Windows executable/DLL
		//
		unsigned char *p = Buffer.GetBegin();
		if (p[0] == 0x4D && p[1] == 0x5A && p[2] == 0x90 && p[3] == 0x00)
		{
			// identifier for executable/DLL found
			return true;
		}
		return false;
	}

	// If true, archive is Amiga 'run' (executable)
    bool IsAmigaRun(CIOBuffer &Buffer) const
	{
		// 0x000003F3 -> AmigaDOS executable
		unsigned char *p = Buffer.GetBegin();
		if (p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x03 && p[3] == 0xF3)
		{
			return true;
		}
		// 0x000003E7 -> AmigaDOS library
		if (p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x03 && p[3] == 0xE7)
		{
			return true;
		}
		return false;
	}
	
	// if file is self-decompressing exectuable,
	// we need to seek to actual archive header..
    unsigned char *SeekArchiveHeader(CIOBuffer &Buffer) const
	{
		unsigned char *p = Buffer.GetBegin();
		unsigned char *pEnd = Buffer.GetEnd();
		
		bool bHeaderFound = false;
		size_t nPos = 0;
		while (p < pEnd && bHeaderFound == false)
		{
			// -l??- -> should be archive-header
			if (! (p[I_METHOD] == '-' && p[I_METHOD+1]=='l' && p[I_METHOD+4]=='-'))
			{
				bHeaderFound = true;
				break;
			}
			p = p+nPos;
		}
		
		if (bHeaderFound == true)
		{
			// 
			return p;
		}
		
		// not found -> not archive?
		return nullptr;
	}
	
	// is valid lha type: verify "-l??-" keyword
	// (e.g. -lh5- -lz0- etc.)
	//
    bool IsValidLha(CIOBuffer &Buffer) const
	{
		bool bHeaderValid = false;

		// usually we can start from beginning
		unsigned char *p = Buffer.GetBegin();
		
		// executable (self-decompressing)
		// -> seek actual archive-header
		if (IsMsdosSFX1(Buffer) == true)
		{
			p = SeekArchiveHeader(Buffer);
			if (p == nullptr)
			{
				// not found -> not archive?
				return false;
			}
			// -> now do actual checking
		}

		// -lh?- or -lz?- where '?' = 0..7
		if (! (p[I_METHOD] == '-' && p[I_METHOD+1] == 'l' && p[I_METHOD+4] == '-'))
			   //&& (p[I_METHOD+2] == 'h' || p[I_METHOD+2] == 'z')))
		{
			// non-valid identifier
			return false;
		}

        // level 0 or 1 header 
        if ((p[I_HEADER_LEVEL] == 0 || p[I_HEADER_LEVEL] == 1)
            && p[I_HEADER_SIZE] > 20
            && p[I_HEADER_CHECKSUM] == calc_sum(p+2, p[I_HEADER_SIZE])) 
		{
			bHeaderValid = true;
        }

        // level 2 header 
        if (p[I_HEADER_LEVEL] == 2
            && p[I_HEADER_SIZE] >= 24
            && p[I_ATTRIBUTE] == 0x20) 
		{
			bHeaderValid = true;
        }
		
		return bHeaderValid;
	}

	void ParseHeaders(CAnsiFile &ArchiveFile);

signals:
	// progress-status by signals, errors by exceptions
	void message(QString);
	void warning(QString);
	
protected:

	bool get_header_level0(CAnsiFile &ArchiveFile, LzHeader *pHeader);
	bool get_header_level1(CAnsiFile &ArchiveFile, LzHeader *pHeader);
	bool get_header_level2(CAnsiFile &ArchiveFile, LzHeader *pHeader);
	bool get_header_level3(CAnsiFile &ArchiveFile, LzHeader *pHeader);

	size_t get_extended_area(CAnsiFile &ArchiveFile, LzHeader *pHeader, const size_t nLen);
	size_t get_extended_header(CAnsiFile &ArchiveFile, LzHeader *pHeader, unsigned int *hcrc);
	
	void readFilenameComment(CAnsiFile &ArchiveFile, LzHeader *pHeader, const int name_length);
	
	void UpdatePaddingToCrc(CAnsiFile &ArchiveFile, unsigned int &hcrc, const long lPadSize);
	
	friend class CLhArchive;
};


#endif // LZHEADER_H
