//////////////////////////////////////////////////////
//
// CUnZoo :
// Ilkka Prusi <ilkka.prusi@gmail.com>
//
// Rewrite of unzoo.c by Martin Schoenert
// 
// This file in public domain.
//
/*
*  unzoo.c by Martin Schoenert
*
*  @(#)$Id: unzoo.c,v 4.4 2000/05/29 08:56:57 sal Exp $
*
*  This file is in the Public Domain.
*
* Modified for use in C++ library by Ilkka Prusi <ilkka.prusi@gmail.com>
*
*/

#include "UnZoo.h"

#include "GenericTime.h" // conversion helper

/****************************************************************************
**
*A  unzoo.c                     Tools                        Martin Schoenert
**
*H  @(#)$Id: unzoo.c,v 4.4 2000/05/29 08:56:57 sal Exp $
**
*Y  This file is in the Public Domain.
**
**  SYNTAX
**
**  'unzoo'
**  'unzoo [-l] [-v] <archive>[.zoo] [<file>..]'
**  'unzoo -x [-abnpo] <archive>[.zoo] [<file>..]'
**
**  DESCRIPTION
**
**  'unzoo' is  a zoo  archive extractor.   A zoo archive   is  a  file  that
**  contains several files, called its members, usually in compressed form to
**  save space.  'unzoo' can list all or  selected members or  extract all or
**  selected members, i.e.,  uncompress them and write   them  to files.   It
**  cannot add new members or  delete  members.  For this   you need the  zoo
**  archiver, called 'zoo', written by Rahul Dhesi.
**
**  If you call 'unzoo'  with no arguments, it will  first print a summary of
**  the commands and  then prompt for  command lines interactively, until you
**  enter an empty line.  This is useful  on systems  that do not support the
**  notion of command line arguments such as the Macintosh.
**
**  If you call  'unzoo' with the  '-l' option,  it lists the  members in the
**  archive <archive>.   For each member 'unzoo'   prints  the size  that the
**  extracted file  would  have, the  compression factor,  the  size that the
**  member occupies in the archive (not  counting  the  space needed to store
**  the attributes such as the path name of the file), the date and time when
**  the files were last modified, and finally  the path name itself.  Finally
**  'unzoo' prints a grand total for the  file sizes, the compression factor,
**  and the member sizes.
**
**  The '-v' suboption causes 'unzoo' to append to each path name,  separated
**  by a ';', the generation number of the member,  where higher numbers mean
**  later generations.  Members for which generations are disabled are listed
**  with  ';0'.  Also 'unzoo'   will print the  comments associated  with the
**  archive itself or the members, preceeded by the string '# '.
**
**  If you call 'unzoo' with the '-x' option,  it extracts the  members  from
**  the archive <archive>.  Members are  stored with a  full path name in the
**  archive and if the operating system supports this, they will be extracted
**  into   appropriate subdirectories,   which will   be  created on  demand.
**  The members are usually  extracted as binary files,  with no translation.
**  However, if a member has a  comment that starts with the string '!TEXT!',
**  it is  extracted as a  text file, i.e.,  it will be  translated from  the
**  universal text file format (with <lf> as line separator as under UNIX) to
**  the local text file format (e.g., with <cr>/<lf> as separator under DOS).
**  If the archive  itself has a  comment that starts with  '!TEXT!' then all
**  members will be extracted as text files, even those that have no comment.
**  For each member the name is printed followed by  '-- extracted as binary'
**  or '-- extracted as text' when the member has been completely extracted.
**
**  The '-a' suboption causes  'unzoo' to extract all members  as text files,
**  even if they have no comment starting with  '!TEXT!'.
**
**  The '-b' suboption causes 'unzoo' to extract all members as binary files,
**  even if they have a comment starting with  '!TEXT!'.
**
**  The '-n' suboption causes 'unzoo' to suppress writing the files.  You use
**  this suboption  to test the integrity  of the archive  without extracting
**  the members.  For each member the name is printed followed by '-- tested'
**  if the member is intact or by '-- error, CRC failed' if it is not.
**
**  The '-p' suboption causes 'unzoo' to print the files to stdout instead of
**  writing them to files.
**
**  The '-o'  suboption causes 'unzoo'   to overwrite existing  files without
**  asking  you for confirmation.   The  default is  to ask for  confirmation
**  '<file> exists, overwrite it? (Yes/No/All/Ren)'.   To this you can answer
**  with 'y' to overwrite the  file, 'n' to skip  extraction of the file, 'a'
**  to overwrite this and all following files, or 'r' to enter a new name for
**  the file.  'unzoo' will never overwrite existing read-only files.
**
**  The '-j <prefix>' suboption causes 'unzoo' to prepend the string <prefix>
**  to  all path names for  the members  before  they  are extracted.  So for
**  example if an archive contains absolute  path names under  UNIX,  '-j ./'
**  can be used to convert them to relative pathnames.   This option  is also
**  useful  on  the Macintosh where   you start 'unzoo' by clicking,  because
**  then the current directory will be the one where 'unzoo' is,  not the one
**  where the  archive is.   Note  that the  directory  <prefix> must  exist,
**  'unzoo' will not create it on demand.
**
**  If no  <files>  argument is given all members  are  listed or  extracted.
**  If  one or  more <files>  arguments are given,  only members whose  names
**  match at least one of  the  <files> patterns  are  listed  or  extracted.
**  <files> can  contain the wildcard   '?', which  matches any character  in
**  names, and '*', which  matches any number  of characters  in names.  When
**  you pass the <files> arguments on the command  line you will usually have
**  to quote them to keep the shell from trying to expand them.
**
**  Usually 'unzoo' will  only list or extract the  latest generation of each
**  member.  But if you append ';<nr>' to a path  name pattern the generation
**  with the number <nr> is listed or extracted.  <nr> itself can contain the
**  wildcard characters '?' and '*', so appending ';*' to a path name pattern
**  causes all generations to be listed or extracted.
**
**
**  COMPATIBILITY
**
**  'unzoo'  is based heavily on the 'booz' archive extractor by Rahul Dhesi.
**  I basically stuffed everything in one file (so  no 'Makefile' is needed),
**  cleaned it up (so that it is now more portable and  a little bit faster),
**  and added the  support for  long file names,  directories,  and comments.
**
**  'unzoo' differs in some details from  'booz' and the zoo archiver  'zoo'.
**
**  'unzoo' can  only list  and extract members   from archives, like 'booz'.
**  'zoo' can also add members, delete members, etc.
**
**  'unzoo' can extract members as text files, converting from universal text
**  format to the local text format,  if the '-a' option is given or the '-b'
**  option is not given and the  member has a comment starting with '!TEXT!'.
**  So in the absence of the '-a' option and comments starting with '!TEXT!',
**  'unzoo' behaves like  'zoo' and 'booz',  which always extract as  binary.
**  But  'unzoo' can  correctly extract  text files from  archives that  were
**  created under UNIX (or other systems using the universal text format) and
**  extended with '!TEXT!' comments on systems such as DOS, VMS, Macintosh.
**
**  'unzoo' can handle  long names, which it converts  in  a system dependent
**  manner to local  names, like  'zoo'  (this may not   be available on  all
**  systems).  'booz' always uses the short DOS format names.
**
**  'unzoo' extracts  members  into  subdirectories, which  it  automatically
**  creates, like 'zoo' (this  may not be available on  all systems).  'booz'
**  always extracts all members into the current directory.
**
**  'unzoo'  can handle comments and generations in the  archive, like 'zoo'.
**  'booz' ignores all comments and generations.
**
**  'unzoo' cannot handle  members compressed with  the old method, only with
**  the new  high method or  not compressed  at all.   'zoo' and  'booz' also
**  handle members compress with the old method.  This shall be fixed soon.
**
**  'unzoo' can handle archives in  binary format under  VMS, i.e., it is not
**  necessary to convert  them to stream linefeed  format  with 'bilf' first.
**  'zoo' and 'booz' require this conversion.
**
**  'unzoo' is somewhat faster than 'zoo' and 'booz'.
**
**  'unzoo' should be much easier to port than both 'zoo' and 'booz'.
**
**  COMPILATION
**
**  Under  UNIX  with the  standard  C compiler,  compile  'unzoo' as follows
**      cc  -o unzoo  -DSYS_IS_UNIX   -O  unzoo.c
**  If your UNIX has the 'mkdir' system call,  you may add  '-DSYS_HAS_MKDIR'
**  for a slightly faster executable.   BSD has it,  else try  'man 2 mkdir'.
**
**  Under  DOS  with the  DJGPP  GNU C compiler,  compile  'unzoo' as follows
**      gcc  -o unzoo.out  -DSYS_IS_DOS_DJGPP  -O2  unzoo.c
**      copy /b \djgpp\bin\go32.exe+unzoo.out unzoo.exe
**
**  Under TOS with the GNU compiler and unixmode, compile  'unzoo' as follows
**      gcc  -o unzoo.ttp  -DSYS_IS_TOS_GCC  -O2  unzoo.c
**
**  Under OS/2 2 with the emx development system, compile  'unzoo' as follows
**      gcc  -o unzoo.exe  -DSYS_IS_OS2_EMX  -Zomf -Zsys  -O2  unzoo.c
**  To create an executable that runs under OS/2 and DOS,  but which requires
**  the emx runtime, compile without the '-Zomf' and '-Zsys' options.
**
**  On a VAX running VMS with the DEC C compiler, compile  'unzoo' as follows
**      cc   unzoo/define=SYS_IS_VMS
**      link unzoo
**  Then perform the following global symbolic assignment
**      unzoo :== $<dev>:[<dir>]unzoo.exe
**  where  <dir> is the    name of the   directory  where you  have installed
**  'unzoo' and  <dev> is the device on which this directory is,  for example
**      unzoo :== $dia1:[progs.archivers]unzoo
**  You may want to put this symbolic assignment into your  'login.com' file.
**
**  On a  Macintosh  with  the  MPW C  compiler,  compile  'unzoo' as follows
**      C    -model far  -d SYS_IS_MAC_MPW  -opt on  unzoo.c
**      Link -model far -d -c '????' -t APPL unzoo.c.o -o unzoo   <continued>
**          "{CLibraries}"StdClib.o "{Libraries}"SIOW.o           <continued>
**          "{Libraries}"Runtime.o  "{Libraries}"Interface.o
**      Rez  -a "{RIncludes}"SIOW.r  -o unzoo
**  Afterwards choose the  'Get Info' command in the  finder 'File' menu  and
**  increase the  amount of memory  'unzoo' gets upon startup to  256 KBytes.
**  To  create a MPW  tool instead of a  standalone, link with creator 'MPS '
**  instead of '????', with type 'MPST' instead  of 'APPL' and with 'Stubs.o'
**  instead of 'SIOW.o'.  The  'Rez' command  is  not required for the  tool.
**  Alternatively choose the 'Create Build Commands...'  command from the MPW
**  'Build' menu to create a  makefile.  Edit it  and add '-d SYS_IS_MAC_MPW'
**  to the  compile command.  Choose the  'Build...' command from the 'Build'
**  menu to build 'unzoo'.
**
**  On  other systems with a C compiler,  try to  compile  'unzoo' as follows
**      cc  -o unzoo -DSYS_IS_GENERIC  -O  unzoo.c
**
**  PORTING
**
**  If this  does not work,  you must supply new   definitions for the macros
**  'OPEN_READ_ARCH',   'OPEN_READ_TEXT' and  'OPEN_WRIT_TEXT'.  If you  want
**  'unzoo' to keep long file  names, you must   supply a definition for  the
**  macro 'CONV_NAME'.  If  you want 'unzoo'  to extract into subdirectories,
**  you   must supply a  definition for  the macro 'CONV_DIRE'.   If you want
**  'unzoo' to automatically create directories, you must supply a definition
**  for the macro 'MAKE_DIR'.  If you want  'unzoo' to set the permissions of
**  extracted  members to those  recorded in the archive,  you must  supply a
**  definition for the macro 'SETF_PERM'.  Finally if you want 'unzoo' to set
**  the times of the extracted members to  the times recorded in the archive,
**  you must supply a definition for the  macro 'SETF_TIME'.  Everything else
**  should be system independent.
**
**  ACKNOWLEDGMENTS
**
**  Rahul Dhesi  wrote the  'zoo' archiver and the  'booz' archive extractor.
**  Haruhiko Okumura  wrote the  LZH code (originally for his 'ar' archiver).
**  David Schwaderer provided the CRC-16 calculation in PC Tech Journal 4/85.
**  Jeff Damens  wrote the name match code in 'booz' (originally for Kermit).
**  Harald Boegeholz  ported 'unzoo' to OS/2 with the emx development system.
**  Dave Bayer ported 'unzoo' to the Macintosh,  including Macbinary support.
**
**  HISTORY
*H  $Log: unzoo.c,v $
*H  Revision 4.4  2000/05/29 08:56:57  sal
*H  Remove all the \ continuation lines -- who needs the hassle.	SL
*H
*H  Revision 4.3  1999/10/27 08:51:11  sal
*H  Fix date problem on alphas (I hope)	SL
*H
*H  Revision 4.2  1999/05/26 09:27:03  gap
*H  burkhard: use fseek to access file comments; Mac version: several minor fixes
*H
*H  Revision 1.5  1994/01/21  13:32:32  mschoene
*H  added Mac support from Dave Bayer
*H
*H  Revision 1.4  1994/01/20  20:45:46  mschoene
*H  cleaned up determination of write mode
*H
*H  Revision 1.3  1993/12/02  12:43:12  mschoene
*H  added OS/2 support from Harald Boegeholz
*H
*H  Revision 1.2  1993/12/02  12:33:39  mschoene
*H  fixed several typos, renamed MS-DOS to DOS
*H
*H  Revision 1.1  1993/11/09  07:17:50  mschoene
*H  Initial revision
*H
*/





