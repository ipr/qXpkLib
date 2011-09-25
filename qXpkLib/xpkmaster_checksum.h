#ifndef XPKMASTER_CHECKSUM_H
#define XPKMASTER_CHECKSUM_H

/* Routinesheader

	Name:		checksum.c
	Main:		xpkmaster
	Versionstring:	$VER: checksum.c 1.3 (09.01.1998)
	Author:		SDI
	Distribution:	Freeware
	Description:	Simple checksum routines

 1.0   05.10.96 : first real version
 1.1   30.01.97 : added new option to cchecksum, became necessary
 1.2   31.01.97 : removed new option
 1.3   09.01.98 : added XPK_ALLINONE
 
 1.4   25.09.11 : Made to header for calling from C++ code, 
                  dropped some platform-header includes,
                  changed to ISO-standard type definitions instead of STRPTR/UBYTE..
*/

#include <stdint.h>

uint8_t hchecksum(uint8_t *ptr, uint32_t count)
{
  uint8_t sum = 0;

  while(count-- > 0)
  {
	sum ^= *ptr++;
  }

  return sum;
}

uint16_t cchecksum(uint32_t *ptr, uint32_t count)
{
  uint32_t sum = 0;

  while(count-- > 0)
  {
    sum ^= *ptr++;
  }

  return (uint16_t)(sum ^ (sum >> 16));
}

#endif /* XPKMASTER_CHECKSUM_H */
