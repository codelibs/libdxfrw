/******************************************************************************
**  libDXFrw - Library to read/write DXF files (ascii & binary)              **
**                                                                           **
**  Copyright (C) 2011-2013 Rallaz, rallazz@gmail.com                        **
**                                                                           **
**  This library is free software, licensed under the terms of the GNU       **
**  General Public License as published by the Free Software Foundation,     **
**  either version 2 of the License, or (at your option) any later version.  **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.    **
******************************************************************************/

#ifndef DWGUTIL_H
#define DWGUTIL_H

//#include <fstream>
//#include <sstream>
#include "../drw_base.h"

class dwgCompressor {
public:
    dwgCompressor(){}
    ~dwgCompressor(){}

    void decompress(char *cbuf, char *dbuf, duint32 csize, duint32 dsize);
    static void decrypt(char *buf, duint32 size, duint32 offset);

private:
    duint32 litLength();
    duint32 longCompressionOffset();
    duint32 twoByteOffset(duint32 *ll);

    char *bufC;
    char *bufD;
    duint32 sizeC;
    duint32 sizeD;
    duint32 pos;
    duint32 rpos;

};

#endif // DWGUTIL_H
