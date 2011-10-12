/* xpkSQSH.c -- an LZ based cruncher; special algorithms for 8 bit sound data;
 * fast decrunching
 * Copyright (C) 1994 John Hendrikx
 * This file is part of the xpk package.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA.
 */

/* Written by John Hendrikx <?>
 * Converted by Bert Jahn <wepl@kagi.com>
 * XPK library version by Vesa Halttunen <vesuri@jormas.com>
 * C++ library version by Ilkka Prusi <ilkka.prusi@gmail.com>
 */


#ifndef XPKSQSH_H
#define XPKSQSH_H

#include "xpkSQSH_global.h"

#include <QObject>

// base for library interface
#include "xpkLibraryBase.h"


class XPKSQSHSHARED_EXPORT xpkSQSH : public xpkLibraryBase
{
public:
    xpkSQSH();
    virtual ~xpkSQSH();

	virtual bool Crunch(XpkProgress *pProgress);
	
	virtual bool Decrunch(XpkProgress *pProgress);

};

#ifdef __cplusplus
extern "C"
{
#endif

// need this for caller to locate instance dynamically,
// library needs to have one global instance of the cruncher (has no members anyway..)
//
XPKSQSHSHARED_EXPORT xpkLibraryBase *GetXpkInstance(void);

#ifdef __cplusplus
}
#endif

#endif // XPKSQSH_H
