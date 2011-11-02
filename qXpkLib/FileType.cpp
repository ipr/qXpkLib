#include "FileType.h"

/*
tHeaderType CFileType::SubTypeFromHeader(const tHeaderType enType, const uint8_t *pBuffer, const uint32_t ulLength)
{
	if (enType == HEADERTYPE_XPK_GENERIC)
	{
		// determine compression type
	}
	
	// give generic type if sub-type is not known
	return enType;
}
*/

tHeaderType CFileType::FileTypeFromHeader(const uint8_t *pBuffer, const uint32_t ulLength) const
{
	// invalid/small buffer/file
	// (file should have at least 16 bytes to have any meaningful data..)
	tHeaderType enFileType = HEADERTYPE_UNKNOWN;
	if (pBuffer == NULL
		|| ulLength < 16)
	{
		return HEADERTYPE_UNKNOWN;
	}
	
	// note: start with types which have least possibility of conflict
	// such as IFF "FORM", 
	// add ambigious types to end (such as only two-byte identifiers).

    // identifier in odd place..
    // and most common module-format also..
    if (ulLength >= 1084)
    {
        char *pData = (char*)(pBuffer + 1080);
        if (::memcmp(pData, "M.K.", 4) == 0
            || ::memcmp(pData, "M!K!", 4) == 0)
        {
            // should be ProTracker module..
            // could be anything else also..
            // we just don't know, variations also:
            // "6CHN", "8CHN"
            return HEADERTYPE_MOD_PROTRACKER;
        }
        else if (::memcmp(pData, "N.T.", 4) == 0)
        {
            // NoiseTracker variant of MOD..
            return HEADERTYPE_MOD_NOISETRACKER;
        }
        else if (::memcmp(pData, "FLT4", 4) == 0)
        {
            // Startrekker variant of MOD..
			// also old FastTracker ?
            return HEADERTYPE_MOD_STARTREKKER;
        }
        else if (::memcmp(pData, "FLT8", 4) == 0)
        {
            // Startrekker variant of MOD..
			// also old FastTracker ?
            return HEADERTYPE_MOD_STARTREKKER;
        }
        /*
        else
        {
            // !! old-style original SoundTracker
            // did not have identifier here..
            // how do we detect it??
            //
            //HEADERTYPE_MOD_SOUNDTRACKER
        }
        */
    }

	if (ulLength >= 48)
    {
        char *pData = (char*)(pBuffer + 44);
        if (::memcmp(pData, "SCRM", 4) == 0)
        {
            // ScreamTracker 3
            return HEADERTYPE_S3M;
        }
    }

    if (ulLength >= 24)
    {
		// 0..19  = "ZOO 2.10 Archive.<ctr>Z"
		// 20..23 = 0xfdc4a7dc
		//
        if (::memcmp(pBuffer, "ZOO ", 4) == 0
			&& pBuffer[23] == 0xfd
			&& pBuffer[22] == 0xc4
			&& pBuffer[21] == 0xa7
			&& pBuffer[20] == 0xdc
			)
        {
            return HEADERTYPE_ZOO;
        }
    }
	
    if (ulLength >= 20)
    {
        if (::memcmp(pBuffer, "DIGI Booster module\0", 20) == 0)
        {
            // starts with 20 byte string
            
            // old-style Digibooster module
            return HEADERTYPE_DIGIBOOSTER;
        }
    }

    if (ulLength >= 18)
    {
        if (::memcmp(pBuffer, "Extended module: ", 17) == 0)
        {
            // starts with 17 byte string
            
            // FastTracker II, DigiTracker..
            return HEADERTYPE_XM;
        }
    }
    
    if (ulLength >= 14)
    {
        char *pData = (char*)(pBuffer + 7);
        if (::memcmp(pData, "**ACE**", 7) == 0)
        {
			// ACE archiver
			return HEADERTYPE_ACE;
        }
    }
    if (ulLength >= 13)
    {
        char *pData = (char*)(pBuffer + 4);
        if (::memcmp(pData, "ZAP V1.41", 9) == 0)
        {
			// TODO: check, also with 4-byte ID at start?
			// Amiga ZAP disk-image
			return HEADERTYPE_ZAP_DISK;
        }
    }

    // LhA, Lzh variations..
    if (ulLength >= 8)
    {
        // many variations: skip two bytes
        // and look for string like
        // "-lh?-" or "-lz?-",
        // where '?' is number from 0..7
        // or character 's'..
        // 
        char *pData = (char*)pBuffer;
        if ((::memcmp(pData + 2, "-lh", 3) == 0
             || ::memcmp(pData + 2, "-lz", 3) == 0)
            && pData[6] == '-')
        {
            return HEADERTYPE_LHA;
        }
    }

	// ISOCD image may have empty boot sector 
	// -> check first actual sector
	// (expecting fixed sized sectors, fixed boot sector count)
	if (ulLength >= (2048*16 + 6))
	{
		if (::memcmp(pBuffer + (2048*16 +1), "CD001", 5) == 0)
		{
			return HEADERTYPE_ISOCD;
		}
	}
    else if (ulLength >= 6)
    {
		// check boot sector.. 
		// (we may skip this entirely if we know boot sector never has this?)
        if (::memcmp(pBuffer + 1, "CD001", 5) == 0)
        {
			return HEADERTYPE_ISOCD;
        }
    }
    
    // try to determine by string at start..
    if (::memcmp(pBuffer, "TFMX-SONG ", 10) == 0)
    {
        // Amiga TFMX module
        return HEADERTYPE_TFMX;
    }
	else if (::memcmp(pBuffer, "Warp v1.1", 9) == 0)
	{
		// Amiga Warp disk-image
		return HEADERTYPE_WARP_DISK;
	}
    else if (::memcmp(pBuffer, "OKTASONG", 8) == 0)
    {
        // Amiga Oktalyzer tracker module
        return HEADERTYPE_OKTALYZER;
    }
    /*
    else if (::memcmp(pBuffer, "ziRCONia", 8) == 0)
    {
		// MMCMP format packed file?
		// (compressed audio-module apparently..)
    }
    */
	else if (::memcmp(pBuffer, "MAESTRO", 7) == 0)
	{
		// Maestro (Samplitude?) sampleformat
		return HEADERTYPE_MAESTRO;
	}
	else if (::memcmp(pBuffer, "FORM", 4) == 0)
	{
		// EA-IFF format file header:
		// handle additional info for actual type,
		// values in file in big-endian format
	    enFileType = HEADERTYPE_IFF_GENERIC;

		const uint8_t *pTmp = (pBuffer +8);
		if (::memcmp(pTmp, "LWOB", 4) == 0)
		{
			// Lightwave, older (<6.5)
			return HEADERTYPE_LWOB;
		}
		else if (::memcmp(pTmp, "LWLO", 4) == 0)
		{
			// Lightwave, older (<6.5) with layers
			return HEADERTYPE_LWLO;
		}
		else if (::memcmp(pTmp, "LWSC", 4) == 0)
		{
			// Lightwave, older (<6.5) scene-format
			return HEADERTYPE_LWSC;
		}
		else if (::memcmp(pTmp, "LWO2", 4) == 0)
		{
			// Lightwave, newer (>=6.5)
			return HEADERTYPE_LWO2;
		}
		else if (::memcmp(pTmp, "TDDD", 4) == 0)
		{
			// Imagine
			return HEADERTYPE_TDDD;
		}
		else if (::memcmp(pTmp, "ILBM", 4) == 0)
		{
			return HEADERTYPE_ILBM;
		}
		else if (::memcmp(pTmp, "ACBM", 4) == 0)
		{
			return HEADERTYPE_ACBM;
		}
		else if (::memcmp(pTmp, "8SVX", 4) == 0)
		{
			// 8SVX-audio
			return HEADERTYPE_8SVX;
		}
		else if (::memcmp(pTmp, "MAUD", 4) == 0)
		{
			// 16-bit IFF audio
			return HEADERTYPE_MAUD;
		}
		else if (::memcmp(pTmp, "AIFF", 4) == 0)
		{
			// sub-type of EA-IFF-format
			// for "Audio IFF"
			return HEADERTYPE_AIFF;
		}
		else if (::memcmp(pTmp, "ANIM", 4) == 0)
		{
			// Amiga ANIM
			return HEADERTYPE_ANIM;
		}
		return enFileType;
	}
    else if (::memcmp(pBuffer, "RIFF", 4) == 0)
	{
		// note: also RIFX for big-endian version?
		
		// Microsoft/IBM counterpart to EA-IFF format:
		// values in file in little-endian format
	    enFileType = HEADERTYPE_RIFF_GENERIC;
	    
	    const uint8_t *pTmp = (pBuffer +8);
	    if (::memcmp(pTmp, "WAVE", 4) == 0)
	    {
			enFileType = HEADERTYPE_WAVE;
	    }
	    else if (::memcmp(pTmp, "AVI ", 4) == 0)
		{
			enFileType = HEADERTYPE_AVI;
		}
		/*
	    else if (::memcmp(pTmp, "CDDA", 4) == 0)
		{}
		*/
		return enFileType;
	}
	else if (::memcmp(pBuffer, "XPKF", 4) == 0)
	{
		// XPK-packer (generic packer),
		// determine actual packer..
	    enFileType = HEADERTYPE_XPK_GENERIC;

		/* // test generic detection -> commented out
		const uint8_t *pTmp = (pBuffer +8);
		if (::memcmp(pTmp, "SQSH", 4) == 0)
		{
			return HEADERTYPE_XPK_SQSH;
		}
		else if (::memcmp(pTmp, "NUKE", 4) == 0)
		{
			return HEADERTYPE_XPK_NUKE;
		}
		else if (::memcmp(pTmp, "RLEN", 4) == 0)
		{
			return HEADERTYPE_XPK_RLEN;
		}
		*/
		return enFileType;
	}
	else if (::memcmp(pBuffer, "XFDD", 4) == 0)
	{
		// (identifier from XPK-master..)
		// XFD-library packed,
		// also multi-algorithm packing..
		return HEADERTYPE_XFD_GENERIC;
	}
	else if (::memcmp(pBuffer, "XFDF", 4) == 0)
	{
		// (identifier from XFD-master..)
		// XFD-library packed,
		// also multi-algorithm packing..
		return HEADERTYPE_XFD_GENERIC;
	}
	else if (::memcmp(pBuffer, "PP20", 4) == 0)
	{
		// powerpacker
		return HEADERTYPE_POWERPACKER;
	}
	else if (::memcmp(pBuffer, "IMP!", 4) == 0)
	{
		// imploder
		return HEADERTYPE_IMPLODER;
	}
    else if (::memcmp(pBuffer, "DIGI", 4) == 0)
	{
        // actually, starts with 20 byte string:
        // "DIGI Booster module\0"
        
		// old-style Digibooster module
		return HEADERTYPE_DIGIBOOSTER;
	}
	else if (::memcmp(pBuffer, "DBM0", 4) == 0)
	{
		// Digibooster PRO module
		return HEADERTYPE_DBMPRO;
	}
	else if (::memcmp(pBuffer, "SymM", 4) == 0)
	{
		// Symphonie module
		return HEADERTYPE_SYMMOD;
	}
    else if (::memcmp(pBuffer, "MT20", 4) == 0)
    {
        // Mad Tracker 2
        return HEADERTYPE_MADTRACKER2;
    }
    else if (::memcmp(pBuffer, "IMPM", 4) == 0)
	{
		// ImpulseTracker module
		return HEADERTYPE_IT;
	}
    /*
    else if (::memcmp(pBuffer, "IMPI", 4) == 0)
	{
		// ImpulseTracker instrument?
		return HEADERTYPE_IT;
	}
    else if (::memcmp(pBuffer, "IMPS", 4) == 0)
	{
		// ImpulseTracker sample?
		return HEADERTYPE_IT;
	}
    */
	else if (::memcmp(pBuffer, "LPAK", 4) == 0)
	{
		// stand-alone compression format
		return HEADERTYPE_LPAK;
	}
	else if (::memcmp(pBuffer, "SZDD", 4) == 0)
	{
		// stand-alone compression format (Microsoft compress)
		return HEADERTYPE_SZDD;
	}
	/* check
	else if (::memcmp(pBuffer, "MSCF", 4) == 0)
	{
		// Microsoft CAB-file
		return HEADERTYPE_MSCAB;
	}
	*/
	else if (::memcmp(pBuffer, "ZOOM", 4) == 0
		|| ::memcmp(pBuffer, "ZOM5", 4) == 0)
	{
		// 0x5A4F4F4D
		// Amiga Zoom disk archiver
		return HEADERTYPE_ZOOM_DISK;
	}
	else if (::memcmp(pBuffer, "LZX", 3) == 0)
	{
		// LZX archive
		return HEADERTYPE_LZX;
	}
	else if (pBuffer[0] == 0x52
			&& pBuffer[1] == 0x61
			&& pBuffer[2] == 0x72
			&& pBuffer[3] == 0x21
			&& pBuffer[4] == 0x1a
			&& pBuffer[5] == 0x07
			&& pBuffer[6] == 0x00)
	{
		// actually, two formats:
		// old : 0x52 0x45 0x7e 0x5e
		// new : 0x52 0x61 0x72 0x21 0x1a 0x07 0x00
		//
		// Rar-archive (newer)
		return HEADERTYPE_RAR;
	}
	else if (pBuffer[0] == 0x52
			&& pBuffer[1] == 0x45
			&& pBuffer[2] == 0x7e
			&& pBuffer[3] == 0x5e)
	{
		// actually, two formats:
		// old : 0x52 0x45 0x7e 0x5e
		// new : 0x52 0x61 0x72 0x21 0x1a 0x07 0x00
		//
		// Rar-archive (older)
		return HEADERTYPE_RAR;
	}
    else if (::memcmp(pBuffer, "MED", 3) == 0)
    {
        // OctaMED/Pro
        // MED2..4
        return HEADERTYPE_OCTAMED;
    }
	else if (::memcmp(pBuffer, "MMD", 3) == 0)
	{
		// OctaMED SoundStudio
		// MMD0..3
        return HEADERTYPE_OCTAMED_OSS;
	}
    else if (::memcmp(pBuffer, "THX", 3) == 0)
    {
        // AHX (which was previously THX)
        // sound module
        return HEADERTYPE_AHX;
    }
    else if (::memcmp(pBuffer, "MTM", 3) == 0)
    {
        // MultiTracker sound module
        return HEADERTYPE_MTM;
    }
	else if (::memcmp(pBuffer, "DOS", 3) == 0)
	{
		// AmigaDOS-disk? (ADF: raw image of disk)
		enFileType = HEADERTYPE_ADFDOS_DISK;
		/*
		if (pBuffer[3] == 0)
		{
			// OFS
		}
		else if (pBuffer[3] == 1)
		{
			// FFS
		}
		*/
		return enFileType;
	}
	
	if (pBuffer[0] == 'B'
		&& pBuffer[1] == 'Z'
		&& pBuffer[2] == '0')
	{
		// "BZ0", older BZip format
		return HEADERTYPE_BZIP;
	}
	else if (pBuffer[0] == 'B'
		&& pBuffer[1] == 'Z'
		&& pBuffer[2] == 'h')
	{
		// "BZh?", where ? = 0..9
		// for variations: "BZh1".."BZh9"
		return HEADERTYPE_BZIP2;
	}
	else if (pBuffer[0] == 0x1F
		&& pBuffer[1] == 0x8B
		&& pBuffer[2] == 0x08)
	{
		// actually, two bytes at start for ID,
		// third for compression mode:
		// only "deflate" (0x8) is supported?
		// -> see http://www.gzip.org/zlib/rfc-gzip.html
		return HEADERTYPE_GZIP;
	}
	else if (pBuffer[0] == 0xFF
		 && pBuffer[1] == 0xD8
		 && pBuffer[2] == 0xFF)
	{
		// next byte should be one of: E0, E1, E8 ?
		// also, should follow with "JFIF" ?
		return HEADERTYPE_JPEG;
	}
	else if (pBuffer[0] == '7'
			 && pBuffer[1] == 'z'
			 && pBuffer[2] == 0xBC
			 && pBuffer[3] == 0xAF
			 && pBuffer[4] == 0x27
			 && pBuffer[5] == 0x1C)
	{
		return HEADERTYPE_7Z;
	}
	else if (pBuffer[0] ==  0xFD
			 && pBuffer[1] == '7'
			 && pBuffer[2] == 'z'
			 && pBuffer[3] == 'X'
			 && pBuffer[4] == 'Z'
			 && pBuffer[5] == 0x00)
	{
		return HEADERTYPE_XZ;
	}
	else if (pBuffer[0] == 'M'
		 && pBuffer[1] == 'S'
		 && pBuffer[2] == 'H')
	{
		// xMash
		return HEADERTYPE_XMASH;
	}
	else if (pBuffer[0] == 0x1F
		&& pBuffer[1] == 0x9D)
	{
		// Unix-compress (really old..)
		/*
		if (pBuffer[2] & 0x60)
		{
			// unknown format (unsupported variation?)
		}
		else if ((pBuffer[2] & 0x1f) > 16)
		{
			// too many bits -> unsupported
		}
		*/
		return HEADERTYPE_ZCOMPRESS;
	}
	else if (pBuffer[0] == 'H'
		&& pBuffer[1] == 'A')
	{
		return HEADERTYPE_HA;
	}
	else if (pBuffer[0] == 0x60
		&& pBuffer[1] == 0xEA)
	{
		// ARJ
		return HEADERTYPE_ARJ;
	}
	/*
	else if (pBuffer[0] == 0x1F
		&& pBuffer[1] == 0x0A)
	{
		// LHA-compressed TAR?
		//enFileType = ;
	}
	*/
	else if (pBuffer[0] == 0x4D
		&& pBuffer[1] == 0x5A)
	{
		// 'MZ' executable header
		return HEADERTYPE_MSDOSWINDOWS;
	}
	
	// try to determine from first four bytes.. (in case "magic number" as non-ASCII chars)
	// note: needs byteswap..
	//
	uint32_t ulFirstFour = Swap4((*((uint32_t*)pBuffer)));
	if (ulFirstFour == 0x444D5321)
	{
		// note: big-endian
		// 0x444D5321 // DMS! (big-endian)
		return HEADERTYPE_DMS_DISK;
	}
	else if (ulFirstFour == 0x44494D50)
	{
		// "DImp", disk-imploder (packed floppy image)
		return HEADERTYPE_DISK_IMPLODER;
	}
	else if (ulFirstFour == 0x504B0304)
	{
		// "PK"
		return HEADERTYPE_ZIP;
	}
	else if (ulFirstFour == 0x89504E47)
	{
		return HEADERTYPE_PNG;
	}
	else if (ulFirstFour == 0x47494638)
	{
		enFileType = HEADERTYPE_GIF;
		// sub-types of GIF?
		/*
		if (::memcmp(pBuffer, "GIF87a", 6) == 0)
		{}
		else if (::memcmp(pBuffer, "GIF89a", 6) == 0)
		{}
		*/
		return enFileType;
	}
	// note of TIFF: may also be 49492A00 ?
	else if (ulFirstFour == 0x4D4D002A)
	{
		// big-endian TIFF
		return HEADERTYPE_TIFF;
	}
	else if (ulFirstFour == 0x49492A00)
	{
		// little-endian TIFF
		return HEADERTYPE_TIFF;
	}
	else if (ulFirstFour == 0x7F454C46)
	{
		// executable (multi-platform)
		return HEADERTYPE_UNIXELF;
	}
	else if (ulFirstFour == 0x000003F3)
	{
		return HEADERTYPE_AMIGAOS;
	}
	else if (ulFirstFour == 0x000003E7)
	{
		return HEADERTYPE_AMIGAOSLIB;
	}

	// various imploder-variations/clones, only ID is different?
	// check for magic ID 'IMP!', or one of the other IDs used by Imploder 
	// clones; ATN!, BDPI, CHFI, Dupa, EDAM, FLT!, M.H., PARA and RDC9 */ 
	if (ulFirstFour == 0x494d5021 || ulFirstFour == 0x41544e21 || ulFirstFour == 0x42445049
		|| ulFirstFour == 0x43484649 || ulFirstFour == 0x44757061 || ulFirstFour == 0x4544414d
		|| ulFirstFour == 0x464c5421 || ulFirstFour == 0x4d2e482e || ulFirstFour == 0x50415241
		|| ulFirstFour == 0x52444339)
	{
		// imploder
		return HEADERTYPE_IMPLODER;
	}
	
	// note: big-endian and little-endian variants
	// -> should check both ways..
	// identifier is defined as octal value 070707
	// -> that is crap, change to proper hex for less mistakes
	// (who the hell uses octal values any more ?)
	uint16_t *pId = (uint16_t*)pBuffer;
	if (*pId == 0x71C7
		|| *pId == 0xC771)
	{
		// Un*x CPIO (old format?)
		return HEADERTYPE_CPIO;
	}
	// there's also two newer CPIO-formats which have ASCII-based identifiers?
	
	/*
	// TAR (POSIX)	.tar	75 73 74 61 72	ustar (offset by 257 bytes)
	*/

	return enFileType;
}

