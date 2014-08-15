/**                                        -*- mode:C++ -*-
 * UlamKeyTypeSignature.h -  Basic handling of UlamType Keys for ULAM
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
  \file UlamKeyTypeSignature.h -  Basic handling of UlamType Keys for ULAM
  \author Elenas S. Ackley.
  \author David H. Ackley.
  \date (C) 2014 All rights reserved.
  \gpl
*/


#ifndef ULAMKEYTYPESIGNATURE_H
#define ULAMKEYTYPESIGNATURE_H

#include <string>
#include "itype.h"


namespace MFM{
  
#define MAX_TYPENAME_LEN 15
  
  struct UlamKeyTypeSignature
  {
    char m_typeName[MAX_TYPENAME_LEN + 1];
    u32  m_bits;
    u32 m_arraySize;

    UlamKeyTypeSignature();
    UlamKeyTypeSignature(const char * name, u32 bitsize, u32 arraysize=0);
    ~UlamKeyTypeSignature();
    
    char * getUlamKeyTypeSignatureName();
    u32 getUlamKeyTypeSignatureBitSize();
    u32 getUlamKeyTypeSignatureArraySize();
    const std::string getUlamKeyTypeSignatureAsString();
    static const std::string getUlamKeyTypeSignatureAsString(UlamKeyTypeSignature utk);
    bool operator<(const UlamKeyTypeSignature & key2);
    bool operator==(const UlamKeyTypeSignature & key2);

  };
  
  
}

#endif //end ULAMKEYTYPESIGNATURE_H
