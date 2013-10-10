/******************************************************************************
**  libDXFrw - Library to read/write DXF files (ascii & binary)              **
**                                                                           **
**  Copyright (C) 2011 Rallaz, rallazz@gmail.com                             **
**                                                                           **
**  This library is free software, licensed under the terms of the GNU       **
**  General Public License as published by the Free Software Foundation,     **
**  either version 2 of the License, or (at your option) any later version.  **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.    **
******************************************************************************/


#include "dwgbuffer.h"
#include "../libdwgr.h"
#include "drw_textcodec.h"
//#include <bitset>
/*#include <fstream>
#include <algorithm>
#include <sstream>
#include "dwgreader.h"*/
//#include "dxfwriter.h"


//#define FIRSTHANDLE 48

/*enum sections {
    secUnknown,
    secHeader,
    secTables,
    secBlocks,
    secEntities,
    secObjects
};*/

static unsigned int crctable[256]= {
0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040 };


union typeCast  {
    char buf[8];
    duint16 i16;
    duint32 i32;
    duint64 i64;
    ddouble64 d64;
};

bool dwgFileStream::setPos(int p){
/*    bool chac = p<0;
    bool pop = p >= sz;
    if (chac || pop)*/
    if (p<0 || p >= sz)
        return false;

    stream->seekg(p);
    return stream->good();
}

bool dwgFileStream::read(char* s, int n){
    stream->read (s,n);
    return stream->good();
}

bool dwgCharStream::setPos(int p){
    if (p<0 || p > size()) {
        isOk = false;
        return false;
    }

    pos = p;
    return true;
}

bool dwgCharStream::read(char* s, int n){
    if ( n > (sz - pos) ) {
        isOk = false;
        return false;
    }
    for (int i=0; i<n; i++){
        s[i]= stream[pos++];
    }
    return true;
}

dwgBuffer::dwgBuffer(char *buf, int size, DRW_TextCodec *dc){
    filestr = new dwgCharStream(buf, size);
    decoder = dc;
    maxSize = size;
    bitPos = 0;

}

dwgBuffer::dwgBuffer(std::ifstream *stream, DRW_TextCodec *dc){
    filestr = new dwgFileStream(stream);
    decoder = dc;
    maxSize = filestr->size();
    bitPos = 0;

}

dwgBuffer::~dwgBuffer(){
    delete filestr;
}

/**Sets the buffer position in pos byte, reset the bit position **/
bool dwgBuffer::setPosition(int pos){
    bitPos = 0;
/*    if (pos>=maxSize)
        return false;*/
    return filestr->setPos(pos);
//    return true;
}

void dwgBuffer::setBitPos(int pos){
    if (pos<0 || pos>7)
        return;
    bitPos = pos;
    if (bitPos != 0){
        char buffer;
        filestr->read (&buffer,1);
        currByte = buffer;
    }
}

/**Reads one Bit returns a char with value 0/1 (B) **/
duint8 dwgBuffer::getBit(){
    char buffer;
    duint8 ret = 0;
    if (bitPos == 0){
        filestr->read (&buffer,1);
        currByte = buffer;
    }

    ret = (currByte >> (7 - bitPos) & 1);
    bitPos +=1;
    if (bitPos == 8)
        bitPos = 0;

    return ret;
}

/**Reads two Bits returns a char (BB) **/
duint8 dwgBuffer::get2Bits(){
    char buffer;
    duint8 ret = 0;
    if (bitPos == 0){
        filestr->read (&buffer,1);
        currByte = buffer;
    }

    bitPos +=2;
    if (bitPos < 9)
        ret = currByte >>(8 - bitPos);
    else {//read one bit per byte
        ret = currByte << 1;
        filestr->read (&buffer,1);
        currByte = buffer;
        bitPos = 1;
        ret = ret | currByte >> 7;
    }
    if (bitPos == 8)
        bitPos = 0;
    ret = ret & 3;
    return ret;
}

/**Reads tree Bits returns a char (3B) for R24 **/
//to be written