unsigned long   ByteReadArch ()
{
    return ((PtrArch < EndArch) ? *PtrArch++ : FillReadArch());
}


unsigned long   HalfReadArch ()
{
    unsigned long       result;
    result  = ((unsigned long)ByteReadArch());
    result += ((unsigned long)ByteReadArch()) << 8;
    return result;
}

unsigned long   FlahReadArch ()
{
    unsigned long       result;
    result  = ((unsigned long)ByteReadArch()) << 8;
    result += ((unsigned long)ByteReadArch());
    return result;
}

unsigned long   TripReadArch ()
{
    unsigned long       result;
    result  = ((unsigned long)ByteReadArch());
    result += ((unsigned long)ByteReadArch()) << 8;
    result += ((unsigned long)ByteReadArch()) << 16;
    return result;
}

// for fucks sake.. it's 4 bytes, not 2
// -> why not say so ? it's supposed to be Dword then..
unsigned long   WordReadArch ()
{
    unsigned long       result;
    result  = ((unsigned long)ByteReadArch());
    result += ((unsigned long)ByteReadArch()) << 8;
    result += ((unsigned long)ByteReadArch()) << 16;
    result += ((unsigned long)ByteReadArch()) << 24;
    return result;
}



/****************************************************************************
**
*F  DecodeCopy(<size>). . . . . . . . . . . .  extract an uncompressed member
**
**  'DecodeCopy' simply  copies <size> bytes  from the  archive to the output
**  file.
*/
//
// note: caller should seek data position already
//
bool CUnZoo::DecodeCopy(ZooEntry *pEntry, CAnsiFile &archive, CAnsiFile &outFile)
{
    /* initialize the crc value                                            */
    m_crc.ClearCrc(); // = 0;
    m_ReadBuffer.PrepareBuffer(8192, false); // clear, allocate if not enough
    
    size_t size = pEntry->compressed_size;

    /* loop until everything has been copied                               */
    while ( 0 < size ) 
    {
        /* read as many bytes as possible in one go                        */
        const size_t bufSize = m_ReadBuffer.GetSize();
        unsigned long blockSize = (bufSize < size ? bufSize : size); // choose largest possible
        
	    if (archive.Read(m_ReadBuffer.GetBegin(), blockSize) == false)
        {
            throw IOException("unexpected <eof> in the archive");
        }
        if (outFile.Write(m_ReadBuffer.GetBegin(), blockSize) == false)
        {
            throw IOException("cannot write output file");
        }

        /* compute the crc                                                 */
        m_crc.UpdateCrc(BufFile, blockSize);

        /* on to the next block                                            */
        size -= blockSize;
    }

	// actually check the computed CRC-value also..
    if (m_crc.m_Crc != pEntry->data_crc)
    {
		throw ArcException("CRC error on DecodeCopy()", pEntry->fileName);
    }
    return true;
}


