//////////////////////////////////
//
// CFileType : file type detection
//
// Ilkka Prusi, 2010
//

#ifndef FILETYPE_H
#define FILETYPE_H

#include <stdint.h>

#include <string>
using namespace std;


// type of file data
// according to header in file
//
typedef enum tHeaderType
{
	HEADERTYPE_UNKNOWN = 0, // indeterminate or unsupported
	
	// "container" types, indeterminate "payload"
	HEADERTYPE_IFF_GENERIC, // IFF-format 
	HEADERTYPE_RIFF_GENERIC, // RIFF-format (similar, MS-specific)

	// archive (file collection) types
	HEADERTYPE_LHA, // LHA-archive (multi-file)
	HEADERTYPE_LZX, // LZX-archive (multi-file)
	HEADERTYPE_RAR, // RAR-archive (multi-file)
	HEADERTYPE_ZIP, // PK-zip and variants, 0x04034B50
	//HEADERTYPE_TAR, // "tape-archive" (POSIX) (multi-file)
	HEADERTYPE_ZOO, // 0xfdc4a7dc ?
	//HEADERTYPE_AR, // Un*x AR, LSAR?
	HEADERTYPE_CPIO, // Un*x CPIO
	//HEADERTYPE_SHAR, // Un*x shell-archive..
	//HEADERTYPE_STUFFIT, // MacOS Stuffit (SIT)
	//HEADERTYPE_PACKIT, // MacOS Packit (PIT)
	//HEADERTYPE_MACBINARY, // MacBinary (BIN)
	//HEADERTYPE_BINHEX, // MacOS BinHex (HQX)
	HEADERTYPE_ACE, // "**ACE**"
	//HEADERTYPE_ARC, // 
	HEADERTYPE_ARJ, // ARJ
	//HEADERTYPE_MSCAB, // Microsoft CAB-file
	
	// packer types
	HEADERTYPE_POWERPACKER, // Powerpacker, Amiga
	HEADERTYPE_IMPLODER, // Imploder-packed file (multiple ID variations..), Amiga
	HEADERTYPE_XPK_GENERIC, // XPK, multi-algorithm (SQSH etc.), Amiga
	HEADERTYPE_XFD_GENERIC, // XFD, multi-algorithm, Amiga
	HEADERTYPE_BZIP, // older BZip packer 
	HEADERTYPE_BZIP2, // BZip2 packer (bz2)
	HEADERTYPE_GZIP, // GNU-zip packer (gz)
	HEADERTYPE_ZCOMPRESS, // Un*X compress (Z)
	HEADERTYPE_7Z, // 7Z packer (7zip)
	HEADERTYPE_XZ, // XZ Utils packer (xz)
	HEADERTYPE_LPAK, // 'LPAK' (similar to 'SZDD')
	HEADERTYPE_SZDD, // 'SZDD' (Microsoft compress)

	// audio types, modules ("trackers")
    //HEADERTYPE_MOD_SOUNDTRACKER, // original SoundTracker format?
	HEADERTYPE_MOD_PROTRACKER, // ProTracker (and others..)
    HEADERTYPE_MOD_NOISETRACKER, // NoiseTracker
    HEADERTYPE_MOD_STARTREKKER, // StarTrekker
	HEADERTYPE_OCTAMED,	 // OctaMED "classic" (Music EDitor) (MED2..4)
    HEADERTYPE_OCTAMED_OSS,	 // OctaMED SoundStudio (MMD0..3, MED2..4)
	HEADERTYPE_DIGIBOOSTER,	 // old Digibooster DIGI-module
	HEADERTYPE_DBMPRO,	 // Digibooster PRO (DBM0)
	HEADERTYPE_SYMMOD,	 // Symphonie mod (SYMMOD)
    HEADERTYPE_TFMX,	 // TFMX, "The Final Musicsystem eXtended"
    HEADERTYPE_AHX, // "Abyss' Highest eXperience" (AHX0, AHX1, was called: THX)
    HEADERTYPE_OKTALYZER, // Oktalyzer
	HEADERTYPE_XM, // "eXtended Module", FastTracker II, DigiTracker..
    HEADERTYPE_IT, // ImpulseTracker
	HEADERTYPE_S3M, // ScreamTracker 3
    HEADERTYPE_MTM, // MultiTracker
    //HEADERTYPE_FC, // FutureComposer (FC13, FC14)
    //HEADERTYPE_BUZZ, // BuzzMachine ?
    //HEADERTYPE_MO3, // Module packed with MP3..
    HEADERTYPE_MADTRACKER2, // Mad Tracker 2
    
	// audio types, voice/sound
	HEADERTYPE_8SVX, // 8-bit sound "voice" (sample)(IFF)
	//HEADERTYPE_16VX, // 
	HEADERTYPE_MAUD, // 16-bit sound (IFF)
	//HEADERTYPE_RSND, // 
	HEADERTYPE_AIFF, // Audio-IFF (IFF-subtype)
	HEADERTYPE_WAVE, // wave-sound (RIFF)
	HEADERTYPE_MAESTRO, // Maestro (Samplitude?) (sample)
	//HEADERTYPE_AU, // audacity audio-file
	//HEADERTYPE_SND,
	//HEADERTYPE_OGG,
	//HEADERTYPE_MP2,
	//HEADERTYPE_MP3,

	// video, animation
	HEADERTYPE_ANIM, // Amiga ANIM (IFF)
	//HEADERTYPE_YAFA, // Wildfire ?
	HEADERTYPE_AVI, // MS-AVI video (RIFF)
	//HEADERTYPE_MOV, // Apple-Quicktime video
	//HEADERTYPE_MPEG, // 
	//HEADERTYPE_MATROSKA, // 
	
	// 3d-object types (3d-mesh)
	HEADERTYPE_LWOB, // older LWOB-format (IFF)
	HEADERTYPE_LWLO, // older LWOB with layers (IFF)
	HEADERTYPE_LWSC, // older LW scene (IFF)
	HEADERTYPE_LWO2, // newer LWO2-format (IFF)
	HEADERTYPE_TDDD, // Imagine TDDD-format (IFF)
	//HEADERTYPE_DXF,
	//HEADERTYPE_AUTOCAD,
	//HEADERTYPE_CINEMA4D,

	// picture types
	HEADERTYPE_ILBM, // ILBM-picture (IFF)
	HEADERTYPE_ACBM, // ACBM-picture (IFF): Amiga continuous BitMap
	//HEADERTYPE_DEEP, // IFF-DEEP (XiPaint)
	//HEADERTYPE_RGB8, // 
	//HEADERTYPE_RGBN, // 
	//HEADERTYPE_RGFX, // 
	//HEADERTYPE_SHAM, // 
	//HEADERTYPE_DHAM, // 
	//HEADERTYPE_DCOL, // 
	//HEADERTYPE_PCHG, // 
	//HEADERTYPE_PBM, // 
	//HEADERTYPE_YUVN, // 
	//HEADERTYPE_JPEG_JFIF, // separate alternates?
	//HEADERTYPE_JPEG_EXIF, // separate alternates?
	//HEADERTYPE_JPEG_SPIFF, // separate alternates?
	HEADERTYPE_JPEG,
	HEADERTYPE_PNG,
	HEADERTYPE_GIF,
	HEADERTYPE_TIFF,
	//HEADERTYPE_TARGA,
	//HEADERTYPE_ADF_PIC, // somewhere there is this kind of "picture" file?
	
	// document types
	//HEADERTYPE_PDF,
	//HEADERTYPE_EPS, // encapsulated postscript

	// disk images
	HEADERTYPE_ADFDOS_DISK, // amiga disk file format, TODO: is there header signature or just booblock of disk?
	HEADERTYPE_DMS_DISK, // Diskmasher compressed disk file, 0x444D5321 == DMS! (big-endian)
	HEADERTYPE_XMASH, // xMash
	HEADERTYPE_ZOOM_DISK, // Zoom disk archiver image
	//HEADERTYPE_ZAP_DISK, // ZAP disk cruncher
	//HEADERTYPE_WARP_DISK, // Warp disk cruncher
	//HEADERTYPE_XDM_DISK, // xDM compressed disk file
	//HEADERTYPE_AMIGA_HARDFILE,
	HEADERTYPE_ISOCD, // ISO-9660 CD/DVD-ROM image file
	//HEADERTYPE_MDF_CDIMAGE, //
	//HEADERTYPE_MAC_DMG, // MacOS DMG disk image
	
	// executables
	HEADERTYPE_AMIGAOS, // AmigaOS loadseg()ble executable/binary
	HEADERTYPE_AMIGAOSLIB, // AmigaOS object/library data
	HEADERTYPE_MSDOSWINDOWS, // MS-DOS,OS/2,Windows
	HEADERTYPE_UNIXELF, // Un*x ELF
	
	//HEADERTYPE_SFX_LHA, // self-extracting (Lha, msdos)

	// for future
	HEADERTYPE_RESERVED // (reserved for future use)
};