/**Reads compresed Short (max. 16 + 2 bits) little-endian order, returns a signed 16 bits (BS) **/
dint16 dwgBuffer::getBitShort(){
    dint8 b = get2Bits();
    if (b == 0)
        return getRawShort16();
    else if (b== 1)
        return getRawChar8();
    else if (b == 2)
        return 0;
    else
        return 256;
}

/**Reads compresed 32 bits Int (max. 32 + 2 bits) little-endian order, returns a signed 32 bits (BL) **/
//to be written
dint32 dwgBuffer::getBitLong(){
    dint8 b = get2Bits();
    if (b == 0)
        return getRawLong32();
    else if (b== 1)
        return getRawChar8();
    else //if (b == 2)
        return 0;
}

/**Reads compresed LongLong (max. 56 + 7 bits) returns a unsigned 64 bits (BLL) (R24) **/
//to be written
//dint64 dwgBuffer::getBitLongLong(){}

/**Reads compresed Double (max. 64 + 2 bits) returns a floating point double of 64 bits (BD) **/
double dwgBuffer::getBitDouble(){
    dint8 b = get2Bits();
    if (b == 1)
        return 1.0;
    else if (b == 0){
        char buffer[8];
        if (bitPos != 0) {
            for (int i = 0; i < 8; i++)
                buffer[i] = getRawChar8();
        } else {
        filestr->read (buffer,8);
        }
        double ret = *reinterpret_cast<double*>( buffer );
        return ret;
    }
    //    if (b == 2)
    return 0.0;
}

/**Reads raw char 8 bits returns a unsigned char (RC) **/
duint8 dwgBuffer::getRawChar8(){
    duint8 ret;
    char buffer;
    filestr->read (&buffer,1);
    if (bitPos == 0)
        return buffer;
    else {
        ret = currByte << bitPos;
        currByte = buffer;
        ret = ret | (currByte >>(8 - bitPos));
    }
    return ret;
}

/**Reads raw short 16 bits little-endian order, returns a unsigned short (RS) **/
duint16 dwgBuffer::getRawShort16(){
    char buffer[2];
    duint16 ret;

    filestr->read (buffer,2);
    if (bitPos == 0) {
        /* no offset directly swap bytes for little-endian */
        ret = (buffer[1] << 8) | (buffer[0] & 0x00FF);
    } else {
        ret = (buffer[0] << 8) | (buffer[1] & 0x00FF);
        /* apply offset */
        ret = ret >> (8 - bitPos);
        ret = ret | (currByte << (8 + bitPos));
        currByte = buffer[1];
        /* swap bytes for little-endian */
        ret = (ret << 8) | (ret >> 8);
    }
    return ret;
}

/**Reads raw double IEEE standard 64 bits returns a double (RD) **/
double dwgBuffer::getRawDouble(){
    char buffer[8];
    if (bitPos == 0)
        filestr->read (buffer,8);
    else {
        for (int i = 0; i < 8; i++)
            buffer[i] = getRawChar8();
    }
/*    dint32 tmp = buffer[0]<<24 | buffer[1]<<16 | buffer[2]<<8 | buffer[3];
    dint32 tmp2 = buffer[4]<<24 | buffer[5]<<16 | buffer[6]<<8 | buffer[7];
    dint64 tmp3 = tmp <<32 | tmp;
    double *nOffset = reinterpret_cast<double*>(&tmp3);*/
//    &nOffset = &tmp;
    double nOffset = *reinterpret_cast<double*>( buffer );
    return nOffset;
}

/**Reads raw int 32 bits little-endian order, returns a unsigned int (RL) **/
duint32 dwgBuffer::getRawLong32(){
    duint16 tmp1 = getRawShort16();
    duint16 tmp2 = getRawShort16();
    duint32 ret = (tmp2 << 16) | (tmp1 & 0x0000FFFF);

    return ret;
}

/**Reads raw int 64 bits little-endian order, returns a unsigned long long (RLL) **/
duint64 dwgBuffer::getRawLong64(){
    duint32 tmp1 = getRawLong32();
    duint64 tmp2 = getRawLong32();
    duint64 ret = (tmp2 << 32) | (tmp1 & 0x00000000FFFFFFFF);

    return ret;
}

