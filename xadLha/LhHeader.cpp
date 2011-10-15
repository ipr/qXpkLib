//////////////////////////////////
//
// structure and code for handling Lz-archive header
//
// partly rewritten from header.c in LHa for UNIX
// by: Ilkka Prusi 2011
//
// original copyrights:
/* ------------------------------------------------------------------------ */
/* LHa for UNIX                                                             */
/*              header.c -- header manipulate functions                     */
/*                                                                          */
/*      Modified                Nobutaka Watazaki                           */
/*                                                                          */
/*  Original                                                Y.Tagawa        */
/*  modified                                    1991.12.16  M.Oki           */
/*  Ver. 1.10  Symbolic Link added              1993.10.01  N.Watazaki      */
/*  Ver. 1.13b Symbolic Link Bug Fix            1994.08.22  N.Watazaki      */
/*  Ver. 1.14  Source All chagned               1995.01.14  N.Watazaki      */
/*  Ver. 1.14i bug fixed                        2000.10.06  t.okamoto       */
/*  Ver. 1.14i Contributed UTF-8 convertion for Mac OS X                    */
/*                                              2002.06.29  Hiroto Sakai    */
/*  Ver. 1.14i autoconfiscated & rewritten      2003.02.23  Koji Arai       */
/* ------------------------------------------------------------------------ */


#include "LhHeader.h"

#include <QString>


void CLhHeader::ParseHeaders(CAnsiFile &ArchiveFile)
{
    //char *archive_delim = "\377\\"; /* `\' is for level 0 header and broken archive. */
	
	if (m_pReadBuffer == nullptr)
	{
		// emulate old style, read 4096 max. at a time
		m_pReadBuffer = new CReadBuffer(4096); // default bufsize
	}
	m_pReadBuffer->PrepareBuffer(4096, false);

	bool bIsEnd = false;
	while (bIsEnd == false)
	{
		m_get_ptr = m_pReadBuffer->GetBegin();
		
		long lHeaderPos = 0;
		if (ArchiveFile.Tell(lHeaderPos) == false)
		{
			throw IOException("Failure getting current position");
		}
		
		int end_mark = getc((FILE*)ArchiveFile);
		if (end_mark == EOF || end_mark == 0) 
		{
			bIsEnd = true;
			break;           /* finish */
		}
		m_get_ptr[0] = end_mark;
		
		if (ArchiveFile.Read(m_get_ptr + 1, COMMON_HEADER_SIZE - 1) == false) 
		{
			throw ArcException("Invalid header (LHarc file ?)", "");
		}
		
		LzHeader *pHeader = new LzHeader();
		m_HeaderList.push_back(pHeader);

		// try to keep track of how much there is for header in truth..
		// note: we don't know true "final" header size until reading it
		// according to level, extended header fields etc.
		// 
		pHeader->header_size = COMMON_HEADER_SIZE;
		pHeader->header_pos = lHeaderPos;
		pHeader->header_level = m_get_ptr[I_HEADER_LEVEL];
		
		bool bRet = false;
		switch (pHeader->header_level) 
		{
		case 0:
			bRet = get_header_level0(ArchiveFile, pHeader);
			break;
		case 1:
			bRet = get_header_level1(ArchiveFile, pHeader);
			break;
		case 2:
			bRet = get_header_level2(ArchiveFile, pHeader);
			break;
		case 3:
			bRet = get_header_level3(ArchiveFile, pHeader);
			break;
		default:
			throw ArcException("Unknown level header", pHeader->header_level);
		}
		
		if (bRet == false)
		{
			throw ArcException("Failure reading header", "");
		}
		
		// keep this file offset (after header)
		// to locate data in archive for the file-entry later
		if (ArchiveFile.Tell(pHeader->data_pos) == false)
		{
			throw IOException("Failure getting current position");
		}
		
		// parse method-string to enum now..
		pHeader->m_enCompression = pHeader->GetMethod();
		if (pHeader->m_enCompression == LZ_UNKNOWN)
		{
			emit warning(QString("unknown compression in file: %1 method: ")
							.arg(pHeader->filename).append(pHeader->pack_method));
		}
		else if ((pHeader->packed_size == 0 || pHeader->original_size == 0)
				&& pHeader->m_enCompression != LZHDIRS_METHOD_NUM)
		{
			emit warning(QString("zero size in file: %1 method: ")
							.arg(pHeader->filename).append(pHeader->pack_method));
		}
		
		// fix path-names, no need for MSDOS-style paths
		// since around 1999 (W2k or newer)..
		pHeader->filename.replace('\\', "/");
		pHeader->dirname.replace('\\', "/");
		
		// symlink only on unix-style file-attributes
		if (pHeader->m_pUnixHeader != nullptr)
		{
			if (pHeader->GetUnixHeader()->UnixMode.isSymlink) 
			{
				/* hdr->name is symbolic link name */
				/* hdr->realname is real name */
				int iPos = pHeader->filename.lastIndexOf('|');
				if (iPos == -1)
				{
					// not found as expecing -> fatal
					throw ArcException("Unknown symlink name", pHeader->filename.toStdString());
				}
				pHeader->realname = pHeader->filename.left(iPos +1);
			}
		}
		
		// seek past actual data of the entry in archive
		if (ArchiveFile.Seek(pHeader->packed_size, SEEK_CUR) == false)
		{
			throw IOException("Failure seeking next header");
		}
		
		emit message(QString("Header found: %1").arg(pHeader->filename));
	}
}


