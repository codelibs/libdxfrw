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


#include "drw_dbg.h"
#include "dwgutil.h"
#include "../libdwgr.h"

duint32 dwgCompressor::twoByteOffset(duint32 *ll){
    duint32 cont = 0;
    duint8 fb = bufC[pos++];
    cont = (fb >> 2) | (bufC[pos++] << 6);
    *ll = (fb & 0x03);
    return cont;
}

duint32 dwgCompressor::longCompressionOffset(){
    duint32 cont = 0;
    duint8 ll = bufC[pos++];
    while (ll == 0x00){
        cont += 0xFF;
        ll = bufC[pos++];
    }
    cont += ll;
    return cont;
}

duint32 dwgCompressor::litLength(){
    duint32 cont;
    duint8 ll = bufC[pos++];
    if (ll > 0x0F) {
        pos--;
        return 0;
    }

    if (ll == 0x00) {
        cont = 0x0F;
        ll = bufC[pos++];
        if (ll == 0x00)
            cont +=0xFF;//repeat until ll != 0x00 ???
        else cont +=ll;
    } else {
        cont = ll;
    }
    cont +=3; //if (buf[1] == 0x00) already sum 3 ???
    return cont;
//    ll = bufC[pos++];
}

void dwgCompressor::decompress(char *cbuf, char *dbuf, duint32 csize, duint32 dsize){
//                           char *buf, char *result, int size){
    bufC = cbuf;
    bufD = dbuf;
    sizeC = csize;
    sizeD = dsize;

//    duint8 cont;
    duint32 compBytes;
    duint32 compOffset;
    duint32 litCount;

    pos=0;
    rpos=0;
//    duint32 ll = litLength();
    duint32 tmp = litLength();
//    duint32 tmp = ll+pos;
    tmp += pos;
    for (duint32 i=pos; i < tmp; i++) {
        bufD[rpos++]= bufC[pos++];
    }

    while (pos < csize && rpos < dsize){//rpos < dsize to prevent crash more robust are needed
        duint8 ll = bufC[pos++];
        if (ll == 0x10){
            compBytes = longCompressionOffset()+ 9;
            compOffset = twoByteOffset(&litCount);
            if (litCount == 0)
                litCount= litLength();
        } else if (ll == 0x11){
            return; //end of input stream
        } else if (ll > 0x11 && ll< 0x20){
            compBytes = (ll & 0x0F) + 2;
            compOffset = twoByteOffset(&litCount) + 0x3FFF;
            if (litCount == 0)
                litCount= litLength();
        } else if (ll == 0x20){
            compBytes = longCompressionOffset()+ 0x21;
            compOffset = twoByteOffset(&litCount);
            if (litCount == 0)
                litCount= litLength();
        } else if (ll > 0x20 && ll< 0x40){
            compBytes = ll - 0x1E;
            compOffset = twoByteOffset(&litCount);
            if (litCount == 0)
                litCount= litLength();
        } else if ( ll > 0x3F){
            compBytes = ((ll & 0xF0) >> 4) - 1;
            duint8 ll2 = bufC[pos++];
            compOffset =  (ll2 << 2) | ((ll & 0x0C) >> 2);
            litCount = ll & 0x03;
            litCount++;
            litCount--;
            if (litCount < 1){
                litCount= litLength();}
/*            if (litCount == 0){
                litCount= litLength();}*/
        } else { //ll < 0x10
            return; //fails, not valid
        }
        //copy "compresed data", TODO Needed verify out of bounds
        for (duint32 i=0, j= rpos - compOffset -1; i < compBytes; i++) {
            bufD[rpos++] = bufD[j++];
        }
        //copy "uncompresed data", TODO Needed verify out of bounds
        for (duint32 i=0; i < litCount; i++) {
            bufD[rpos++] = bufC[pos++];
        }
    }
    DBG("dwgCompressor::decompress, pos: ");DBG(pos);DBG(", rpos: ");DBG(rpos);DBG("\n");
}


void dwgCompressor::decrypt(char *buf, duint32 size, duint32 offset){
    duint8 max = size / 4;
    duint32 secMask = 0x4164536b ^ offset;
    duint32* pHdr = (duint32*)buf;
    for (duint8 j = 0; j < max; j++)
        *pHdr++ ^= secMask;
}