/**Reads modular unsigner int, char based, compresed form, little-endian order, returns a unsigned int (U-MC) **/
duint32 dwgBuffer::getUModularChar(){
    std::vector<dint8> buffer;
    dint32 result =0;
    for (int i=0; i<4;i++){
        duint8 b= getRawChar8();
        buffer.push_back(b & 0x7F);
        if (! (b & 0x80))
            break;
    }
    int offset = 0;
    for (unsigned int i=0; i<buffer.size();i++){
        result += buffer[i] << offset;
        offset +=7;
    }
    return result;
}

/**Reads modular int, char based, compresed form, little-endian order, returns a signed int (MC) **/
dint32 dwgBuffer::getModularChar(){
    bool negative = false;
    std::vector<dint8> buffer;
    dint32 result =0;
    for (int i=0; i<4;i++){
        duint8 b= getRawChar8();
        buffer.push_back(b & 0x7F);
        if (! (b & 0x80))
            break;
    }
    dint8 b= buffer.back();
    if (b & 0x40) {
        negative = true;
        buffer.pop_back();
        buffer.push_back(b & 0x3F);
    }

    int offset = 0;
    for (unsigned int i=0; i<buffer.size();i++){
        result += buffer[i] << offset;
        offset +=7;
    }
    if (negative)
        result = -result;
    return result;
}

/**Reads modular int, short based, compresed form, little-endian order, returns a unsigned int (MC) **/
dint32 dwgBuffer::getModularShort(){
//    bool negative = false;
    std::vector<dint16> buffer;
    dint32 result =0;
    for (int i=0; i<2;i++){
        duint16 b= getRawShort16();
        buffer.push_back(b & 0x7FFF);
        if (! (b & 0x8000))
            break;
    }

    //only positive ?
/*    dint8 b= buffer.back();
    if (! (b & 0x40)) {
        negative = true;
        buffer.pop_back();
        buffer.push_back(b & 0x3F);
    }*/

    int offset = 0;
    for (unsigned int i=0; i<buffer.size();i++){
        result += buffer[i] << offset;
        offset +=15;
    }
/*    if (negative)
        result = -result;*/
    return result;
}

dwgHandle dwgBuffer::getHandle(){ //H
    dwgHandle hl;
    duint8 data = getRawChar8();
    hl.code = (data >> 4) & 0x0F;
    hl.size = data & 0x0F;
    hl.ref=0;
    for (int i=0; i< hl.size;i++){
        hl.ref = (hl.ref << 8) | getRawChar8();
    }
    return hl;
}

std::string dwgBuffer::getVariableText(){
    dint16 textSize = getBitShort();
    if (textSize == 0)
        return std::string();
    char buffer[textSize+1];
    filestr->read (buffer,textSize);
    if (!filestr->good())
        return std::string();

    unsigned char tmp;
    if (bitPos != 0){
        for (int i=0; i<textSize;i++){
            tmp =  buffer[i];
            buffer[i] = (currByte << bitPos) | (tmp >> (8 - bitPos));
            currByte = tmp;
        }
    }
    buffer[textSize]= '\0';
    return std::string(buffer);
}

std::string dwgBuffer::getVariableUtf8Text(){
    std::string strData;
    strData = getVariableText();
    if (decoder == NULL)
        return strData;

    return decoder->toUtf8(strData);
}

/* Bit Extrusion
* For R13-R14 this is 3BD. We are asserting that the version
* is not R13-R14; this values should be read by the user
* For R2000, this is a single bit, If the single bit is 1,
* the extrusion value is assumed to be 0,0,1 and no explicit
* extrusion is stored. If the single bit is 0, then it will
* be followed by 3BD.
*/
DRW_Coord dwgBuffer::getExtrusion(bool b_R2000_style) {
    DRW_Coord ext(0.0,0.0,1.0);
    if ( b_R2000_style )
        /* If the bit is one, the extrusion value is assumed to be 0,0,1*/
        if ( getBit() == 1 )
            return ext;
    /*R13-R14 or bit == 0*/
    ext.x = getBitDouble();
    ext.y = getBitDouble();
    ext.z = getBitDouble();
    return ext;
}