/*
 * level 0 header
 *
 *
 * offset  size  field name
 * ----------------------------------
 *     0      1  header size    [*1]
 *     1      1  header sum
 *            ---------------------------------------
 *     2      5  method ID                         ^
 *     7      4  packed size    [*2]               |
 *    11      4  original size                     |
 *    15      2  time                              |
 *    17      2  date                              |
 *    19      1  attribute                         | [*1] header size (X+Y+22)
 *    20      1  level (0x00 fixed)                |
 *    21      1  name length                       |
 *    22      X  pathname                          |
 * X +22      2  file crc (CRC-16)                 |
 * X +24      Y  ext-header(old style)             v
 * -------------------------------------------------
 * X+Y+24        data                              ^
 *                 :                               | [*2] packed size
 *                 :                               v
 * -------------------------------------------------
 *
 * ext-header(old style)
 *     0      1  ext-type ('U')
 *     1      1  minor version
 *     2      4  UNIX time
 *     6      2  mode
 *     8      2  uid
 *    10      2  gid
 *
 * attribute (MS-DOS)
 *    bit1  read only
 *    bit2  hidden
 *    bit3  system
 *    bit4  volume label
 *    bit5  directory
 *    bit6  archive bit (need to backup)
 *
 */
bool CLhHeader::get_header_level0(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
    pHeader->size_field_length = 2; /* in bytes */
	
	const size_t header_size = get_byte(); // whole header -2 (size&sum)
    const int checksum = get_byte();
    pHeader->header_size = header_size +2;
	
	unsigned char *pBuf = m_pReadBuffer->GetBegin();

    if (ArchiveFile.Read(pBuf + COMMON_HEADER_SIZE, header_size + 2 - COMMON_HEADER_SIZE) == false) 
	{
		throw ArcException("Invalid header (LHarc file ?)", header_size);
    }

    if (calc_sum(pBuf + I_METHOD, header_size) != checksum)
	{
		throw ArcException("Checksum error (LHarc file?)", checksum);
    }

	// there's size to it given so use it
    pHeader->pack_method = get_string(PACKMETHOD_TYPE_LENGTH);
    pHeader->packed_size = get_longword();
    pHeader->original_size = get_longword();
    pHeader->last_modified_stamp.setTime_t((time_t)get_generictime());
    pHeader->GetDosHeader()->msdos_attributes.SetFromValue(get_byte()); /* MS-DOS attribute */
    pHeader->header_level = get_byte();
    
    const int name_length = get_byte(); // keep full length
    
    // on Amiga, may include file comment as part of filename
	readFilenameComment(ArchiveFile, pHeader, name_length);
	
	// header_size: total data in header
	// name_length: size of filename (from above)
	// I_GENERIC_HEADER_SIZE: size of fixed fields in header
	// -> extended area size is whatever remains,
    long extend_size = (header_size+2 - name_length) - (I_GENERIC_HEADER_SIZE -2);
	if (extend_size == 0) 
	{
	    // old version of LArc does not write CRC-field
	    // -> ok, nothing more to do here?
		pHeader->os_type = EXTEND_GENERIC;
		return true;
	}
    else if (extend_size < 0 || extend_size == 1) 
	{
		// any other values it's error?
		throw ArcException("Unknown header (lha file?)", extend_size);
    }
	else if (extend_size >= 2) 
	{
		// 2 or more bytes remain,
		// get & keep CRC
		pHeader->setFileCrc(get_word());
		extend_size -= 2; // update count
	}

    if (extend_size == 0)
	{
		// no extended area -> ok
        return true;
	}

	// read old-style "extended area"
    extend_size -= get_extended_area(ArchiveFile, pHeader, extend_size);
    if (extend_size > 0)
	{
        incrementPtr(extend_size); // skip rest of old style extend area
	}

	// +2 for size-field of extended header segment?
    pHeader->header_size += 2;
    return true;
}