/****************************************************************************
**
*F  DecodeLzd() . . . . . . . . . . . . . . .  extract a LZ compressed member
**
*N  1993/10/21 martin add LZD.
*/
/**/
bool CUnZoo::DecodeLzd(ZooEntry *pEntry, CAnsiFile &archive, CAnsiFile &outFile)
{
	// just ignore this for now
	//throw IOException("LZD not implemented");
    return false;
}
/**/

/****************************************************************************
**
*F  DecodeLzh() . . . . . . . . . . . . . . . extract a LZH compressed member
**
**  'DecodeLzh'  decodes  a LZH  (Lempel-Ziv 77  with dynamic Huffman coding)
**  encoded member from the archive to the output file.
**
**  Each member is encoded as a  series of blocks.  Each  block starts with a
**  16  bit field that contains the  number of codes  in this block <number>.
**  The member is terminated by a block with 0 codes.
**
**  Next each block contains the  description of three Huffman codes,  called
**  pre code, literal/length code, and log code.  The purpose of the pre code
**  is to encode the description of  the literal/length code.  The purpose of
**  the literal/length code and the  log code is   to encode the  appropriate
**  fields in the LZ code.   I am too stupid to  understand the format of the
**  description.
**
**  Then   each block contains  <number>  codewords.  There  are two kinds of
**  codewords, *literals* and *copy instructions*.
**
**  A literal represents a certain byte.  For  the moment imaging the literal
**  as having 9 bits.   The first bit  is zero, the other  8 bits contain the
**  byte.
**
**      +--+----------------+
**      | 0|     <byte>     |
**      +--+----------------+
**
**  When a  literal is  encountered, the byte  <byte> that  it represents  is
**  appended to the output.
**
**  A copy  instruction represents a certain  sequence of bytes that appeared
**  already  earlier in the output.  The  copy instruction  consists of three
**  parts, the length, the offset logarithm, and the offset mantissa.
**
**      +--+----------------+--------+--------------------+
**      | 1|   <length>-3   |  <log> |     <mantissa>     |
**      +--+----------------+--------+--------------------+
**
**  <length>  is  the  length  of the sequence   which  this copy instruction
**  represents.  We store '<length>-3', because <length> is never 0, 1, or 2;
**  such sequences are better represented by 0, 1, or  2 literals.  <log> and
**  <mantissa>  together represent the offset at  which the sequence of bytes
**  already  appeared.  '<log>-1'  is  the number of   bits in the <mantissa>
**  field, and the offset is $2^{<log>-1} + <mantissa>$.  For example
**
**      +--+----------------+--------+----------+
**      | 1|        9       |    6   | 0 1 1 0 1|
**      +--+----------------+--------+----------+
**
**  represents the sequence of 12 bytes that appeared $2^5 + 8 + 4  + 1 = 45$
**  bytes earlier in the output (so those 18 bits of input represent 12 bytes
**  of output).
**
**  When a copy instruction  is encountered, the  sequence of  <length> bytes
**  that appeared   <offset> bytes earlier  in the  output  is again appended
**  (copied) to   the output.   For this  purpose  the last  <max>  bytes are
**  remembered,  where  <max>  is the   maximal  used offset.   In 'zoo' this
**  maximal offset is $2^{13} =  8192$.  The buffer in  which those bytes are
**  remembered is  called   a sliding  window for   reasons  that  should  be
**  obvious.
**
**  To save even  more space the first 9  bits of each code, which  represent
**  the type of code and either the literal value or  the length, are encoded
**  using  a Huffman code  called the literal/length  code.   Also the next 4
**  bits in  copy instructions, which represent  the logarithm of the offset,
**  are encoded using a second Huffman code called the log code.
**
**  Those  codes  are fixed, i.e.,  not  adaptive, but  may  vary between the
**  blocks, i.e., in each block  literals/lengths and logs  may be encoded by
**  different codes.  The codes are described at the beginning of each block.
**
**  Haruhiko Okumura  wrote the  LZH code (originally for his 'ar' archiver).
*/

