/////////////////////////////////////
//
// misc. file-mode related flags
// and helper-structures to handle them 
// (used in LhA headers).
//

#ifndef FILEMODEFLAGS_H
#define FILEMODEFLAGS_H

#include <stdint.h>

typedef struct MsdosFlags
{
	// constructor
	MsdosFlags()
	{
		SetFromValue(0x20);
	}
	
	// some shitty MS-DOS-style flags (who cares about this?)
	bool bRo;  // bit1  read only
	bool bHid; // bit2  hidden
	bool bSys; // bit3  system
	bool bVol; // bit4  volume label
	bool bDir; // bit5  directory
	bool bArc; // bit6  archive bit (need to backup)
	
	void SetFromValue(const uint8_t ucVal)
	{
		bRo = ((ucVal & 2) ? true : false);
		bHid = ((ucVal & 4) ? true : false);
		bSys = ((ucVal & 8) ? true : false);
		bVol = ((ucVal & 16) ? true : false);
		bDir = ((ucVal & 32) ? true : false);
		bArc = ((ucVal & 64) ? true : false);
	}
	
} MsdosFlags;


typedef struct UnixModeFlags
{
	enum tUnixFlags
	{
		UNIX_FILE_TYPEMASK     = 0170000,
		UNIX_FILE_REGULAR      = 0100000,
		UNIX_FILE_DIRECTORY    = 0040000,
		UNIX_FILE_SYMLINK      = 0120000,
		UNIX_SETUID            = 0004000,
		UNIX_SETGID            = 0002000,
		UNIX_STICKYBIT         = 0001000,
		UNIX_OWNER_READ_PERM   = 0000400,
		UNIX_OWNER_WRITE_PERM  = 0000200,
		UNIX_OWNER_EXEC_PERM   = 0000100,
		UNIX_GROUP_READ_PERM   = 0000040,
		UNIX_GROUP_WRITE_PERM  = 0000020,
		UNIX_GROUP_EXEC_PERM   = 0000010,
		UNIX_OTHER_READ_PERM   = 0000004,
		UNIX_OTHER_WRITE_PERM  = 0000002,
		UNIX_OTHER_EXEC_PERM   = 0000001,
		UNIX_RW_RW_RW          = 0000666
	};

	// mode-flags as-is
	unsigned short  unix_mode;
	
	// parsed flags
	bool isDir;
	bool isSymlink;
	bool isRegular;

	bool isOwnerRead;
	bool isOwnerWrite;
	//bool isOwnerExec;

	bool isGroupRead;
	bool isGroupWrite;
	//bool isGroupExec;
	
	bool isOtherRead;
	bool isOtherWrite;
	//bool isOtherExec;

	/*
	bool isSetUID;
	bool isSetGID;
	bool isSticky;
	*/

	// constructor
	UnixModeFlags()
	{
		// default: normal file with read+write for everyone
		unix_mode = UNIX_FILE_REGULAR | UNIX_RW_RW_RW;
		
		isDir = false;
		isSymlink = false;
		
		/*
		isSetUID = false;
		isSetGID = false;
		isSticky = false;
		*/
	}
	
	void ParseMode(const uint16_t mode)
	{
		unix_mode = mode;
		
		isDir =		(mode & (int)UNIX_FILE_DIRECTORY) ? true : false;
		
		// may have file && symlink flags?
		isSymlink = ((mode & (int)UNIX_FILE_SYMLINK) == UNIX_FILE_SYMLINK) ? true : false;
		isRegular = (mode & (int)UNIX_FILE_REGULAR) ? true : false;
		
		isOwnerRead = (mode & (int)UNIX_OWNER_READ_PERM) ? true : false;
		isOwnerWrite = (mode & (int)UNIX_OWNER_WRITE_PERM) ? true : false;
		isGroupRead = (mode & (int)UNIX_GROUP_READ_PERM) ? true : false;
		isGroupWrite = (mode & (int)UNIX_GROUP_WRITE_PERM) ? true : false;
		isOtherRead = (mode & (int)UNIX_OTHER_READ_PERM) ? true : false;
		isOtherWrite = (mode & (int)UNIX_OTHER_WRITE_PERM) ? true : false;
	}
	
} UnixModeFlags;

// these match Amiga-style file-attributes
// (protection mode flags):
// HSPA RWED
//
// is there "extended header" with these somewhere in LhA variations?
//
typedef struct AmigaAttributes
{
	// parsed file-protection flags
	bool h; // 'hidden'
	bool s; // 'script'
	bool p; // 'pure' (can be made resident in-memory)
	bool a; // 'archived'
	bool r; // 'readable'
	bool w; // 'writable'
	bool e; // 'executable'
	bool d; // 'delete'

	// protection flags "as-is"
	// TODO: check if LhA way is compatible..
	unsigned char m_ucAttribs;

	// protection flags masks
	enum tAmigaProtectionFlags
	{
		APF_PROT_READ    = 1,
		APF_PROT_WRITE   = 2,
		APF_PROT_DELETE  = 4,
		APF_PROT_EXECUTE = 8,
		APF_PROT_ARCHIVE = 16,
		APF_PROT_HOLD    = 32,
		APF_PROT_SCRIPT  = 64,
		APF_PROT_PURE    = 128
	};
	
	// constructor
	AmigaAttributes(void)
	{
		h = false;
		s = false;
		p = false;
		a = false;

		r = false;
		w = false;
		e = false;
		d = false;
		m_ucAttribs = 0;
	}
	
	/* parse file protection modes */
	void ParseAttributes(const uint8_t ucAttribs)
	{
		m_ucAttribs = ucAttribs;
		h = ((m_ucAttribs & (int)APF_PROT_HOLD)    ? true : false);
		s = ((m_ucAttribs & (int)APF_PROT_SCRIPT)  ? true : false);
		p = ((m_ucAttribs & (int)APF_PROT_PURE)    ? true : false);
		a = ((m_ucAttribs & (int)APF_PROT_ARCHIVE) ? true : false);
		r = ((m_ucAttribs & (int)APF_PROT_READ)    ? true : false);
		w = ((m_ucAttribs & (int)APF_PROT_WRITE)   ? true : false);
		e = ((m_ucAttribs & (int)APF_PROT_EXECUTE) ? true : false);
		d = ((m_ucAttribs & (int)APF_PROT_DELETE)  ? true : false);
	}

} AmigaAttributes;

// placeholder: no idea of contents yet..
//
// HFS/HFS+ protection flags:
// t (<TYPE>)
// c (<CREA>) creator, name/ID?
// i (bool) invisible (hidden)
// l (bool) locked (read-only?)
//
/*
typedef struct MacOSHFSAttributes
{
	// constructor
	MacOSHFSAttributes(void)
	{
	}

	void ParseAttributes(const uint16_t attribs)
	{
	}
	
} MacOSHFSAttributes;
*/

#endif // FILEMODEFLAGS_H