// temp..
tHeaderCategory CFileType::FileCategoryByType(const tHeaderType enType) const
{
	switch (enType)
	{
	case HEADERTYPE_LHA:
	case HEADERTYPE_LZX:
	case HEADERTYPE_RAR:
	case HEADERTYPE_ZIP:
	//case HEADERTYPE_TAR:
	//case HEADERTYPE_ZOO:
	//case HEADERTYPE_ARC:
	//case HEADERTYPE_ARJ:
		return HEADERCAT_ARCHIVE;
		
	case HEADERTYPE_POWERPACKER:
	case HEADERTYPE_IMPLODER:
	case HEADERTYPE_XPK_GENERIC:
	//case HEADERTYPE_XPK_SQSH:
	//case HEADERTYPE_XPK_NUKE:
	//case HEADERTYPE_XPK_RLEN:
	case HEADERTYPE_XFD_GENERIC:
	case HEADERTYPE_GZIP:
	case HEADERTYPE_BZIP2:
	case HEADERTYPE_ZCOMPRESS:
	case HEADERTYPE_7Z:
	case HEADERTYPE_XZ:
	case HEADERTYPE_LPAK:
	case HEADERTYPE_SZDD:
		return HEADERCAT_PACKER;
		
    //case HEADERTYPE_MOD_SOUNDTRACKER:
	case HEADERTYPE_MOD_PROTRACKER:
    case HEADERTYPE_MOD_NOISETRACKER:
    case HEADERTYPE_MOD_STARTREKKER:
	case HEADERTYPE_OCTAMED:
    case HEADERTYPE_OCTAMED_OSS:
	case HEADERTYPE_DIGIBOOSTER:
	case HEADERTYPE_DBMPRO:
	case HEADERTYPE_SYMMOD:
    case HEADERTYPE_TFMX:
    case HEADERTYPE_AHX:
    case HEADERTYPE_OKTALYZER:
    case HEADERTYPE_XM:
	case HEADERTYPE_IT:
	case HEADERTYPE_S3M:
    case HEADERTYPE_MTM:
    //case HEADERTYPE_FC:
    //case HEADERTYPE_MO3:
    case HEADERTYPE_MADTRACKER2:
		return HEADERCAT_SOUND_MODULE;
		
	case HEADERTYPE_8SVX:
	case HEADERTYPE_MAUD:
	case HEADERTYPE_AIFF:
	case HEADERTYPE_WAVE:
	case HEADERTYPE_MAESTRO:
	//case HEADERTYPE_AU:
	//case HEADERTYPE_SND:
	//case HEADERTYPE_OGG:
	//case HEADERTYPE_MP2:
	//case HEADERTYPE_MP3:
		return HEADERCAT_SOUND_VOICE;
		
	case HEADERTYPE_ANIM:
	//case HEADERTYPE_YAFA:
	case HEADERTYPE_AVI:
	//case HEADERTYPE_MOV:
	//case HEADERTYPE_MPEG:
	//case HEADERTYPE_MATROSKA:
		return HEADERCAT_VIDEO_ANIM;
		
	case HEADERTYPE_LWOB:
	case HEADERTYPE_LWLO:
	case HEADERTYPE_LWSC:
	case HEADERTYPE_LWO2:
	case HEADERTYPE_TDDD:
	//case HEADERTYPE_DXF:
	//case HEADERTYPE_AUTOCAD:
	//case HEADERTYPE_CINEMA4D:
		return HEADERCAT_3DOBJECT;
		
	case HEADERTYPE_ILBM:
	case HEADERTYPE_ACBM:
	case HEADERTYPE_JPEG:
	case HEADERTYPE_PNG:
	case HEADERTYPE_GIF:
	case HEADERTYPE_TIFF:
		return HEADERCAT_PICIMAGE;
		
	case HEADERTYPE_ADFDOS_DISK:
	case HEADERTYPE_DMS_DISK:
	//case HEADERTYPE_XDM_DISK:
		return HEADERCAT_FILEIMAGE;
		
	case HEADERTYPE_AMIGAOS:
	case HEADERTYPE_AMIGAOSLIB:
	case HEADERTYPE_MSDOSWINDOWS:
	case HEADERTYPE_UNIXELF:
		return HEADERCAT_PROGRAM;
		
	}
	return HEADERCAT_UNKNOWN;
}