int MakeTablLzh ( const int nchar, const unsigned char *bitlen, const int tablebits, uint16_t *table )
{
    uint16_t      count[17], weight[17], start[18];
    unsigned int        i, k;

    for (i = 1; i <= 16; i++) // why first remains random value..?
    {
		count[i] = 0;
	}
    for (i = 0; i < nchar; i++) 
    {
		count[bitlen[i]]++;
	}

    start[1] = 0;
    for (i = 1; i <= 16; i++)
    {
        start[i + 1] = start[i] + (count[i] << (16 - i));
    }
    
    if (start[17] != (uint16_t)((unsigned) 1 << 16))
    {
		// corrupted file,
		// wrong offset used
		// or bug in code
		throw IOException("make table failed");
    }

    unsigned int jutbits = 16 - tablebits;
    for (i = 1; i <= tablebits; i++) 
    {
        start[i] >>= jutbits;
        weight[i] = (unsigned) 1 << (tablebits - i);
    }
    while (i <= 16) 
    {
        weight[i] = (unsigned) 1 << (16 - i);
        i++;
    }

    i = start[tablebits + 1] >> jutbits;
    if (i != (uint16_t)((unsigned) 1 << 16)) 
    {
        k = 1 << tablebits;
        while (i != k) 
        {
	        table[i++] = 0;
	    }
    }

    unsigned int avail = nchar;
    unsigned int mask = (unsigned) 1 << (15 - tablebits);
    for (unsigned int ch = 0; ch < nchar; ch++) 
    {
		uint32_t len = bitlen[ch];
        if (len == 0) 
        {
			continue;
		}
		
        if (len <= tablebits) 
        {
            for (uint32_t i = 0; i < weight[len]; i++ )  
            {
				table[i+start[len]] = ch;
			}
        }
        else 
        {
            uint32_t k = start[len];
            uint16_t *p = &table[k >> jutbits];
            uint32_t i = len - tablebits;
            while (i != 0) 
            {
                if (*p == 0) 
                {
                    TreeRight[avail] = TreeLeft[avail] = 0;
                    *p = avail++;
                }
                if (k & mask) 
                {
					p = &TreeRight[*p];
				}
                else
                {
					p = &TreeLeft[*p];
				}
                k <<= 1;  
                i--;
            }
            *p = ch;
        }
        start[len] += weight[len];
    }

    /* indicate success                                                    */
    return 1;
}

#define MAX_LIT                 255     /* maximal literal code            */
#define MIN_LEN                 3       /* minimal length of match         */
#define MAX_LEN                 256     /* maximal length of match         */
#define MAX_CODE                (MAX_LIT+1 + MAX_LEN+1 - MIN_LEN)
#define BITS_CODE               9       /* 2^BITS_CODE > MAX_CODE (+1?)    */
#define MAX_OFF                 8192    /* 13 bit sliding directory        */
#define MAX_LOG                 13      /* maximal log_2 of offset         */
#define BITS_LOG                4       /* 2^BITS_LOG > MAX_LOG (+1?)      */
#define MAX_PRE                 18      /* maximal pre code                */
#define BITS_PRE                5       /* 2^BITS_PRE > MAX_PRE (+1?)      */

uint16_t  TreeLeft [2*MAX_CODE+1];/* tree for codes   (upper half)   */
uint16_t  TreeRight[2*MAX_CODE+1];/* and  for offsets (lower half)   */
uint16_t TabCode  [4096];        /* table for fast lookup of codes  */
uint8_t   LenCode  [MAX_CODE+1];  /* number of bits used for code    */
uint16_t  TabLog   [256];         /* table for fast lookup of logs   */
uint8_t   LenLog   [MAX_LOG+1];   /* number of bits used for logs    */
uint16_t  TabPre   [256];         /* table for fast lookup of pres   */
uint8_t   LenPre   [MAX_PRE+1];   /* number of bits used for pres    */