/*
 * level 1 header
 *
 *
 * offset   size  field name
 * -----------------------------------
 *     0       1  header size   [*1]
 *     1       1  header sum
 *             -------------------------------------
 *     2       5  method ID                        ^
 *     7       4  skip size     [*2]               |
 *    11       4  original size                    |
 *    15       2  time                             |
 *    17       2  date                             |
 *    19       1  attribute (0x20 fixed)           | [*1] header size (X+Y+25)
 *    20       1  level (0x01 fixed)               |
 *    21       1  name length                      |
 *    22       X  filename                         |
 * X+ 22       2  file crc (CRC-16)                |
 * X+ 24       1  OS ID                            |
 * X +25       Y  ???                              |
 * X+Y+25      2  next-header size                 v
 * -------------------------------------------------
 * X+Y+27      Z  ext-header                       ^
 *                 :                               |
 * -----------------------------------             | [*2] skip size
 * X+Y+Z+27       data                             |
 *                 :                               v
 * -------------------------------------------------
 *
 */
bool CLhHeader::get_header_level1(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
    pHeader->size_field_length = 2; /* in bytes */
	
	size_t header_size = get_byte(); // whole header -2 (size&sum)
	int checksum = get_byte();
    pHeader->header_size = header_size +2;

	unsigned char *pBuf = m_pReadBuffer->GetBegin();
	
    if (ArchiveFile.Read(pBuf + COMMON_HEADER_SIZE, (header_size + 2) - COMMON_HEADER_SIZE) == false) 
	{
		throw ArcException("Invalid header (LHarc file ?)", header_size);
    }

    if (calc_sum(pBuf + I_METHOD, header_size) != checksum) 
	{
		throw ArcException("Checksum error (LHarc file?)", checksum);
    }

	// there's size to it given so use it
    pHeader->pack_method = get_string(PACKMETHOD_TYPE_LENGTH);
    pHeader->packed_size = get_longword(); /* skip size */
    pHeader->original_size = get_longword();
    pHeader->last_modified_stamp.setTime_t((time_t)get_generictime());
    pHeader->GetDosHeader()->msdos_attributes.SetFromValue(get_byte()); /* 0x20 fixed */
    pHeader->header_level = get_byte();

    int name_length = get_byte(); // keep for later..
    
    // on Amiga, may include file comment as part of filename
	readFilenameComment(ArchiveFile, pHeader, name_length);

	// get & keep CRC
    pHeader->setFileCrc(get_word());

	// count size for old style "extended area"
    int extarea_size = ((header_size+2) - name_length) - I_LEVEL1_HEADER_SIZE;
    extarea_size += 1; // for OS-type reading
    extarea_size -= get_extended_area(ArchiveFile, pHeader, extarea_size);
    if (extarea_size > 0)
	{
        incrementPtr(extarea_size); // skip rest of old style extend area 
	}

	pHeader->extend_size = get_word();
    long extend_size = get_extended_header(ArchiveFile, pHeader, 0);
    if (extend_size == -1)
	{
        return false;
	}

    /* On level 1 header, size fields should be adjusted. */
    /* the `packed_size' field contains the extended header size. */
    /* the `header_size' field does not. */
    pHeader->packed_size -= extend_size;
    pHeader->header_size += extend_size + 2;

    return true;
}