void CFileType::DetermineFileType(const uint8_t *pBuffer, const uint32_t ulLength)
{
	// determine type from header (if possible)
	if (ulLength >= 12)
	{
		// when at least header should be availalbe,
		// see if file-type can be determined from it
		m_enFileType = FileTypeFromHeader(pBuffer, ulLength);
		m_enFileCategory = FileCategoryByType(m_enFileType);
	}
}

/*
wstring CFileType::GetNameOfType()
{
	switch (m_enFileType)
	{
	case HEADERTYPE_LHA:
		return _T("LhA");
	case HEADERTYPE_LZX:
		return _T("LZX");
	case HEADERTYPE_POWERPACKER:
		return _T("PowerPacker");
	case HEADERTYPE_IMPLODER:
		return _T("Imploder");
	case HEADERTYPE_XPK_GENERIC:
		return _T("XPK");
	case HEADERTYPE_XPK_SQSH:
		return _T("XPK (SQSH)");
	case HEADERTYPE_XPK_NUKE:
		return _T("XPK (NUKE)");
	case HEADERTYPE_XPK_RLEN:
		return _T("XPK (RLEN)");

	case HEADERTYPE_RAR:
		return _T("RAR");
	case HEADERTYPE_ZIP:
		return _T("PK-Zip");
	case HEADERTYPE_GZIP:
		return _T("GNU-Zip");
	case HEADERTYPE_BZIP2:
		return _T("BZip2");
	case HEADERTYPE_7Z:
		return _T("7-Zip");
	case HEADERTYPE_XZ:
		return _T("XZ");
	case HEADERTYPE_ZCOMPRESS:
		return _T("Z-compress");
		
		
	case HEADERTYPE_ADFDOS_DISK:
		return _T("AmigaDOS disk-file");
	case HEADERTYPE_DMS_DISK:
		return _T("DiskMasher disk-file");
	//case HEADERTYPE_XDM_DISK:
		//return _T("xDM disk-file");
	}
	
	return wstring();
}
*/

/*
wstring CFileType::GetNameOfCategory()
{
	switch (m_enFileCategory)
	{
	case HEADERCAT_ARCHIVE:
		return _T("Archive");
	case HEADERCAT_PACKER:
		return _T("Packer/Compressor");
	}
	return wstring();
}
*/