// same stuff as in actual LhA/Lzh library,
// are there differences to worry about?
//
bool CUnZoo::DecodeLzh(ZooEntry *pEntry, CAnsiFile &archive, CAnsiFile &outFile)
{
	size_t size = pEntry->compressed_size;
	
    unsigned long       cnt;            /* number of codes in block        */
    //unsigned long       cnt2;           /* number of stuff in pre code     */
    //unsigned long       code;           /* code from the Archive           */
    //unsigned long       len;            /* length of match                 */
    //unsigned long       log;            /* log_2 of offset of match        */
    //unsigned long       off;            /* offset of match                 */
    //unsigned long       pre;            /* pre code                        */
    //char *              cur;            /* current position in BufFile     */
    char *              pos;            /* position of match               */
    //char *              end;            /* pointer to the end of BufFile   */
    char *              stp;            /* stop pointer during copy        */
    //unsigned long       crc;            /* cyclic redundancy check value   */
    //unsigned long       i;              /* loop variable                   */
    //unsigned long       bits;           /* the bits we are looking at      */
    //unsigned long       bitc;           /* number of bits that are valid   */

    /* initialize bit source, output pointer, and crc                      */
    unsigned long bits = 0;  /* the bits we are looking at      */
    unsigned long bitc = 0;  /* number of bits that are valid   */
    FLSH_BITS(0, bits, bitc);
    char *cur = BufFile;  
    char *end = BufFile + MAX_OFF;
	m_crc.ClearCrc();

    /* loop until all blocks have been read                                */
    cnt = PEEK_BITS( 16, bits, bitc );  
    FLSH_BITS( 16, bits, bitc );
    while ( cnt != 0 ) 
    {
        /* read the pre code                                               */
        uint32_t pc_count = PEEK_BITS( BITS_PRE, bits, bitc );  
        FLSH_BITS( BITS_PRE, bits, bitc );
        if ( pc_count == 0 ) 
        {
            uint32_t pre = PEEK_BITS( BITS_PRE, bits, bitc );  
            FLSH_BITS( BITS_PRE, bits, bitc );
            
            bufferSet(pre, TabPre, 256);
			
			// just memset() it to zero..
			::memset(LenPre, 0, MAX_PRE +1);
        }
        else 
        {
            uint32_t i = 0;
            while ( i < pc_count ) 
            {
                uint32_t len = PEEK_BITS( 3, bits, bitc );  
                FLSH_BITS( 3, bits, bitc );
                if ( len == 7 ) {
                    while ( PEEK_BITS( 1, bits, bitc ) ) 
                    { 
						len++; 
						FLSH_BITS( 1, bits, bitc ); 
					}
                    FLSH_BITS( 1, bits, bitc );
                }
                LenPre[i++] = len;
                if ( i == 3 ) 
                {
                    len = PEEK_BITS( 2, bits, bitc );  
                    FLSH_BITS( 2, bits, bitc );
                    while ( 0 < len-- )  
                    {
						LenPre[i++] = 0;
					}
                }
            }
            while ( i <= MAX_PRE )  
            {
				LenPre[i++] = 0;
			}
            if ( ! MakeTablLzh( MAX_PRE+1, LenPre, 8, TabPre ) ) 
            {
                throw IOException("pre code description corrupted");
            }
        }

        /* read the code (using the pre code)                              */
        pc_count = PEEK_BITS( BITS_CODE, bits, bitc );  
        FLSH_BITS( BITS_CODE, bits, bitc );
        if ( pc_count == 0 ) 
        {
            uint32_t code = PEEK_BITS( BITS_CODE, bits, bitc );  
            FLSH_BITS( BITS_CODE, bits, bitc );
            
            bufferSet(code, TabCode, 4096);
            ::memset(LenCode, 0, MAX_CODE +1);
        }
        else 
        {
            uint32_t i = 0;
            while ( i < pc_count ) 
            {
                uint32_t len = TabPre[ PEEK_BITS( 8, bits, bitc ) ];
                if ( len <= MAX_PRE ) 
                {
                    FLSH_BITS( LenPre[len], bits, bitc );
                }
                else 
                {
                    FLSH_BITS( 8, bits, bitc );
                    do 
                    {
                        if ( PEEK_BITS( 1, bits, bitc ) ) 
                        {
							len = TreeRight[len];
						}
                        else
                        {
							len = TreeLeft [len];
						}
                        FLSH_BITS( 1, bits, bitc );
                    } while ( MAX_PRE < len );
                }
                if ( len <= 2 ) 
                {
                    if ( len == 0 ) 
                    {
                        len = 1;
                    }
                    else if ( len == 1 ) 
                    {
                        len = PEEK_BITS(4, bits, bitc)+3;  
                        FLSH_BITS(4, bits, bitc);
                    }
                    else 
                    {
                        len = PEEK_BITS(BITS_CODE, bits, bitc)+20; 
                        FLSH_BITS(BITS_CODE, bits, bitc);
                    }
                    
                    while ( 0 < len-- )  
                    {
						LenCode[i++] = 0;
					}
                }
                else 
                {
                    LenCode[i++] = len - 2;
                }
            }
            while ( i <= MAX_CODE )  
            {
				LenCode[i++] = 0;
			}
            if ( ! MakeTablLzh( MAX_CODE+1, LenCode, 12, TabCode ) ) 
            {
                throw IOException("literal/length code description corrupted");
            }
        }

        /* read the log_2 of offsets                                       */
        pc_count = PEEK_BITS( BITS_LOG, bits, bitc );  
        FLSH_BITS( BITS_LOG, bits, bitc );
        if ( pc_count == 0 ) 
        {
            uint32_t log = PEEK_BITS( BITS_LOG, bits, bitc );  
            FLSH_BITS( BITS_LOG, bits, bitc );
            
            bufferSet(log, TabLog, 256);
            ::memset(LenLog, 0, MAX_LOG +1);
        }
        else 
        {
            uint32_t i = 0;
            while ( i < pc_count ) 
            {
                uint32_t len = PEEK_BITS( 3, bits, bitc );  
                FLSH_BITS( 3, bits, bitc );
                if ( len == 7 ) 
                {
                    while ( PEEK_BITS( 1, bits, bitc ) ) 
                    { 
						len++; 
						FLSH_BITS( 1, bits, bitc ); 
					}
                    FLSH_BITS( 1, bits, bitc );
                }
                LenLog[i++] = len;
            }
            while ( i <= MAX_LOG )  
            {
				LenLog[i++] = 0;
			}
            if ( ! MakeTablLzh( MAX_LOG+1, LenLog, 8, TabLog ) ) 
            {
                throw IOException("log code description corrupted");
            }
        }

        /* read the codes                                                  */
        while ( 0 < cnt-- ) 
        {
            /* try to decode the code the fast way                         */
            uint32_t code = TabCode[ PEEK_BITS( 12, bits, bitc ) ];

            /* if this code needs more than 12 bits look it up in the tree */
            if ( code <= MAX_CODE ) 
            {
                FLSH_BITS( LenCode[code], bits, bitc );
            }
            else 
            {
                FLSH_BITS( 12, bits, bitc );
                do {
                    if ( PEEK_BITS( 1, bits, bitc ) )  
                    {
						code = TreeRight[code];
					}
                    else
                    {
						code = TreeLeft [code];
					}
                    FLSH_BITS( 1, bits, bitc );
                } while ( MAX_CODE < code );
            }

            if ( code <= MAX_LIT ) 
            {
	            /* if the code is a literal, stuff it into the buffer          */
	            
                *cur++ = code;
                m_crc.m_Crc = m_crc.CRC_BYTE(m_crc.m_Crc, code );
                if ( cur == end ) 
                {
				    if (outFile.Write(BufFile, cur-BufFile) == false)
                    {
		                throw IOException("cannot write output file");
                    }
                    cur = BufFile;
                }
            }
            else 
            {
	            /* otherwise compute match length and offset and copy          */
                uint32_t len = code - (MAX_LIT+1) + MIN_LEN;

                /* try to decodes the log_2 of the offset the fast way     */
                uint32_t log = TabLog[ PEEK_BITS( 8, bits, bitc ) ];
                /* if this log_2 needs more than 8 bits look in the tree   */
                if ( log <= MAX_LOG ) 
                {
                    FLSH_BITS( LenLog[log], bits, bitc );
                }
                else 
                {
                    FLSH_BITS( 8, bits, bitc );
                    do {
                        if ( PEEK_BITS( 1, bits, bitc ) )  
                        {
							log = TreeRight[log];
						}
                        else
                        {
							log = TreeLeft [log];
						}
                        FLSH_BITS( 1, bits, bitc );
                    } while ( MAX_LOG < log );
                }

                /* compute the offset                                      */
                uint32_t off = 0;
                if ( log != 0 ) 
                {
                    off = ((unsigned)1 << (log-1)) + PEEK_BITS( log-1, bits, bitc );
                    FLSH_BITS( log-1, bits, bitc );
                }

                /* copy the match (this accounts for ~ 50% of the time)    */
                pos = BufFile + (((cur-BufFile) - off - 1) & (MAX_OFF - 1));
                if ( cur < end-len && pos < end-len ) 
                {
                    stp = cur + len;
                    do 
                    {
                        code = *pos++;
		                m_crc.m_Crc = m_crc.CRC_BYTE(m_crc.m_Crc, code );
                        *cur++ = code;
                    } while ( cur < stp );
                }
                else 
                {
                    while ( 0 < len-- ) 
                    {
                        code = *pos++;
		                m_crc.m_Crc = m_crc.CRC_BYTE(m_crc.m_Crc, code );
                        *cur++ = code;
                        if ( pos == end ) 
                        {
                            pos = BufFile;
                        }
                        if ( cur == end ) 
                        {
						    if (outFile.Write(BufFile, cur-BufFile) == false)
                            {
				                throw IOException("cannot write output file");
                            }
                            cur = BufFile;
                        }
                    }
                }
            }
        }

        cnt = PEEK_BITS( 16, bits, bitc );  
        FLSH_BITS( 16, bits, bitc );
    }

    /* write out the rest of the buffer                                    */
    if (outFile.Write(BufFile, cur-BufFile) == false)
    {
        throw IOException("cannot write output file");
    }
    return true;
}