typedef enum tHeaderCategory
{
	HEADERCAT_UNKNOWN = 0, // indeterminate or unsupported

	HEADERCAT_PROGRAM, // ELF..
	
	// categories for type of data
	HEADERCAT_PICIMAGE, // ILBM/JPEG/TIFF..
	HEADERCAT_SOUND_MODULE, // 
	HEADERCAT_SOUND_VOICE, // 
	HEADERCAT_VIDEO_ANIM, // 
	HEADERCAT_ARCHIVE, // LHA/LZX/RAR/ZIP..
	HEADERCAT_PACKER, // PP/XPK/GZIP..
	HEADERCAT_FILEIMAGE, // ADF/DMS..
	HEADERCAT_3DOBJECT, // LWO?/TDDD/DXF..

	// "super-type" of format
	// on which actual format is based on,
	// more generic container type
	HEADERCAT_IFF,
	HEADERCAT_RIFF,
	HEADERCAT_AIFF
};


class CFileType
{
public:
	tHeaderType m_enFileType;
	tHeaderCategory m_enFileCategory;
	
	uint32_t Swap4(const uint32_t val) const
	{
		// 4-byte int byteswap
		return (
				((val & 0x000000FF) << 24) + ((val & 0x0000FF00) <<8) |
				((val & 0x00FF0000) >> 8) + ((val & 0xFF000000) >>24)
				);
	};
	
	//tHeaderType SubTypeFromHeader(const tHeaderType enType, const uint8_t *pBuffer, const uint32_t ulLength);
	
public:
    CFileType()
		: m_enFileType(HEADERTYPE_UNKNOWN)
		, m_enFileCategory(HEADERCAT_UNKNOWN)
	{}
	
	CFileType(const uint8_t *pBuffer, const uint32_t ulLength)
		: m_enFileType(HEADERTYPE_UNKNOWN)
		, m_enFileCategory(HEADERCAT_UNKNOWN)
	{
		m_enFileType = FileTypeFromHeader(pBuffer, ulLength);
		m_enFileCategory = FileCategoryByType(m_enFileType);
	}
			
    ~CFileType()
	{}

	tHeaderType FileTypeFromHeader(const uint8_t *pBuffer, const uint32_t ulLength) const;
	tHeaderCategory FileCategoryByType(const tHeaderType enType) const;

	void DetermineFileType(const uint8_t *pBuffer, const uint32_t ulLength);
	
	/*
	wstring GetNameOfType();
	wstring GetNameOfCategory();
	*/

};

#endif // FILETYPE_H
