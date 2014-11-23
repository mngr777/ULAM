/**                                        -*- mode:C++ -*-
 * Constants.h Useful common constants for ULAM
 *
 * Copyright (C) 2014 The Regents of the University of New Mexico.
 * Copyright (C) 2014 Ackleyshack LLC.
 *
 * This file is part of the ULAM programming language compilation system.
 *
 * The ULAM programming language compilation system is free software:
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * The ULAM programming language compilation system is distributed in
 * the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the ULAM programming language compilation system
 * software.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
 */

/**
  \file Constants.h Useful common constants for ULAM
  \author Elenas S. Ackley.
  \author David H. Ackley.
  \date (C) 2014 All rights reserved.
  \gpl
*/

#include "itype.h"

#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace MFM {

  enum STORAGE { IMMEDIATE = 0, EVENTWINDOW, STACK, EVALRETURN, UNPACKEDSTRUCT, TMPREGISTER, TMPBITVAL};
  enum PACKFIT { UNPACKED = 0, PACKED, PACKEDLOADABLE};

#define WritePacked(p) (p == PACKED || p == PACKEDLOADABLE)

#ifndef BITSPERATOM
#define BITSPERATOM (96)
#endif //BITSPERATOM
  
#ifndef ATOMFIRSTSTATEBITPOS
#define ATOMFIRSTSTATEBITPOS (25)
#endif //ATOMFIRSTSTATEBITPOS

#ifndef MAXSTATEBITS
#define MAXSTATEBITS (BITSPERATOM - ATOMFIRSTSTATEBITPOS)
#endif //MAXSTATEBITS

#ifndef MAXBITSPERQUARK
#define MAXBITSPERQUARK (32)
#endif //MAXBITSPERQUARK

#ifndef MAXBITSPERINT
#define MAXBITSPERINT (32)
#endif //MAXBITSPERINT

#ifndef MAXBITSPERLONG
#define MAXBITSPERLONG (64)
#endif //MAXBITSPERLONG

#ifndef BITSPERBOOL
#define BITSPERBOOL (1)
#endif //BITSPERBOOL

#ifndef ANYBITSIZECONSTANT
#define ANYBITSIZECONSTANT (-1)
#endif //ANYBITSIZECONSTANT

#ifndef NONARRAYSIZE
#define NONARRAYSIZE (-1)
#endif //NONARRAYSIZE


#ifndef BASE10
#define BASE10 (10)
#endif //BASE10

#ifndef CYCLEFLAG
#define CYCLEFLAG (-2)
#endif //CYCLEFLAG

#ifndef EMPTYSYMBOLTABLE
#define EMPTYSYMBOLTABLE (-1)
#endif //EMPTYSYMBOLTABLE

#ifndef STATICSYMBOL
#define STATICSYMBOL (-3)
#endif //STATICSYMBOL

#ifndef ULAMTYPE_DEFAULTBITSIZE
#define XX(a,b,c) c,

  static const u32 ULAMTYPE_DEFAULTBITSIZE[] = {
#include "UlamType.inc"
  };
  
#undef XX 
#endif //ULAMTYPE_DEFAULTBITSIZE


  /** Number of bits (rounded up to nearest 32 bits) required to
      hold the bit size argument l */
#define calcWordSize(l) ((l / MAXBITSPERINT) * MAXBITSPERINT + ( (l % MAXBITSPERINT) > 0 ? MAXBITSPERINT : 0))

  /** Number of bits (rounded up to nearest 64 bits) required to
      hold the bit size argument l */
#define calcWordSizeLong(l) ((l / MAXBITSPERLONG) * MAXBITSPERLONG + ( (l % MAXBITSPERLONG) > 0 ? MAXBITSPERLONG : 0))

} //MFM

#endif //CONSTANTS_H