void CUnZoo::readString(CAnsiFile &archive, const size_t offset, const size_t length, std::string &value)
{
	if (archive.Seek(offset, SEEK_SET) == false)
	{
		throw IOException("Failed to seek string entry");
	}
	
	if (archive.Read(m_ReadBuffer.GetBegin(), length) == false)
	{
		throw IOException("Failed to read string");
	}
	value.assign((char*)m_ReadBuffer.GetBegin(), length);
}

// read list of archive contents (entry-list)
//
bool CUnZoo::readArchiveEntryList(CAnsiFile &archive)
{
	// TODO: check, can we just assume we are at correct offset now
	// or do we really need these below..
	
	long lPos = 0;
	if (archive.Tell(lPos) == false)
	{
		throw IOException("Failed to determine position");
	}
	// should be at end of archive header now
	if (lPos != m_archiveInfo.header_size)
	{
		// if not, seek first entry
		if (archive.Seek(m_archiveInfo.first_entry_pos, SEEK_SET) == false)
		{
			throw IOException("Failed to seek first entry");
		}
	}
	
	// size of fixed-length part for entry-information
	const size_t entryFixedSize = 34 + 13; // values + short name len

	size_t nReadCount = m_archiveInfo.header_size;
	while ((nReadCount+entryFixedSize) < m_nFileSize)
	{
		if (archive.Read(m_ReadBuffer.GetBegin(), entryFixedSize) == false)
		{
			// corrupted file or bug? should detect end before..
			throw IOException("Failed to read entry header");
		}
		nReadCount += entryFixedSize;
		
		if (isSupportedEntry(m_ReadBuffer.GetBegin()) == false)
		{
			throw IOException("Unsupported entry detected");
		}
		m_ReadBuffer.SetCurrentPos(0); // simplify rest, keep counting offset

		// should be supported -> allocate entry to list, get rest of values	
		ZooEntry *pEntry = new ZooEntry();
		m_EntryList.push_back(pEntry);
		
	    pEntry->magicid = getULong(m_ReadBuffer.GetNext(4)); // keep for debugging at least..
	    pEntry->member_type = m_ReadBuffer.GetNextByte();
	    pEntry->method = m_ReadBuffer.GetNextByte();
	    pEntry->next_entry_pos = getULong(m_ReadBuffer.GetNext(4));
	    pEntry->data_position = getULong(m_ReadBuffer.GetNext(4));

		// convert this timestamp to usable form
	    uint16_t date = getUWord(m_ReadBuffer.GetNext(2));
	    uint16_t time = getUWord(m_ReadBuffer.GetNext(2));
	    pEntry->timestamp = (time_t)CGenericTime(date, time);
	    
	    pEntry->data_crc = getUWord(m_ReadBuffer.GetNext(2));
	    pEntry->original_size = getULong(m_ReadBuffer.GetNext(4));
	    pEntry->compressed_size = getULong(m_ReadBuffer.GetNext(4));
	    pEntry->version_major = m_ReadBuffer.GetNextByte();
	    pEntry->version_minor = m_ReadBuffer.GetNextByte();
	    pEntry->deleted = m_ReadBuffer.GetNextByte();
	    pEntry->spared = m_ReadBuffer.GetNextByte(); // padding-byte only?
	    pEntry->comment_position = getULong(m_ReadBuffer.GetNext(4));
	    pEntry->comment_size = getUWord(m_ReadBuffer.GetNext(2));
	    
		// some short name (fucking msdos..):
		// overwrite this with proper name when it is found in file
		//
		pEntry->fileName.assign((char*)m_ReadBuffer.GetNext(13), 13);
		fixPathname(pEntry->fileName); // fix path-separator

		// handle extension of entry-information..
		// part of it has fixed size and 
		// 
		if (pEntry->member_type == 2) // (what are these types anyway?)
		{
			pEntry->m_pVarDetails = new ZooVariableEntry();
			
			size_t extendedRead = 2+1+2; // next fields needed..
			if (archive.Read(m_ReadBuffer.GetBegin(), extendedRead) == false)
			{
				throw IOException("Failed to read extended header start");
			}
			m_ReadBuffer.SetCurrentPos(0); // simplify rest, keep counting offset
			nReadCount += extendedRead;
			
			// does this include size of length-field or just the rest?
			// just the rest after these three fields?
			pEntry->m_pVarDetails->variable_size = getUWord(m_ReadBuffer->GetNext(2));
			pEntry->m_pVarDetails->timezone = m_ReadBuffer.GetNextByte();
			pEntry->m_pVarDetails->entry_crc = getUWord(m_ReadBuffer->GetNext(2));
			
			extendedRead = pEntry->m_pVarDetails->variable_size;
			if (extendedRead > 0)
			{
				if (archive.Read(m_ReadBuffer.GetBegin(), extendedRead) == false)
				{
					throw IOException("Failed to read extended header variable-sized part");
				}
				m_ReadBuffer.SetCurrentPos(0); // simplify rest, keep counting offset
				nReadCount += extendedRead;
	
				// no point in this if there is no name also.. check anyway (broken archives?)
				uint8_t fileNameLen = m_ReadBuffer.GetNextByte();
				extendedRead -= 1;
				
				uint8_t dirNameLen = 0;
				if (extendedRead > 0)
				{
					dirNameLen = m_ReadBuffer.GetNextByte();
					extendedRead -= 1;
				}
				
				if (fileNameLen > 0 && extendedRead > 0)
				{
					// overwrite msdos-shit from before when we have proper name for file
					pEntry->fileName.assign((char*)m_ReadBuffer.GetNext(fileNameLen), fileNameLen);
					fixPathname(pEntry->fileName); // fix path-separator for later
					extendedRead -= fileNameLen;
				}
				if (dirNameLen > 0 && extendedRead > 0)
				{
					pEntry->pathName.assign((char*)m_ReadBuffer.GetNext(dirNameLen), dirNameLen);
					fixPathname(pEntry->pathName); // fix path-separator for later
					
					// append path-separator to directory name if missing
					if (pEntry->pathName.at(pEntry->pathName.length() -1) != '/')
					{
						pEntry->pathName += "/";
					}
					
					extendedRead -= dirNameLen;
				}
				if (extendedRead > 0)
				{
					pEntry->m_pVarDetails->systemid = getUWord(m_ReadBuffer.GetNext(2));
					extendedRead -= 2;
				}
				if (extendedRead > 0)
				{
					// some weird three-byte value..
					pEntry->m_pVarDetails->permissions = getTriple(m_ReadBuffer.GetNext(3));
					extendedRead -= 3;
				}
				if (extendedRead > 0)
				{
					// some weird three-byte value..
					pEntry->m_pVarDetails->modgen = m_ReadBuffer.GetNextByte();
					extendedRead -= 1;
				}
				if (extendedRead > 0)
				{
					// some weird three-byte value..
					pEntry->m_pVarDetails->version = getUWord(m_ReadBuffer.GetNext(2));
					extendedRead -= 2;
				}
			}
		}
	}
	
	// read comments..
	// no idea where they should be, hope offsets are ok..
	// does offset break if they are read in actual entry information reading?
	//
	auto it = m_EntryList.begin();
	auto itEnd = m_EntryList.end();
	while (it != itEnd)
	{
		ZooEntry *pEntry = (*it);
		
		if (pEntry->comment_position > 0 && pEntry->comment_size > 0)
		{
			readString(archive,
						pEntry->comment_position,
						pEntry->comment_size,
						pEntry->comment);
		}
		
		++it;
	}

	return true;
}