/*
 * level 2 header
 *
 *
 * offset   size  field name
 * --------------------------------------------------
 *     0       2  total header size [*1]           ^
 *             -----------------------             |
 *     2       5  method ID                        |
 *     7       4  packed size       [*2]           |
 *    11       4  original size                    |
 *    15       4  time                             |
 *    19       1  RESERVED (0x20 fixed)            | [*1] total header size
 *    20       1  level (0x02 fixed)               |      (X+26+(1))
 *    21       2  file crc (CRC-16)                |
 *    23       1  OS ID                            |
 *    24       2  next-header size                 |
 * -----------------------------------             |
 *    26       X  ext-header                       |
 *                 :                               |
 * -----------------------------------             |
 * X +26      (1) padding                          v
 * -------------------------------------------------
 * X +26+(1)      data                             ^
 *                 :                               | [*2] packed size
 *                 :                               v
 * -------------------------------------------------
 *
 */
 //
// note: if "low byte" of header size field is zero, 
// should add extra byte to header (with 0x00 value)
// and replace low byte in size-field with 0x01..
//
bool CLhHeader::get_header_level2(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
    pHeader->size_field_length = 2; /* in bytes */
	
	pHeader->header_size = get_word(); // (see notes)

	unsigned char *pBuf = m_pReadBuffer->GetBegin();
	
    if (ArchiveFile.Read(pBuf + COMMON_HEADER_SIZE, I_LEVEL2_HEADER_SIZE - COMMON_HEADER_SIZE) == false) 
	{
		throw ArcException("Invalid header (LHarc file ?)", pHeader->header_size);
    }

	// there's size to it given so use it
    pHeader->pack_method = get_string(PACKMETHOD_TYPE_LENGTH);
    pHeader->packed_size = get_longword();
    pHeader->original_size = get_longword();
    pHeader->last_modified_stamp.setTime_t((time_t)get_unixtime());
    pHeader->GetDosHeader()->msdos_attributes.SetFromValue(get_byte()); /* reserved */
    pHeader->header_level = get_byte();

    /* defaults for other type */
    pHeader->setFileCrc(get_word());
    pHeader->os_type = get_byte(); // OSType
    pHeader->extend_size = get_word();

    // fixed size -> should use constant value
    unsigned int hcrc = m_crcio.calccrc(0, pBuf, I_LEVEL2_HEADER_SIZE);
    long extend_size = get_extended_header(ArchiveFile, pHeader, &hcrc);
    if (extend_size == -1)
	{
		// should throw exception instead?
        return false;
	}

	// see notes, padding just 0/1 always?
    int padding = (pHeader->header_size - I_LEVEL2_HEADER_SIZE - extend_size);
	UpdatePaddingToCrc(ArchiveFile, hcrc, padding);

    if (pHeader->isHeaderCrc(hcrc) == false)
	{
		throw ArcException("header CRC error", hcrc);
	}

    return true;
}

/*
 * level 3 header
 *
 *
 * offset   size  field name
 * --------------------------------------------------
 *     0       2  size field length (4 fixed)      ^
 *     2       5  method ID                        |
 *     7       4  packed size       [*2]           |
 *    11       4  original size                    |
 *    15       4  time                             |
 *    19       1  RESERVED (0x20 fixed)            | [*1] total header size
 *    20       1  level (0x03 fixed)               |      (X+32)
 *    21       2  file crc (CRC-16)                |
 *    23       1  OS ID                            |
 *    24       4  total header size [*1]           |
 *    28       4  next-header size                 |
 * -----------------------------------             |
 *    32       X  ext-header                       |
 *                 :                               v
 * -------------------------------------------------
 * X +32          data                             ^
 *                 :                               | [*2] packed size
 *                 :                               v
 * -------------------------------------------------
 *
 */