/**Reads compresed Double with default (max. 64 + 2 bits) returns a floating point double of 64 bits (DD) **/
double dwgBuffer::getDefaultDouble(double d){
    dint8 b = get2Bits();
    if (b == 0)
        return d;
    else if (b == 1){
        char buffer[4];
        char *tmp;
        if (bitPos != 0) {
            for (int i = 0; i < 4; i++)
                buffer[i] = getRawChar8();
        } else {
        filestr->read (buffer,4);
        }
        tmp = reinterpret_cast<char*>(&d);
        for (int i = 0; i < 4; i++)
            tmp[i] = buffer[i];
        double ret = *reinterpret_cast<double*>( tmp );
        return ret;
    } else if (b == 2){
        char buffer[6];
        char *tmp;
        if (bitPos != 0) {
            for (int i = 0; i < 6; i++)
                buffer[i] = getRawChar8();
        } else {
        filestr->read (buffer,6);
        }
        tmp = reinterpret_cast<char*>(&d);
        for (int i = 2; i < 6; i++)
            tmp[i-2] = buffer[i];
        tmp[4] = buffer[0];
        tmp[5] = buffer[1];
        double ret = *reinterpret_cast<double*>( tmp );
        return ret;
    }
    //    if (b == 3) return a full raw double
    return getRawDouble();
}


/* BitThickness
* For R13-R14, this is a BD. We are asserting that the version
* is not R13-R14; this value should be read by the user
* For R2000+, this is a single bit, If the bit is one,
* the thickness value is assumed to be 0.0, if not a BD follow
*/
double dwgBuffer::getThickness(bool b_R2000_style) {
    if ( b_R2000_style )
        /* If the bit is one, the thickness value is assumed to be 0.0.*/
        if ( getBit() == 1 )
            return 0.0;
    /*R13-R14 or bit == 0*/
    return getBitDouble();
}

/**Reads raw short 16 bits big-endian order, returns a unsigned short crc & size **/
duint16 dwgBuffer::getBERawShort16(){
    char buffer[2];
    buffer[0] = getRawChar8();
    buffer[1] = getRawChar8();
    duint16 size = (buffer[0] << 8) | (buffer[1] & 0xFF);
    return size;
}

/* reads "size" bytes and stores in "buf" return false if fail */
bool dwgBuffer::getBytes(char *buf, int size){
    unsigned char tmp;
    filestr->read (buf,size);
    if (!filestr->good())
        return false;

    if (bitPos != 0){
        for (int i=0; i<size;i++){
            tmp =  buf[i];
            buf[i] = (currByte << bitPos) | (tmp >> (8 - bitPos));
            currByte = tmp;
        }
    }
    return true;
}

duint16 dwgBuffer::crc8(duint16 dx,dint32 start,dint32 end){
    int pos = filestr->getPos();
    filestr->setPos(start);
    int n = end-start;
    char buf[n];
    unsigned char *p = (unsigned char *)buf;
    filestr->read (buf,n);
    filestr->setPos(pos);
    if (!filestr->good())
        return 0;

    register duint8 al;

  while (n-- > 0) {
    al = (duint8)((*p) ^ ((dint8)(dx & 0xFF)));
    dx = (dx>>8) & 0xFF;
    dx = dx ^ crctable[al & 0xFF];
    p++;
  }
  return(dx);
}

/*std::string dwgBuffer::getBytes(int size){
    char buffer[size];
    char tmp;
    filestr->read (buffer,size);
    if (!filestr->good())
        return NULL;

    if (bitPos != 0){
        for (int i=0; i<=size;i++){
            tmp =  buffer[i];
            buffer[i] = (currByte << bitPos) | (tmp >> (8 - bitPos));
            currByte = tmp;
        }
    }
    std::string st;
    for (int i=0; i<size;i++) {
        st.push_back(buffer[i]);
    }
    return st;
//    return std::string(buffer);
}*/