// read archive metadata (description) from file
//
bool CUnZoo::readArchiveDescription(CAnsiFile &archive)
{
    if (archive.Read(m_ReadBuffer.GetBegin(), 34) == false)
    {
		// file less than header size or just not readable,
		// nothing to do here anyway -> just get out of here
		throw IOException("Failed to read header");
    }
    if (m_archiveInfo.isSupported(m_ReadBuffer.GetBegin()) == false)
    {
		throw IOException("Unsupported file type");
    }
    
    m_archiveInfo.description.assign((char*)m_ReadBuffer.GetBegin(), 20);
    m_ReadBuffer.SetCurrentPos(20); // simplify rest.. keep counting offset
    m_archiveInfo.magicid = getULong(m_ReadBuffer.GetNext(4));
    m_archiveInfo.first_entry_pos = getULong(m_ReadBuffer.GetNext(4));
    m_archiveInfo.klhvmh = getULong(m_ReadBuffer.GetNext(4));
    
    m_archiveInfo.version_major = m_ReadBuffer.GetNextByte();
    m_archiveInfo.version_minor = m_ReadBuffer.GetNextByte();

	// set size so far..
	m_archiveInfo.header_size = 34;

	// extension of header in newer formats
	// if first entry is not directly after "old" header
	if (m_archiveInfo.first_entry_pos > 34)
	{
		// 8 bytes as optional information
		if (archive.Read(m_ReadBuffer.GetCurrentPos(), 8) == false)
		{
			throw IOException("Failed to read header extension");
		}
		m_archiveInfo.member_type = m_ReadBuffer.GetNextByte();
		m_archiveInfo.comment_pos = getULong(m_ReadBuffer.GetNext(4));
		m_archiveInfo.comment_size = getUWord(m_ReadBuffer.GetNext(2));
		m_archiveInfo.modgen = m_ReadBuffer.GetNextByte();
		m_archiveInfo.is_new_style = true;
		m_archiveInfo.header_size += 8; // update (extension included)
		
		if (m_archiveInfo.comment_size > 0 && m_archiveInfo.comment_pos > 0)
		{
			readString(archive, 
						m_archiveInfo.comment_pos, 
						m_archiveInfo.comment_size, 
						m_archiveInfo.comment);
		}
	}
    return true;
}

// read list of archive contents into entry-list
//
bool CUnZoo::ListArchive(const CAnsiFile &archive)
{

    /* if present, print the archive comment                               */
    if ( ver && Descript.sizcmt != 0 ) 
    {
        if ( ! GotoReadArch( Descript.poscmt ) ) 
        {
			throw ArcException("unzoo: cannot find comment in archive", archiveName);
        }
        
		// agh.. just read it and get it done
		m_ReadBuffer.PrepareBuffer(Entry.sizcmt, false);
		archive.Read(m_ReadBuffer.GetBegin(), Entry.sizcmt);
        
        // ? why not read already?
        /*
        for ( i = 0; i < Descript.sizcmt; i++ ) {
            chr = ByteReadArch();
        }
        */
    }

    /* print the header                                                    */
    //printf("Length    CF  Size Now  Date      Time    \n");

    /* loop over the members of the archive                                */
    Entry.posnxt = Descript.posent;
    while ( 1 ) {

        /* read the directory entry for the next member                    */
        if ( ! GotoReadArch( Entry.posnxt ) || ! EntrReadArch() ) 
        {
			throw ArcException("unzoo: bad directory entry in archive", Entry.fileName);
        }
        if ( ! Entry.posnxt )  
        {
			break;
		}

        /* skip members we don't care about                                */
        if ( Entry.deleted == 1 )
        {
			// what is this really?
            continue;
        }
        if ( filec == 0 && userSelection.contains(Entry.fileName) == false)
        {
			// if user-given selection -> check list,
			// don't try regex..
            continue;
        }

        /* print the information about the member                          */
        /*
        printf("%8lu %3lu%% %8lu  %2lu %3s %02lu %02lu:%02lu:%02lu   %s\n",
               Entry.sizorg,
               (100*(Entry.sizorg-Entry.siznow)+Entry.sizorg/2)
               / (Entry.sizorg != 0 ? Entry.sizorg : 1),
               Entry.siznow,
               Entry.day, NameMonth[Entry.month], Entry.year % 100,
               Entry.hour, Entry.min, Entry.sec,
               (ver ? Entry.patv : Entry.patw) );
               */

        /* update the counts for the whole archive                         */
        Descript.sizorg += Entry.sizorg;
        Descript.siznow += Entry.siznow;
        Descript.number += 1;

        /* if present print the file comment                               */
        if ( ver && Entry.sizcmt != 0 ) 
        {
            if ( ! GotoReadArch( Entry.poscmt ) ) 
            {
				throw ArcException("unzoo: cannot find comment in archive", Entry.fileName);
            }
            
            // agh.. just read it and get it done
            m_ReadBuffer.PrepareBuffer(Entry.sizcmt, false);
            archive.Read(m_ReadBuffer.GetBegin(), Entry.sizcmt);
            
            /*
            for (int i = 0; i < Entry.sizcmt; i++ ) 
            {
                chr = ByteReadArch();
            }
            */
        }

    }

    /* print the footer                                                    */
    /*
    printf("%8lu %3lu%% %8lu  %4lu files\n",
           Descript.sizorg,
           (100*(Descript.sizorg-Descript.siznow)+Descript.sizorg/2)
           / (Descript.sizorg != 0 ? Descript.sizorg : 1),
           Descript.siznow,
           Descript.number );
           */

    return true;
}