bool CLhHeader::get_header_level3(CAnsiFile &ArchiveFile, LzHeader *pHeader)
{
    pHeader->size_field_length = get_word();
    pHeader->header_size = COMMON_HEADER_SIZE + I_LEVEL3_HEADER_SIZE;

	unsigned char *pBuf = m_pReadBuffer->GetBegin();
	
    if (ArchiveFile.Read(pBuf + COMMON_HEADER_SIZE, I_LEVEL3_HEADER_SIZE - COMMON_HEADER_SIZE) == false) 
	{
		throw ArcException("Invalid header (LHarc file ?)", pHeader->header_size);
    }

	// there's size to it given so use it
    pHeader->pack_method = get_string(PACKMETHOD_TYPE_LENGTH);
    pHeader->packed_size = get_longword();
    pHeader->original_size = get_longword();
    pHeader->last_modified_stamp.setTime_t((time_t)get_unixtime());
    pHeader->GetDosHeader()->msdos_attributes.SetFromValue(get_byte()); /* reserved */
    pHeader->header_level = get_byte();

    /* defaults for other type */
    pHeader->setFileCrc(get_word());
    
    pHeader->os_type = get_byte();
    pHeader->header_size = get_longword();
    pHeader->extend_size = get_longword();

    // fixed size -> use constant value
    unsigned int hcrc = m_crcio.calccrc(0, pBuf, I_LEVEL3_HEADER_SIZE);
    long extend_size = get_extended_header(ArchiveFile, pHeader, &hcrc);
    if (extend_size == -1)
	{
        return false;
	}

    int padding = (pHeader->header_size - I_LEVEL3_HEADER_SIZE - extend_size);
	UpdatePaddingToCrc(ArchiveFile, hcrc, padding);

    if (pHeader->isHeaderCrc(hcrc) == false)
	{
		throw ArcException("header CRC error", hcrc);
	}
    return true;
}

// Extended area: used by at least level 0 header, possibly level 1 also,
// replaced by "extended header" in level 2 and 3 headers
//
/*
 * For "EXTEND_UNIX"
 *
 * offset   size  field name
 * --------------------------------------------------
 *     0       1  OS type ID     
 *     1       1  minor version
 *     2       4  time (time_t)
 *     6       2  permission (bitfield)
 *     8       2  Unix user ID
 *    10       2  Unix group ID
 */
/*
 * For "EXTEND_MACOS",
 * unknown if ever used.
 *
 * offset   size  field name
 * --------------------------------------------------
 *     0       1  extended area ID (slightly different..)
 *     1       4  file creator
 *     5       4  file type
 *     9       4  creation time
 *    13       4  size of compressed resource fork
 *    17       4  size of resource fork
 *    21       2  CRC16 or resource fork
 *    23       2  MacOS file attributes
 *
 * About Mac OS HFS/HFS+ volume timestamp, check! :
 * - unsigned 32-bit integer (UInt32)
 * - HFS+ (Mac OS X?): number of seconds since midnight, January 1, 1904, GMT
 * - HFS: same but localtime ?
 * - maximum representable date is February 6, 2040 at 06:28:15 GMT
 * - The date values do not account for leap seconds. They do include a leap day in every year that is evenly divisible by four. 
 * This is sufficient given that the range of representable dates does not contain 1900 or 2100, neither of which have leap days.
 *
 * Actully, stored as "time_t" but adjusted automatically to 1904 instead of 1970 ?
 * (constant difference..)
 *
 */
/*
 * For "EXTEND_OS9"
 * unknown if ever used.
 *
 * offset   size  field name
 * --------------------------------------------------
 *     0       1  OS type ID     
 *     1       1  Sub identifier     
 *     2       1  MSDOS file attributes
 *     3       2  Owner ID
 */
