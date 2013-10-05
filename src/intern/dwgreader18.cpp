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

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "drw_dbg.h"
#include "dwgreader18.h"
#include "dwgutil.h"
#include "drw_textcodec.h"
#include "../libdwgr.h"

void dwgReader18::genMagicNumber(){
    int size =0x114;
    duint8 magicStr[size];
    duint8 *p = magicStr;
    int rSeed =1;
    while (size--) {
        rSeed *= 0x343fd;
        rSeed += 0x269ec3;
        *p++ = (duint8)(rSeed >> 0x10);
    }
    int j = 0;
    size =0x114;
    for (int i=0; i< size;i++) {
        DBGH(magicStr[i]);
        if (j == 15) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
    }

}

bool dwgReader18::readFileHeader() {
    version = parent->getVersion();
    decoder.setVersion(version);
    DBG("dwgReader18::readFileHeader\n");
    if (! buf->setPosition(11))
        return false;
    DBG("maintenance verion= "); DBGH(buf->getRawChar8()); DBG("\n");
    DBG("byte at 0x0C= "); DBGH(buf->getRawChar8()); DBG("\n");
    seekerImageData = buf->getRawLong32(); //+ page header size (0x20).
    DBG("seekerImageData= "); DBG(seekerImageData); DBG("\n");
    DBG("app Dwg verion= "); DBGH(buf->getRawChar8()); DBG(", ");
    DBG("app maintenance verion= "); DBGH(buf->getRawChar8()); DBG("\n");
    duint16 cp = buf->getRawShort16();
    DBG("codepage= "); DBG(cp); DBG("\n");
    if (cp == 30)
        decoder.setCodePage("ANSI_1252");
    DBG("3 0x00 bytes(seems 0x00, appDwgV & appMaintV= "); DBGH(buf->getRawChar8()); DBG(", ");
    DBGH(buf->getRawChar8()); DBG(", "); DBGH(buf->getRawChar8()); DBG("\n");
    duint32 secFlags = buf->getRawLong32();
    DBG("security flags= "); DBG(secFlags); DBG("\n");
    /* UNKNOUWN SECTION 4 bytes*/
    duint32 uk =    buf->getRawLong32();
    DBG("UNKNOUWN SECTION= "); DBG(uk); DBG("\n");
    duint32 sumInfo =    buf->getRawLong32();
    DBG("summary Info= "); DBG(sumInfo); DBG("\n");
    duint32 vbaAdd =    buf->getRawLong32();
    DBG("VBA address= "); DBG(vbaAdd); DBG("\n");
    DBG("0x00000080= "); DBGH(buf->getRawLong32()); DBG("\n");
    if (! buf->setPosition(0x80))
        return false;

//    genMagicNumber(); DBG("\n"); DBG("\n");
    char byteStr[0x6C];
    int size =0x6C;
    for (int i=0, j=0; i< 0x6C;i++) {
#ifdef DRWG_DBG
        duint8 ch = buf->getRawChar8();
        DBGH(ch);
        if (j == 15) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
        byteStr[i] = DRW_magicNum18[i] ^ ch;
#else
        byteStr[i] = DRW_magicNum18[i] ^ buf->getRawChar8();
#endif
    }
    DBG("\n");

    size =0x6C;
    for (int i=0, j = 0; i< size;i++) {
        DBGH( (unsigned char)byteStr[i]);
        if (j == 15) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
    }
    DBG("\n");
    dwgBuffer buff(byteStr, 0x6C, &decoder);
    std::string name = byteStr;
    DBG("File ID string (AcFssFcAJMB)= "); DBG(name.c_str());DBG("\n");
    //ID string + NULL = 12
    buff.setPosition(12);
    DBG("0x00 long= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("0x6c long= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("0x04 long= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("Root tree node gap= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("Lowermost left tree node gap= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("Lowermost right tree node gap= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("Unknown long (1)= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("Last section page Id= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("Last section page end address 64b= "); DBGH(buff.getRawLong64()); DBG("\n");
    DBG("Second header data address 64b= "); DBGH(buff.getRawLong64()); DBG("\n");
    DBG("Gap amount= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("Section page amount= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("0x20 long= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("0x80 long= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("0x40 long= "); DBGH(buff.getRawLong32()); DBG("\n");
    dint32 secPageMapId = buff.getRawLong32();
    DBG("Section Page Map Id= "); DBGH(secPageMapId); DBG("\n");
    duint64 secPageMapAddr = buff.getRawLong64()+0x100;
    DBG("Section Page Map address 64b= "); DBGH(secPageMapAddr); DBG("\n");
    DBG("Section Page Map address 64b dec= "); DBG(secPageMapAddr); DBG("\n");
    duint32 secMapId = buff.getRawLong32();
    DBG("Section Map Id= "); DBGH(secMapId); DBG("\n");
    DBG("Section page array size= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("Gap array size= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("CRC32= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("End Encrypted Data 0x14 size:\n");
    for (int i=0, j=0; i< 0x14;i++) {
        DBG("magic num: "); DBGH( (unsigned char)DRW_magicNumEnd18[i]);
        DBG(",read "); DBGH( (unsigned char)buf->getRawChar8());
        if (j == 3) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
    }

    if (! buf->setPosition(secPageMapAddr))
        return false;
    DBG("Section page type= "); DBGH(buf->getRawLong32()); DBG("\n");
    duint32 decompSize = buf->getRawLong32();
    DBG("Decompressed size= "); DBG(decompSize); DBG(", "); DBGH(decompSize); DBG("\n");
    duint32 compSize = buf->getRawLong32();
    DBG("Compressed size= "); DBG(compSize); DBG(", "); DBGH(compSize); DBG("\n");
    DBG("Compression type= "); DBGH(buf->getRawLong32()); DBG("\n");
    DBG("Section page checksum= "); DBGH(buf->getRawLong32()); DBG("\n");
    char compSec[compSize];
    char decompSec[decompSize];
    buf->getBytes(compSec, compSize);
    for (unsigned int i=0, j=0; i< compSize;i++) {
        DBGH( (unsigned char)compSec[i]);
        if (j == 7) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
    } DBG("\n");
    dwgCompressor comp;
    comp.decompress(compSec, decompSec, compSize, decompSize);
    for (unsigned int i=0, j=0; i< decompSize;i++) {
        DBGH( (unsigned char)decompSec[i]);
        if (j == 7) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
    } DBG("\n");
    dwgBuffer buff2(decompSec, decompSize, &decoder);
    duint32 seek = 0x100;
    std::map<dint32, std::pair<dint32,dint32 > >sectionMapTmp;

    for (unsigned int i = 0; i < decompSize;) {
        dint32 num = buff2.getRawLong32();
        duint32 size = buff2.getRawLong32();
        i += 8;
        DBG("Section num= "); DBG(num); DBG("seek= "); DBGH(seek);
        DBG(" size= "); DBGH(size);  DBG("\n");
        sectionMapTmp[num] = std::pair<dint32,dint32>(seek,size);
        //TODO num can be negative indicating gap
/*        if (num == secPageMapId) {
            DBG("Page Map Id= "); DBG(num); DBG("seek= "); DBG(seek);
            DBG("address= "); DBG(secPageMapAddr); DBG(" size= "); DBG(size);  DBG("\n");
            sections["PageMapId"] = std::pair<dint32,dint32>(seek,size);
        }*/
        seek += size;
    }

    DBG("*** Processing Section Map ***\n");
    seek = sectionMapTmp[secMapId].first;
    size = sectionMapTmp[secMapId].second;
    if (!buf->setPosition(seek))
        return false;
    DBG("Section page type= "); DBGH(buf->getRawLong32()); DBG("\n");
    decompSize = buf->getRawLong32();
    DBG("Decompressed size= "); DBG(decompSize); DBG(", "); DBGH(decompSize); DBG("\n");
    compSize = buf->getRawLong32();
    DBG("Compressed size= "); DBG(compSize); DBG(", "); DBGH(compSize); DBG("\n");
    DBG("Compression type= "); DBGH(buf->getRawLong32()); DBG("\n");
    DBG("Section page checksum= "); DBGH(buf->getRawLong32()); DBG("\n");
    char compSec2[compSize];
    char decompSec2[decompSize];
    buf->getBytes(compSec2, compSize);
    for (unsigned int i=0, j=0; i< compSize;i++) {
        DBGH( (unsigned char)compSec2[i]);
        if (j == 7) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
    } DBG("\n");
    comp.decompress(compSec2, decompSec2, compSize, decompSize);
    for (unsigned int i=0, j=0; i< decompSize;i++) {
        DBGH( (unsigned char)decompSec2[i]);
        if (j == 7) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
    } DBG("\n");

    dwgBuffer buff3(decompSec2, decompSize, &decoder);
    duint32 numDescriptions = buff3.getRawLong32();
    DBG("numDescriptions= "); DBGH(numDescriptions); DBG("\n");
    DBG("0x20 long= "); DBGH(buff3.getRawLong32()); DBG("\n");
    DBG("0x00007400 long= "); DBGH(buff3.getRawLong32()); DBG("\n");
    DBG("0x00 long= "); DBGH(buff3.getRawLong32()); DBG("\n");
    DBG("unknown long (numDescriptions)= "); DBGH(buff3.getRawLong32()); DBG("\n");
    for (unsigned int i = 0; i < numDescriptions; i++) {
        dwgSectionInfo secInfo;
        DBG("\n"); DBG("Size of section= "); DBGH(buff3.getRawLong64()); DBG("\n");
        duint8 pageCount = buff3.getRawLong32();
        DBG("Page count= "); DBGH(pageCount); DBG("\n");
        DBG("Max Decompressed Size= "); DBGH(buff3.getRawLong32()); DBG("\n");
        DBG("unknown long= "); DBGH(buff3.getRawLong32()); DBG("\n");
        secInfo.compresed = buff3.getRawLong32();
        DBG("is Compressed?= "); DBGH(secInfo.compresed); DBG("\n");
        secInfo.Id = buff3.getRawLong32();
        DBG("Section Id= "); DBGH(secInfo.Id); DBG("\n");
        secInfo.encrypted = buff3.getRawLong32();
        //encrypted (doc: 0 no, 1 yes, 2 unkn) on read: objects 0 and encrypted yes
        DBG("Encrypted= "); DBGH(secInfo.encrypted); DBG("\n");
        char nameCStr[64];
        buff3.getBytes(nameCStr, 64);
        secInfo.name = nameCStr;
        DBG("Section std::Name= "); DBG( secInfo.name.c_str() ); DBG("\n");
        for (unsigned int i = 0; i < pageCount; i++){
            duint32 pn = buff3.getRawLong32();
            duint32 ds = buff3.getRawLong32();
            secInfo.pages[pn] = std::pair<dint32,dint64>(ds,buff3.getRawLong64());
            DBG("    Page number= "); DBGH(pn); DBG("\n");
            DBG("    Data size= "); DBGH(secInfo.pages[pn].first); DBG("\n");
            DBG("    Start offset= "); DBGH(secInfo.pages[pn].second); DBG("\n");
        }
        if (!secInfo.name.empty()) {
            DBG("Saved section Name= "); DBG( secInfo.name.c_str() );
            DBG(" seek= "); DBGH(sectionMapTmp[secInfo.Id].first);
            DBG(" size= "); DBGH(sectionMapTmp[secInfo.Id].second); DBG("\n");
            sections[secInfo.name] = sectionMapTmp[secInfo.Id];
            sectionInfo[secInfo.name] = secInfo;
        }
    }

    if (! buf->isGood())
        return false;
    DBG("dwgReader18::readFileHeader END\n\n");
    return true;
}

bool dwgReader18::readDwgClasses(){
    DBG("dwgReader18::readDwgClasses");
    dint32 offset = sections["CLASSES"].first;
    dint32 secSize = sections["CLASSES"].second;
    DRW_UNUSED(secSize);
    if (!buf->setPosition(offset))
        return false;
    return true;
}

/*********** objects map ************************/
/** Note: object map are split in sections with max size 2035?
 *  heach section are 2 bytes size + data bytes + 2 bytes crc
 *  size value are data bytes + 2 and to calculate crc are used
 *  2 bytes size + data bytes
 *  last section are 2 bytes size + 2 bytes crc (size value always 2)
**/
bool dwgReader18::readDwgObjectOffsets() {
    DBG("dwgReader18::readDwgObjectOffsets\n");
    dint32 offset = sections["AcDb:AcDbObjects"].first;
    dint32 maxPos = offset + sections["AcDb:AcDbObjects"].second;
    DBG("Section OBJECTS offset= "); DBG(offset); DBG("\n");
    DBG("Section OBJECTS size= "); DBG(sections["AcDb:AcDbObjects"].second); DBG("\n");
    DBG("Section OBJECTS maxPos= "); DBG(maxPos); DBG("\n");
    if (!buf->setPosition(offset))
        return false;
    DBG("Section OBJECTS buf->curPosition()= "); DBG(buf->getPosition()); DBG("\n");

    dwgSectionInfo si = sectionInfo["AcDb:AcDbObjects"];
    char hdrData[32];
    buf->getBytes(hdrData, 32);
//    dwgCompressor comp;
//    comp.decrypt(buff, 32);
//    if (si.encrypted){
    dwgCompressor::decrypt(hdrData, 32, offset);
//    }
    DBG("Section OBJECTS page header= "); DBG("\n");
    for (unsigned int i=0, j=0; i< 32;i++) {
        DBGH( (unsigned char)hdrData[i]);
        if (j == 7) {
            DBG("\n");
            j = 0;
        } else {
            DBG(", ");
            j++;
        }
    } DBG("\n");
    dwgBuffer buff(hdrData, 32, &decoder);
    DBG("section page type= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("section number= "); DBGH(buff.getRawLong32()); DBG("\n");
    duint32 cSize = buff.getRawLong32();
    DBG("data size (compressed)= "); DBGH(cSize); DBG("\n");
    duint32 dSize = buff.getRawLong32();
    DBG("section size (decompressed)= "); DBGH(dSize); DBG("\n");
    DBG("start offset (in decompressed buffer)= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("header checksum= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("data checksum= "); DBGH(buff.getRawLong32()); DBG("\n");
    DBG("unknown= "); DBGH(buff.getRawLong32()); DBG("\n");
    char cData[cSize];
    char dData[dSize];
    if (!buf->setPosition(offset+32))
        return false;
    buf->getBytes(cData, cSize);
    dwgCompressor comp;
    comp.decompress(cData, dData, cSize, dSize);
    dwgBuffer buff2(dData, dSize, &decoder);
    DBG("meaning unknown= "); DBGH(buff2.getRawLong32()); DBG("\n");
    dint32 lastLoc = buff2.getPosition();
    for (unsigned int i = lastLoc; i<dSize;){
    int size = buff2.getModularShort();
    DBG("readDwgObject size: "); DBG(size); DBG("\n");
    duint32 type = buf->getBitShort();
    DBG("readDwgObject object type: "); DBG(type); DBG("\n");
    DBG("readDwgObject object data size: "); DBG(buff2.getRawLong32()); DBG("\n");
    dwgHandle hl = buff2.getHandle();
    buff2.getRawLong32();
    ObjectMap.push_back(objHandle(type, hl.ref, lastLoc));
    DBG("readDwgObject object Handle: "); DBG(hl.code); DBG(".");
    DBG(hl.size); DBG("."); DBG(hl.ref); DBG("\n");
    i = lastLoc += size;
    }

/*    int lastHandle = 0;
    int lastLoc = 0;
    //read data
    while(buff.getPosition()< size){
        pppp = buff.getPosition();
        lastHandle += buff.getModularChar();
        DBG("object map lastHandle= "); DBG(lastHandle); DBG("\n");
        lastLoc += buff.getModularChar();
        DBG("object map lastLoc= "); DBG(lastLoc); DBG("\n");
        ObjectMap.push_back(objHandle(0, lastHandle, lastLoc));
    }
//    bool ret = buf->isGood();

    //read object types
//    DBG("readDwgObjects() Total objects: "); DBG(ObjectMap.size()); DBG("\n");
    for (std::list<objHandle>::iterator it=ObjectMap.begin(); it != ObjectMap.end(); ++it){
        DBG("object map Handle= "); DBG(it->handle); DBG(" "); DBG(it->loc); DBG("\n");
        ret = buf->setPosition(it->loc); //loc == offset
        if (!ret){
            ret = false;
            continue;
        }
        int size = buf->getModularShort();
        DBG("readDwgObject size: "); DBG(size); DBG("\n");
        it->type = buf->getBitShort();
        DBG("readDwgObject object type: "); DBG(it->type); DBG("\n");
    }

    return ret;*/

/*    int startPos = offset;

    while (maxPos > buf->getPosition()) {
        int pppp = buf->getPosition();
        DBG("start object section buf->curPosition()= "); DBG(pppp); DBG("\n");
        duint16 size = buf->getBERawShort16();
        DBG("object map section size= "); DBG(size); DBG("\n");
        buf->setPosition(startPos);
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        if (size != 2){
            buff.setPosition(2);
            int lastHandle = 0;
            int lastLoc = 0;
            //read data
            while(buff.getPosition()< size){
                pppp = buff.getPosition();
                lastHandle += buff.getModularChar();
                DBG("object map lastHandle= "); DBG(lastHandle); DBG("\n");
                lastLoc += buff.getModularChar();
                DBG("object map lastLoc= "); DBG(lastLoc); DBG("\n");
                ObjectMap.push_back(objHandle(0, lastHandle, lastLoc));
            }
        }
        //verify crc
        duint16 crcCalc = buff.crc8(0xc0c1,0,size);
        duint16 crcRead = buf->getBERawShort16();
        DBG("object map section crc8 read= "); DBG(crcRead); DBG("\n");
        DBG("object map section crc8 calculated= "); DBG(crcCalc); DBG("\n");
        pppp = buf->getPosition();
        DBG("object section buf->curPosition()= "); DBG(pppp); DBG("\n");
        startPos = buf->getPosition();
    }

    bool ret = buf->isGood();

    //read object types
    DBG("readDwgObjects() Total objects: "); DBG(ObjectMap.size()); DBG("\n");
    for (std::list<objHandle>::iterator it=ObjectMap.begin(); it != ObjectMap.end(); ++it){
        DBG("object map Handle= "); DBG(it->handle); DBG(" "); DBG(it->loc); DBG("\n");
        ret = buf->setPosition(it->loc); //loc == offset
        if (!ret){
            ret = false;
            continue;
        }
        int size = buf->getModularShort();
        DBG("readDwgObject size: "); DBG(size); DBG("\n");
        it->type = buf->getBitShort();
        DBG("readDwgObject object type: "); DBG(it->type); DBG("\n");
    }

    return ret;*/
    return false;
}

/*********** objects ************************/
/**
 * Reads all the object referenced in the object map section of the DWG file
 * (using their object file offsets)
 */
bool dwgReader18::readDwgTables() {
    bool ret = true;
    bool ret2 = true;
    std::list<objHandle>ObjectControlMap;
    std::list<objHandle>LineTypeMap;
    std::list<objHandle>LayerMap;
    std::list<objHandle>BlockRecordMap;
    std::list<objHandle>StyleMap;
    std::list<objHandle>DimstyleMap;
    std::list<objHandle>VportMap;

//separate control object, layers and linetypes
    for (std::list<objHandle>::iterator it=ObjectMap.begin(); it != ObjectMap.end(); /*++it*/){
        if (it->type == 0x30 || it->type == 0x32 || it->type == 0x34 || it->type == 0x38 || it->type == 0x3C
                || it->type == 0x3E || it->type == 0x40 || it->type == 0x42 || it->type == 0x44 || it->type == 0x46){
            ObjectControlMap.push_back(*it);
            it = ObjectMap.erase(it);
        } else if (it->type == 0x39){
            LineTypeMap.push_back(*it);
            it = ObjectMap.erase(it);
        } else if (it->type == 0x33){
            LayerMap.push_back(*it);
            it = ObjectMap.erase(it);
        } else if (it->type == 0x31 || it->type == 0x4 || it->type == 0x5){
            BlockRecordMap.push_back(*it);
            it = ObjectMap.erase(it);
        } else if (it->type == 0x35){
            StyleMap.push_back(*it);
            it = ObjectMap.erase(it);
        } else if (it->type == 0x45){
            DimstyleMap.push_back(*it);
            it = ObjectMap.erase(it);
        } else if (it->type == 0x41){
            VportMap.push_back(*it);
            it = ObjectMap.erase(it);
        } else
            it++;
    }
    //parse object controls
    //parse linetypes
    for (std::list<objHandle>::iterator it=LineTypeMap.begin(); it != LineTypeMap.end(); ++it){
        DBG("LineTypeMap map Handle= "); DBG(it->handle); DBG(" "); DBG(it->loc); DBG("\n");
        DRW_LType *lt = new DRW_LType();
        buf->setPosition(it->loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = lt->parseDwg(version, &buff);
        ltypemap[lt->handle] = lt;
        if(ret)
            ret = ret2;
    }

    //parse layers
    for (std::list<objHandle>::iterator it=LayerMap.begin(); it != LayerMap.end(); ++it){
        DBG("LayerMap map Handle= "); DBG(it->handle); DBG(" "); DBG(it->loc); DBG("\n");
        DRW_Layer *la = new DRW_Layer();
        buf->setPosition(it->loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = la->parseDwg(version, &buff);
        layermap[la->handle] = la;
        if(ret)
            ret = ret2;
    }

    //parse text styles
    for (std::list<objHandle>::iterator it=StyleMap.begin(); it != StyleMap.end(); ++it){
        DBG("StyleMap map Handle= "); DBG(it->handle); DBG(" "); DBG(it->loc); DBG("\n");
        DRW_Textstyle *la = new DRW_Textstyle();
        buf->setPosition(it->loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = la->parseDwg(version, &buff);
        stylemap[la->handle] = la;
        if(ret)
            ret = ret2;
    }

    //parse dimstyles
    for (std::list<objHandle>::iterator it=DimstyleMap.begin(); it != DimstyleMap.end(); ++it){
        DBG("DimstyleMap map Handle= "); DBG(it->handle); DBG(" "); DBG(it->loc); DBG("\n");
        DRW_Dimstyle *la = new DRW_Dimstyle();
        buf->setPosition(it->loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = la->parseDwg(version, &buff);
        dimstylemap[la->handle] = la;
        if(ret)
            ret = ret2;
    }

    //parse vports
    for (std::list<objHandle>::iterator it=VportMap.begin(); it != VportMap.end(); ++it){
        DBG("VportMap map Handle= "); DBG(it->handle); DBG(" "); DBG(it->loc); DBG("\n");
        DRW_Vport *la = new DRW_Vport();
        buf->setPosition(it->loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = la->parseDwg(version, &buff);
        vportmap[la->handle] = la;
        if(ret)
            ret = ret2;
    }

    //set linetype in layer
    for (std::map<int, DRW_Layer*>::iterator it=layermap.begin(); it!=layermap.end(); ++it) {
        DRW_Layer *ly = it->second;
        duint32 ref =ly->lTypeH.ref;
        std::map<int, DRW_LType*>::iterator lt_it = ltypemap.find(ref);
        if (lt_it != ltypemap.end()){
            ly->lineType = (lt_it->second)->name;
        }
    }

    //parse blocks records
    std::map<int, DRW_Block*> tmpBlockmap;
    for (std::list<objHandle>::iterator it=BlockRecordMap.begin(); it != BlockRecordMap.end(); ++it){
        DBG("BlockMap map Handle= "); DBG(it->handle); DBG(" "); DBG(it->loc); DBG("\n");
        buf->setPosition(it->loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        switch (it->type){
        case 4: {
            DRW_Block *e= new DRW_Block();
            ret2 = e->parseDwg(version, &buff);
            parseAttribs(e);
            tmpBlockmap[e->handle] = e;
            break; }
        case 5: {
            DRW_Block e;
            e.isEnd = true;
            ret2 = e.parseDwg(version, &buff);
            parseAttribs(&e);
            break; }
        case 49: {
            DRW_Block_Record *br = new DRW_Block_Record();
            ret2 = br->parseDwg(version, &buff);
            block_recmap[br->handle] = br;
            break; }
        default:
            break;
        }
        if(ret)
            ret = ret2;
    }

    //complete block entity with block record data
    for (std::map<int, DRW_Block*>::iterator it=tmpBlockmap.begin(); it!=tmpBlockmap.end(); ++it) {
        DRW_Block* bk = it->second;
        std::map<int, DRW_Block_Record*>::iterator brit = block_recmap.find(bk->handleBlock);
        if (brit == block_recmap.end()){//fail, set error
            if(ret)
                ret = ret2;
        } else {
            DRW_Block_Record *bkR = brit->second;
            parseAttribs(bk);
            bk->basePoint = bkR->basePoint;
            bk->flags = bkR->flags;
            if(bk->handleBlock == 0 && bk->name[0]=='*'){//verify model/paper space
                if (bk->name[1]=='P')
                    bk->handleBlock = bkR->handle;
            }
            blockmap[bk->handleBlock] = bk;
        }

    }

    return ret;
}

/**
 * Reads a dwg drawing entity (dwg object entity) given its offset in the file
 */
bool dwgReader18::readDwgEntity(objHandle& obj, DRW_Interface& intfa){
    bool ret = true;

#define ENTRY_PARSE(e) \
            ret = e.parseDwg(version, &buff); \
            if (e.handleBlock != currBlock) { \
                currBlock = e.handleBlock; \
                intfa.setBlock(e.handleBlock); \
            } \
            parseAttribs(&e)

        buf->setPosition(obj.loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        if (obj.type > 499){
            std::map<int, DRW_Class*>::iterator it = classesmap.find(obj.type);
            if (it == classesmap.end()){//fail, not found in classes set error
                return false;
            } else {
                DRW_Class *cl = it->second;
                if (cl->dwgType != 0)
                    obj.type = cl->dwgType;
            }
        }

        switch (obj.type){
        case 17: {
            DRW_Arc e;
            ENTRY_PARSE(e);
            intfa.addArc(e);
            break; }
        case 18: {
            DRW_Circle e;
            ENTRY_PARSE(e);
            intfa.addCircle(e);
            break; }
        case 19:{
            DRW_Line e;
            ENTRY_PARSE(e);
            intfa.addLine(e);
            break;}
        case 27: {
            DRW_Point e;
            ENTRY_PARSE(e);
            intfa.addPoint(e);
            break; }
        case 35: {
            DRW_Ellipse e;
            ENTRY_PARSE(e);
            intfa.addEllipse(e);
            break; }
        case 7: {//minsert = 8
            DRW_Insert e;
            ENTRY_PARSE(e);
            e.name = findTableName(DRW::BLOCK_RECORD, e.blockRecH.ref);
            intfa.addInsert(e);
            break; }
        case 77: {
            DRW_LWPolyline e;
            ENTRY_PARSE(e);
            intfa.addLWPolyline(e);
            break; }
        case 1: {
            DRW_Text e;
            ENTRY_PARSE(e);
            e.style = findTableName(DRW::STYLE, e.styleH.ref);
            intfa.addText(e);
            break; }
        case 44: {
            DRW_MText e;
            ENTRY_PARSE(e);
            e.style = findTableName(DRW::STYLE, e.styleH.ref);
            intfa.addMText(e);
            break; }
        case 28: {
            DRW_3Dface e;
            ENTRY_PARSE(e);
            intfa.add3dFace(e);
            break; }
        case 31: {
            DRW_Solid e;
            ENTRY_PARSE(e);
            intfa.addSolid(e);
            break; }

        default:
            break;
        }

    return ret;
}