/****************************************************************************
**
*F  ExtrArch(<bim>,<out>,<ovr>,<pre>,<arc>,<filec>,<files>) . extract members
**
**  'ExtrArch' extracts the members  of the archive with  the name <arc> that
**  match one  of the file name  patterns '<files>[0] .. <files>[<filec>-1]'.
**  If <bim> is 0, members with comments starting with '!TEXT!' are extracted
**  as text files and the other members are extracted as  binary files; if it
**  is 1,  all members are extracted  as text files; if  it is 2, all members
**  are  extracted as binary  files. If <out>  is 0, no members are extracted
**  and only tested  for integrity; if it  is 1, the  members are printed  to
**  stdout, i.e., to the screen.  and if it  is 2, the members are extracted.
**  If <ovr> is 0, members will not overwrite  existing files; otherwise they
**  will.  <pre> is a prefix that is prepended to all path names.
*/
bool CUnZoo::ExtrArch(CAnsiFile &archive)
{

	// Descript.sizcmt has "!TEXT!" or "!MACBINARY!" in some cases,
	// we should treat all files as being just blobs of binary data:
	// just extract and if it is some obscure file let user decide what to do.
	// (if text-encoding codepage needs changing -> any word processor can do that)
	// (if it is different byteorder -> program using will want original order anyway)
	// etc.
	// -> keep it simple and generic to extract files
	
    /* loop over the members of the archive                                */
    Entry.posnxt = Descript.posent;
    while ( 1 ) {

        /* read the directory entry for the next member                    */
        if ( ! GotoReadArch( Entry.posnxt ) || ! EntrReadArch() ) 
        {
	        throw ArcException("unzoo: bad entry in archive", archiveName);
        }
        
        if ( ! Entry.posnxt )  
        {
	        break;
	    }

        /* skip members we don't care about                                */
        if ( Entry.deleted == 1 )
        {
			// what is this really?
            continue;
        }
        if ( filec == 0 && userSelection.contains(Entry.fileName) == false)
        {
			// if user-given selection -> check list,
			// don't try regex..
            continue;
        }

        /* check that we can decode this file                              */
        if ( (2 < Entry.method) || (2 < Entry.majver)
          || (2 == Entry.majver && 1 < Entry.minver) ) 
        {
            //printf("unzoo: unknown method, you need a later version\n");
            continue;
        }
        
        // TODO: fix path separator also if msdos-style..
        std::string outFilename = Entry.dirName;
        if (outFilename.at(outFilename.length() -1) != '/')
        {
			outFilename += "/";
        }
        outFilename += Entry.fileName;
        
		// file creation fails if this fails
		// -> we can detect error then
        CPathHelper::MakePathToFile(outFilename);
        
        CAnsiFile outFile(outFilename, true);
        if (outFile.IsOk() == false)
        {
			throw ArcException("Failed to open file for writing", outFilename);
        }

        /* decode the file                                                 */
        /*
        if ( ! GotoReadArch( Entry.posdat ) ) 
        {
			throw ArcException("cannot find data in archive", outFilename);
        }
        */
        //ZooEntry *pEntry;
		if (archive.Seek(pEntry->data_position, SEEK_SET) == false)
		{
			throw ArcException("Failed to seek entry", pEntry->fileName);
		}
        
        bool bRes = false;
        
        if (pEntry->method == PackCopyOnly)
        {
			bRes = DecodeCopy(pEntry, archive, outFile);
		}
        else if ( Entry.method == PackLzd )  
        {
			// "directory-only" entry ??
			//bRes = DecodeLzd(pEntry);
			bRes = true;
		}
        else if ( Entry.method == PackLzh )  
        {
			bRes = DecodeLzh(pEntry, archive, outFile);
		}

        /* check that everything went ok                                   */
        if      ( bRes == false)  
        {
	        throw ArcException("Failed to extract file", outFilename);
	    }
        else if ( m_crc.m_Crc != Entry.crcdat  )  
        {
	        throw ArcException("CRC error", outFilename);
        }

        /* close the file after extraction                                 */
        outFile.Close();
    }


    /* indicate success                                                    */
    return true;
}


// get list of archive entries to entry-list
bool CUnZoo::ListContents()
{
	// TODO: better way of handling multiple reads of same file?
	Clear();
	
    CAnsiFile archive(m_szArchive);
    if (archive.IsOk() == false)
    {
        throw ArcException("could not open archive", m_szArchive);
    }
    m_nFileSize = archive.GetSize();

	// should throw exception on error anyway..
	if (readArchiveDescription(archive) == false)
	{
        throw ArcException("failed reading header", m_szArchive);
	}
	
	if (readArchiveEntryList(archive) == false)
	{
        throw ArcException("failed reading entry list", m_szArchive);
	}
	
	return ListArchive(m_szArchive);
}

// test integrity, try extracting without output
bool CUnZoo::TestArchive()
{
	//return ExtractNoOutput(m_szArchive);
}

// extract all files to path given before,
// just extract "as-is": users have other tools to convert text-encoding etc.
bool CUnZoo::Extract()
{
	return ExtrArch(m_szArchive);
}

// with user selections
//
//bool CUnZoo::ExtractSelection(QList<QString> &lstSelectedFiles)
//{}