size_t CLhHeader::get_extended_area(CAnsiFile &ArchiveFile, LzHeader *pHeader, const size_t nLen)
{
	bool bMacOsArea = false; // MacOS style extended area
	size_t areaSize = 0;

	// note: if packed on MacOS this may have 
	// different values from Unix LHa..
	//
    pHeader->os_type = get_byte();
    areaSize += 1;
    
    // no ambiguousness with this type-ID?
    /*
    if (nLen == 5 && pHeader->os_type == EXTEND_OS9)
    {
		uint8_t subid = get_byte();
		uint8_t attribs = get_byte();
		uint16_t ownerID = get_word();
	    return nLen;
    }
    */

	// check whole size, including OS ID..
	if (nLen == 12 && pHeader->os_type == EXTEND_UNIX
		&& bMacOsArea == false) 
	{
		LzUnixHeader *pUxh = pHeader->GetUnixHeader();
		pUxh->minor_version = get_byte();
		// this still goes in "common" header
		pHeader->last_modified_stamp.setTime_t((time_t)get_unixtime());
		pUxh->UnixMode.ParseMode(get_word());
		pUxh->unix_uid = get_word();
		pUxh->unix_gid = get_word();
		return nLen; // set fixed length (read wholly)
	} 
    
    // check for MacOS-style header..
    if (pHeader->os_type == 0x00)
    {
		// same as "EXTEND_GENERIC" - MS-DOS in MacOS-style extended area..
		// -> convert to enumeration
		pHeader->os_type = EXTEND_MSDOS;
		//bMacOsArea = true; // ?? is it or not?? same ID ??
    }
    else if (pHeader->os_type == 0x01)
    {
		// MacOS in MacOS-style extended area..
		// -> convert to enumeration
		pHeader->os_type = EXTEND_MACOS;
		bMacOsArea = true;
    }
    else if (pHeader->os_type == 0x02)
    {
		// Unix in MacOS-style extended area..
		// -> convert to enumeration
		pHeader->os_type = EXTEND_UNIX;
		bMacOsArea = true;
    }

	// known size and should be supported data format..
	if (nLen == 25 && bMacOsArea == true)
	{
		// note: check endianess..
		// also contents..?
		uint32_t creator = get_longword();
		uint32_t filetype = get_longword();
		
		// TODO: make conversion helper
		//CMacHfsTimeHelper time = get_mactime();
		//pHeader->last_modified_stamp.setTime_t((time_t)time());
		uint32_t time = get_mactime();
		uint32_t compressedSize = get_longword();
		uint32_t originalSize = get_longword();
		uint16_t crc = get_word();
		uint16_t macAttribs = get_word();
		//TODO: make attrib handler
		//MacOSHFSAttributes.ParseAttributes();
		areaSize = nLen; // set fixed length (read wholly)
	}
	return areaSize;
}


/*
 * extended header
 *
 *             size  field name
 *  --------------------------------
 *  base header:         :
 *           2 or 4  next-header size  [*1]
 *  --------------------------------------
 *  ext header:   1  ext-type            ^
 *                ?  contents            | [*1] next-header size
 *           2 or 4  next-header size    v
 *  --------------------------------------
 *
 *  on level 1, 2 header:
 *    size field is 2 bytes
 *  on level 3 header:
 *    size field is 4 bytes
 */

