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

#ifndef DWGBUFFER_H
#define DWGBUFFER_H

#include <fstream>
#include <sstream>
#include "../drw_base.h"

class DRW_Coord;
class DRW_TextCodec;

class dwgBasicStream{
protected:
    dwgBasicStream(){}
public:
    virtual ~dwgBasicStream(){}
    virtual bool read(char* s, int n) = 0;
    virtual int size() = 0;
    virtual int getPos() = 0;
    virtual bool setPos(int p) = 0;
    virtual bool good() = 0;
};

class dwgFileStream: public dwgBasicStream{
public:
    dwgFileStream(std::ifstream *s){
        stream =s;
        stream->seekg (0, std::ios::end);
        sz = stream->tellg();
        stream->seekg(0, std::ios_base::beg);
    }
    virtual ~dwgFileStream(){}
    virtual bool read(char* s, int n);
    virtual int size(){return sz;}
    virtual int getPos(){return stream->tellg();}
    virtual bool setPos(int p);
    virtual bool good(){return stream->good();}
private:
    std::istream *stream;
    int sz;
};

class dwgCharStream: public dwgBasicStream{
public:
    dwgCharStream(char *buf, int s){
        stream =buf;
        sz = s;
        pos = 0;
        isOk = true;
    }
    virtual ~dwgCharStream(){}
    virtual bool read(char* s, int n);
    virtual int size(){return sz;}
    virtual int getPos(){return pos;}
    virtual bool setPos(int p);
    virtual bool good(){return isOk;}
private:
    char *stream;
    int sz;
    int pos;
    bool isOk;
};

class dwgBuffer {
public:
    dwgBuffer(std::ifstream *stream, DRW_TextCodec *decoder = NULL);
    dwgBuffer(char *buf, int size, DRW_TextCodec *decoder= NULL);
    ~dwgBuffer();
    duint32 size(){return filestr->size();}
    bool setPosition(int pos);
    int getPosition(){return filestr->getPos();}
    void setBitPos(int pos);
    int getBitPos(){return bitPos;}

    duint8 getBit();  //B
    duint8 get2Bits(); //BB
    //3B to be written
    dint16 getBitShort(); //BS
    dint32 getBitLong(); //BL
    //dint64 dwgBuffer::getBitLongLong(){}  //BLL (R24)
    double getBitDouble(); //BD
    //2BD ?
    //3BD ?
    duint8 getRawChar8();  //RC
    duint16 getRawShort16();  //RS
    double getRawDouble(); //RD
    duint32 getRawLong32();   //RL
    duint64 getRawLong64();   //RLL
    //2RD ?
    //3RD ?
    duint32 getUModularChar(); //UMC, unsigned for offsets in 1015
    dint32 getModularChar(); //MC
    dint32 getModularShort(); //MS
    dwgHandle getHandle(); //H
    dwgHandle getOffsetHandle(duint32 href); //H converted to hard
    //T, TU, TV, X, U, SN,
    std::string getVariableText();
    std::string getVariableUtf8Text();
    DRW_Coord getExtrusion(bool b_R2000_style); //BE
    double getDefaultDouble(double d); //DD
    double getThickness(bool b_R2000_style);//BT
    //3DD, CMC, TC

    duint16 getBERawShort16();
    bool isGood(){return filestr->good();}
    bool getBytes(char *buf, int size);
    int numRemainingBytes(){return (maxSize- filestr->getPos());}

//    dint16 crc8(duint16 dx,dint8 *p,dint32 n);
    duint16 crc8(duint16 dx,dint32 start,dint32 end);

    duint8 getCurrByte(){return currByte;}
    DRW_TextCodec *decoder;

private:
    dwgBasicStream *filestr;
    int maxSize;
    duint8 currByte;
    duint8 bitPos;
};

#endif // DWGBUFFER_H