size_t CLhHeader::get_extended_header(CAnsiFile &ArchiveFile, LzHeader *pHeader, unsigned int *hcrc)
{
    size_t whole_size = pHeader->extend_size;
    
    // overhead of extension type (byte) + length (2 or 4 bytes, depends on header level etc.)
    int nExtensionOverhead = 1 + pHeader->size_field_length; /* `ext-type' + `next-header size' */

    if (pHeader->header_level == 0)
	{
        return 0;
	}

	// clear or allocate larger if necessary
	m_pReadBuffer->PrepareBuffer(pHeader->extend_size, false);
	
	// keep this size while reading
	size_t extend_size = pHeader->extend_size;
    while (extend_size) 
	{
		m_get_ptr = m_pReadBuffer->GetBegin();
		
		// this only makes sense for "wrap-under" case of unsigned counter..
        if (m_pReadBuffer->GetSize() < extend_size) 
		{
			throw ArcException("header size too large.", extend_size);
        }

        if (ArchiveFile.Read(m_pReadBuffer->GetBegin(), extend_size) == false) 
		{
			throw ArcException("Invalid header (LHa file ?)", extend_size);
        }
		
        // type of extension "header"
        //
        const int iExthType = get_byte();
        switch (iExthType) 
		{
        case EXTH_CRC:
			{
				/* header crc (CRC-16) */
				pHeader->setHeaderCrc(get_word());
				
				/* clear CRC in buffer for CRC calculation. */
				uint8_t *pBuf = m_pReadBuffer->GetBegin();
				pBuf[1] = pBuf[2] = 0;
				
				incrementPtr(extend_size - nExtensionOverhead - 2);
			}
            break;
            
        case EXTH_FILENAME:
            // filename 
            // note: in case of broken compression-tool 
            // there is a wchar-string instead of narrow-char which it should be..
            pHeader->filename = getPathname(extend_size - nExtensionOverhead, true);
            break;
        case EXTH_PATHNAME:
            // directory 
            // note: in case of broken compression-tool 
            // there is a wchar-string instead of narrow-char which it should be..
            pHeader->dirname = getPathname(extend_size - nExtensionOverhead, true);
            break;
			
        case EXTH_MSDOSATTRIBS:
            /* MS-DOS attribute flags */
            // (if EXTEND_MSDOS || EXTEND_HUMAN || EXTEND_GENERIC)
            pHeader->GetDosHeader()->msdos_attributes.SetFromValue(get_word());
            break;
			
        case EXTH_WINDOWSTIMES:
			/* Windows time stamp (FILETIME structure) */
			/* time in 100 nanosecond-intervals since 1601-01-01 00:00:00 (UTC) */
			/* note order of reading values.. use conversion helpers */
			pHeader->creation_stamp.setTime_t((time_t)get_wintime());
			pHeader->last_modified_stamp.setTime_t((time_t)get_wintime()); // already set ?
			pHeader->last_access_stamp.setTime_t((time_t)get_wintime());
			break;
		case EXTH_LARGEFILE:
			// 64-bit filesize (Windows only?)
			// compressed size and original size of file
			pHeader->lf_packedsize = get_longlong();
			pHeader->lf_originalsize = get_longlong();
			break;
			
        case EXTH_UNIXPERMISSIONS:
            /* UNIX permission */
            // (if EXTEND_UNIX)
            pHeader->GetUnixHeader()->UnixMode.ParseMode(get_word());
            break;
        case EXTH_UNIXGIDUID:
            /* UNIX gid and uid */
            pHeader->GetUnixHeader()->unix_gid = get_word();
            pHeader->GetUnixHeader()->unix_uid = get_word();
            break;
        case EXTH_UNIXGROUP:
            /* UNIX group name */
            pHeader->GetUnixHeader()->unix_group = get_string(extend_size - nExtensionOverhead);
            break;
        case EXTH_UNIXUSER:
            /* UNIX user name */
            pHeader->GetUnixHeader()->unix_user = get_string(extend_size - nExtensionOverhead);
            break;
        case EXTH_UNIXLASTMODIFIED:
            /* UNIX last modified time */
			// (32-bit time_t)
			// this still goes in "common" header
            pHeader->last_modified_stamp.setTime_t((time_t)get_unixtime());
            break;
            
            /*
		case EXTH_CAPSULEHEADER:
			// - 4 bytes, capsule header size
			// - 4 bytes, file size
			break;
			*/

			/*
		case EXTH_PLATFORMINFORMATION:
			// - 1 byte, extended attribute identifier:
			//  -- 0x00: no compression
			//  -- 0x01: compression (unknown packing)
			// - 1 byte, os name size (in bytes)
			// - n bytes, os name (see above)
			// - n bytes, extended attribute
			break;
			*/
			
			/*
		case EXTH_NEWATTRIBUTES:
			// - 2 bytes, msdos file attribute
			// - 2 bytes, "permission" of file
			// - 2 bytes, group ID
			// - 2 byte, user ID
			// - 4 bytes, creation time (time_t)
			// - 4 bytes, last acccess time (time_t)
			break;
			*/

			/*
		case EXTH_UNIX_NEWATTRIBUTES:
			// note: should be avoided due confusion of format with UNLHA32.DLL?
			// something like this:
			// - 4 bytes "permission" of compressed file, only "2 high bytes" should be used?
			// - 4 bytes group ID
			// - 4 bytes user ID
			// - 4 bytes creation time (time_t)
			// - 4 bytes last access time (time_t)
			break;
			*/
            
		case EXTH_COMMENT:
			// uncompressed comment
            pHeader->file_comment = get_string(extend_size - nExtensionOverhead);
            break;
            
			/*
		case EXTH_COMPCOMMENT1:
		case EXTH_COMPCOMMENT2:
		case EXTH_COMPCOMMENT3:
		case EXTH_COMPCOMMENT4:
		case EXTH_COMPCOMMENT5:
			// compressed comment (varying dictionary sizes)
			// just read compressed bytes and decompress somewhere?
            break;
            */
			
        default:
            /* other headers */
            /* 0x39: multi-disk header
               0x42: 64bit large file size
               0x48-0x4f(?): reserved for authenticity verification
               0x7d: encapsulation
               0x7e: extended attribute - platform information
               0x7f: extended attribute - permission, owner-id and timestamp
                     (level 3 on OS/2)
               0xc4: compressed comment (dict size: 4096)
               0xc5: compressed comment (dict size: 8192)
               0xc6: compressed comment (dict size: 16384)
               0xc7: compressed comment (dict size: 32768)
               0xc8: compressed comment (dict size: 65536)
               0xd0-0xdf(?): operating systemm specific information
               0xfc: encapsulation (another opinion)
               0xfe: extended attribute - platform information(another opinion)
               0xff: extended attribute - permission, owner-id and timestamp
                     (level 3 on UNLHA32) 
            */
            incrementPtr(extend_size - nExtensionOverhead);
			emit warning(QString("unknown extended header %1").arg(iExthType));
            break;
        }

        if (hcrc)
		{
            *hcrc = m_crcio.calccrc(*hcrc, m_pReadBuffer->GetBegin(), extend_size);
		}

		// note: size of length-field may vary,
		// read length of next now.
		// this is also dependant on extension type used before it?
        if (pHeader->size_field_length == 2)
		{
            extend_size = get_word();
		}
        else
		{
            extend_size = get_longword();
		}
		
		// keep track of how much was read in extensions
		whole_size += extend_size;
    }

    /* concatenate dirname and filename */
    if (pHeader->dirname.length() > 0) 
	{
		// we assume there is a path separator..
		pHeader->filename.insert(0, pHeader->dirname);
    }

    return whole_size;
}

// read filename and (optional) comment from archive-file.
//
// Note: in some cases (Amiga-packed) there is filecomment
// also in same string as filename (separated by NULL).
//
// Normal Unicode-reading tries to locate two NULL-terminators
// before ending string -> need to check for that.
//
void CLhHeader::readFilenameComment(CAnsiFile &ArchiveFile, LzHeader *pHeader, const int name_length)
{
	// read at max. given length, stop on NULL if found:
	// check what remains (if any)
    int read_name_len = getStringToNULL(name_length, pHeader->filename, true);
    if (read_name_len < name_length)
    {
		int null = get_byte(); // read null byte..
		read_name_len += 1;
		
		// read remaining part to file comment
		pHeader->file_comment = get_string(name_length - read_name_len);
	}
}

void CLhHeader::UpdatePaddingToCrc(CAnsiFile &ArchiveFile, unsigned int &hcrc, const long lPadSize)
{
	// allocate enough for padding (zeroed)
	// or just clear existing if enough
	m_pReadBuffer->PrepareBuffer(lPadSize, false);

	// check how much of file remains
	long lPos = 0;
	if (ArchiveFile.Tell(lPos) == false)
	{
		throw IOException("Failed to tell() current position");
	}
	long lRemaining = (ArchiveFile.GetSize() - lPos);

	// read padding
	bool bRet = false;
	if (lRemaining < lPadSize)
	{
		bRet = ArchiveFile.Read(m_pReadBuffer->GetBegin(), lRemaining);
	}
	else
	{
		bRet = ArchiveFile.Read(m_pReadBuffer->GetBegin(), lPadSize);
	}
	
	if (bRet == false)
	{
		throw IOException("Failed reading header padding for crc");
	}

	// update crc by padding-values
	unsigned char *pData = m_pReadBuffer->GetBegin();
	for (long l = 0; l < lPadSize; l++)
	{
        hcrc = m_crcio.UpdateCrc(hcrc, pData[l]);
	}
}


