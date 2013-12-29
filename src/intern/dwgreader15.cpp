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
#include "dwgreader15.h"
#include "drw_textcodec.h"
#include "../libdwgr.h"


bool dwgReader15::readFileHeader() {
    version = parent->getVersion();
    decoder.setVersion(version);
    DBG("dwgReader15::readFileHeader\n");
    if (! buf->setPosition(13))
        return false;
    seekerImageData = buf->getRawLong32();
    DBG("seekerImageData= "); DBG(seekerImageData); DBG("\n");
    /* UNKNOUWN SECTION 2 bytes*/
    duint16 uk =    buf->getRawShort16();
    DBG("UNKNOUWN SECTION= "); DBG(uk); DBG("\n");
    duint16 cp = buf->getRawShort16();
    DBG("codepage= "); DBG(cp); DBG("\n");
    if (cp == 30)
        decoder.setCodePage("ANSI_1252");
    duint32 count = buf->getRawLong32();
    DBG("count records= ");
    DBG(count);
    DBG("\n");
    for (unsigned int i = 0; i < count; i++) {
        duint8 rec = buf->getRawChar8();
        duint32 seek = buf->getRawLong32();
        duint32 size = buf->getRawLong32();
        if (rec == 0) {
            DBG("Section HEADERS seek= ");
            DBG(seek); DBG(" size= "); DBG(size);  DBG("\n");
            sections["HEADERS"] = std::pair<duint32,duint32>(seek,size);
        } else if (rec == 1) {
            DBG("Section CLASSES seek= ");
            DBG(seek); DBG(" size= "); DBG(size);  DBG("\n");
            sections["CLASSES"] = std::pair<duint32,duint32>(seek,size);
        } else if (rec == 2) {
            DBG("Section OBJECTS seek= ");
            DBG(seek); DBG(" size= "); DBG(size);  DBG("\n");
            sections["OBJECTS"] = std::pair<duint32,duint32>(seek,size);
        } else if (rec == 3) {
            DBG("Section UNKNOWN seek= ");
            DBG(seek); DBG(" size= "); DBG(size);  DBG("\n");
            sections["UNKNOWN"] = std::pair<dint32,dint32>(seek,size);
        } else if (rec == 4) {
            DBG("Section R14DATA seek= ");
            DBG(seek); DBG(" size= "); DBG(size);  DBG("\n");
            sections["R14DATA"] = std::pair<dint32,dint32>(seek,size);
        } else if (rec == 5) {
            DBG("Section R14REC5 seek= ");
            DBG(seek); DBG(" size= "); DBG(size);  DBG("\n");
            sections["R14REC5"] = std::pair<dint32,dint32>(seek,size);
        } else {
            std::cerr << "Unsupported section number\n";
        }
    }
    if (! buf->isGood())
        return false;
    DBG("position after read section locator records= "); DBG(buf->getPosition());
    DBG(", bit are= "); DBG(buf->getBitPos()); DBG("\n");
    duint32 ckcrc = buf->crc8(0,0,buf->getPosition());
    DBG("file header crc8 0 result= "); DBG(ckcrc); DBG("\n");
    switch (count){
    case 3:
        ckcrc = ckcrc ^ 0xA598;
        break;
    case 4:
        ckcrc = ckcrc ^ 0x8101;
        break;
    case 5:
        ckcrc = ckcrc ^ 0x3CC4;
        break;
    case 6:
        ckcrc = ckcrc ^ 0x8461;
    }
    DBG("file header crc8 xor result= "); DBG(ckcrc); DBG("\n");
    DBG("file header CRC= "); DBG(buf->getRawShort16()); DBG("\n");
    DBG("file header sentinel= ");
    for (int i=0; i<16;i++) {
        DRW_DBGH(buf->getRawChar8()); DBG(" ");
    }

    DBG("\nposition after read file header sentinel= "); DBG(buf->getPosition());
    DBG(", bit are= "); DBG(buf->getBitPos()); DBG("\n");

    DBG("dwgReader15::readFileHeader END\n");
    return true;
}

bool dwgReader15::readDwgClasses(){
    DBG("dwgReader15::readDwgClasses");
    dint32 offset = sections["CLASSES"].first;
    dint32 secSize = sections["CLASSES"].second;
//    dint32 maxPos = offset + sections["CLASSES"].second;
    //secSize = 16(sentinel) + 4(size) + data + 2 (crc) + 16(sentinel)
//    dint32 maxPos = offset + secSize;
    if (!buf->setPosition(offset))
        return false;
    DBG("file header sentinel= ");

    for (int i=0; i<16;i++) {
        DRW_DBGH(buf->getRawChar8()); DBG(" ");
    }

    dint32 size = buf->getRawLong32();
    if (size != (secSize - 38)) {
        DBG("WARNING dwgReader15::readDwgClasses size are "); DBG(size);
        DBG(" and secSize - 38 are "); DBG(secSize - 38); DBG("\n");
    }
    char byteStr[size];
    buf->getBytes(byteStr, size);
    dwgBuffer buff(byteStr, size, &decoder);
    while (size > buff.getPosition()) {
        DRW_Class *cl = new DRW_Class();
        cl->parseDwg(version, &buff);
        classesmap[cl->classNum] = cl;
    }
    return buff.isGood();

    //verify crc
//    duint16 crcCalc = buff.crc8(0xc0c1,0,size);
//    duint16 crcRead = buf->getBERawShort16();
//    DBG("object map section crc8 read= "); DBG(crcRead); DBG("\n");
//    DBG("object map section crc8 calculated= "); DBG(crcCalc); DBG("\n");
}

/*********** objects map ************************/
/** Note: object map are split in sections with max size 2035?
 *  heach section are 2 bytes size + data bytes + 2 bytes crc
 *  size value are data bytes + 2 and to calculate crc are used
 *  2 bytes size + data bytes
 *  last section are 2 bytes size + 2 bytes crc (size value always 2)
**/
bool dwgReader15::readDwgObjectOffsets() {
    DBG("dwgReader15::readDwgObjectOffsets\n");
    dint32 offset = sections["OBJECTS"].first;
    dint32 maxPos = offset + sections["OBJECTS"].second;
    DBG("Section OBJECTS offset= "); DBG(offset); DBG("\n");
    DBG("Section OBJECTS size= "); DBG(sections["OBJECTS"].second); DBG("\n");
    DBG("Section OBJECTS maxPos= "); DBG(maxPos); DBG("\n");
    if (!buf->setPosition(offset))
        return false;
    DBG("Section OBJECTS buf->curPosition()= "); DBG(buf->getPosition()); DBG("\n");

    int startPos = offset;

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
                lastHandle += buff.getUModularChar();
//                lastHandle += buff.getModularChar();
                DBG("object map lastHandle= "); DRW_DBGH(lastHandle); DBG("\n");
                lastLoc += buff.getModularChar();
                DBG("object map lastLoc= "); DBG(lastLoc); DBG("\n");
                ObjectMap[lastHandle]= objHandle(0, lastHandle, lastLoc);
//                ObjectMap.insert( std::pair<duint32, objHandle>(lastHandle, objHandle(0, lastHandle, lastLoc)) );
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
    int controlCount=0;
    DBG("readDwgObjects() Total objects: "); DBG(ObjectMap.size()); DBG("\n");
    for (std::map<duint32, objHandle>::iterator it=ObjectMap.begin(); it != ObjectMap.end(); ++it){
        DBG("object map Handle= "); DRW_DBGH(it->second.handle); DBG(" "); DBG(it->second.loc); DBG("\n");
        ret = buf->setPosition(it->second.loc); //loc == offset
        if (!ret){
            ret = false;
            continue;
        }
        int size = buf->getModularShort();
        DBG("readDwgObject size: "); DBG(size); DBG("\n");
        it->second.type = buf->getBitShort();
        DBG("readDwgObject object type: "); DBG(it->second.type); DBG("\n");
        //annotate control objects
        //for speed, normal position is first objects do not exec switch if all are readed
        if (controlCount<10){
            switch (it->second.type){
            case 0x30:
                controlCount++;
                blockCtrl = it->second.handle;
                break;
            case 0x32:
                controlCount++;
                layerCtrl = it->second.handle;
                break;
            case 0x34:
                controlCount++;
                styleCtrl = it->second.handle;
                break;
            case 0x38:
                controlCount++;
                linetypeCtrl = it->second.handle;
                break;
            case 0x3C:
                controlCount++;
                viewCtrl = it->second.handle;
                break;
            case 0x3E:
                controlCount++;
                ucsCtrl = it->second.handle;
                break;
            case 0x40:
                controlCount++;
                vportCtrl = it->second.handle;
                break;
            case 0x42:
                controlCount++;
                appidCtrl = it->second.handle;
                break;
            case 0x44:
                controlCount++;
                dimstyleCtrl = it->second.handle;
                break;
            case 0x46:
                controlCount++;
                vpEntHeaderCtrl = it->second.handle;
                break;
            default:
                break;
            }
        }

    }

    return ret;
}

/*********** objects ************************/
/**
 * Reads all the object referenced in the object map section of the DWG file
 * (using their object file offsets)
 */
bool dwgReader15::readDwgTables() {
    bool ret = true;
    bool ret2 = true;

    //parse linetypes, start with linetype Control
    objHandle oc = ObjectMap[linetypeCtrl];
    ObjectMap.erase(linetypeCtrl);
    if (oc.type == 0) {
        DBG("\nWARNING: LineType control not found\n");
        ret = false;
    } else {
        DBG("\n**********Parsing LineType control*******\n");
        DRW_ObjControl ltControl;
        buf->setPosition(oc.loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = ltControl.parseDwg(version, &buff);
        if(ret)
            ret = ret2;

        for (std::list<duint32>::iterator it=ltControl.hadlesList.begin(); it != ltControl.hadlesList.end(); ++it){
            oc = ObjectMap[*it];
            ObjectMap.erase(oc.handle);
            if (oc.type == 0) {
                DBG("\nWARNING: LineType not found\n");
                ret = false;
            } else {
                DBG("LineTypeMap map Handle= "); DRW_DBGH(oc.handle); DBG(" "); DBG(oc.loc); DBG("\n");
                DRW_LType *lt = new DRW_LType();
                buf->setPosition(oc.loc);
                int size = buf->getModularShort();
                char byteStr[size];
                buf->getBytes(byteStr, size);
                dwgBuffer buff(byteStr, size, &decoder);
                ret2 = lt->parseDwg(version, &buff);
                ltypemap[lt->handle] = lt;
                if(ret)
                    ret = ret2;
            }
        }
    }

    //parse layers, start with layer Control
    oc = ObjectMap[layerCtrl];
    ObjectMap.erase(layerCtrl);
    if (oc.type == 0) {
        DBG("\nWARNING: Layer control not found\n");
        ret = false;
    } else {
        DBG("\n**********Parsing Layer control*******\n");
        DRW_ObjControl layControl;
        buf->setPosition(oc.loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = layControl.parseDwg(version, &buff);
        if(ret)
            ret = ret2;

        for (std::list<duint32>::iterator it=layControl.hadlesList.begin(); it != layControl.hadlesList.end(); ++it){
            oc = ObjectMap[*it];
            ObjectMap.erase(oc.handle);
            if (oc.type == 0) {
                DBG("\nWARNING: Layer not found\n");
                ret = false;
            } else {
                DBG("LayerMap map Handle= "); DRW_DBGH(oc.handle); DBG(" "); DBG(oc.loc); DBG("\n");
                DRW_Layer *la = new DRW_Layer();
                buf->setPosition(oc.loc);
                int size = buf->getModularShort();
                char byteStr[size];
                buf->getBytes(byteStr, size);
                dwgBuffer buff(byteStr, size, &decoder);
                ret2 = la->parseDwg(version, &buff);
                layermap[la->handle] = la;
                if(ret)
                    ret = ret2;
            }
        }
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

    //parse text styles, start with style Control
    oc = ObjectMap[styleCtrl];
    ObjectMap.erase(styleCtrl);
    if (oc.type == 0) {
        DBG("\nWARNING: Style control not found\n");
        ret = false;
    } else {
        DBG("\n**********Parsing Style control*******\n");
        DRW_ObjControl styControl;
        buf->setPosition(oc.loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = styControl.parseDwg(version, &buff);
        if(ret)
            ret = ret2;

        for (std::list<duint32>::iterator it=styControl.hadlesList.begin(); it != styControl.hadlesList.end(); ++it){
            oc = ObjectMap[*it];
            ObjectMap.erase(oc.handle);
            if (oc.type == 0) {
                DRW_DBG("\nWARNING: Style not found\n");
                ret = false;
            } else {
                DBG("StyleMap map Handle= "); DRW_DBGH(oc.handle); DBG(" "); DBG(oc.loc); DBG("\n");
                DRW_Textstyle *sty = new DRW_Textstyle();
                buf->setPosition(oc.loc);
                int size = buf->getModularShort();
                char byteStr[size];
                buf->getBytes(byteStr, size);
                dwgBuffer buff(byteStr, size, &decoder);
                ret2 = sty->parseDwg(version, &buff);
                stylemap[sty->handle] = sty;
                if(ret)
                    ret = ret2;
            }
        }
    }

    //parse dim styles, start with dimstyle Control
    oc = ObjectMap[dimstyleCtrl];
    ObjectMap.erase(dimstyleCtrl);
    if (oc.type == 0) {
        DBG("\nWARNING: Dimension Style control not found\n");
        ret = false;
    } else {
        DBG("\n**********Parsing Dimension Style control*******\n");
        DRW_ObjControl dimstyControl;
        buf->setPosition(oc.loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = dimstyControl.parseDwg(version, &buff);
        if(ret)
            ret = ret2;

        for (std::list<duint32>::iterator it=dimstyControl.hadlesList.begin(); it != dimstyControl.hadlesList.end(); ++it){
            oc = ObjectMap[*it];
            ObjectMap.erase(oc.handle);
            if (oc.type == 0) {
                DRW_DBG("\nWARNING: Dimension Style not found\n");
                ret = false;
            } else {
                DBG("DimstyleMap map Handle= "); DRW_DBGH(oc.handle); DBG(" "); DBG(oc.loc); DBG("\n");
                DRW_Dimstyle *sty = new DRW_Dimstyle();
                buf->setPosition(oc.loc);
                int size = buf->getModularShort();
                char byteStr[size];
                buf->getBytes(byteStr, size);
                dwgBuffer buff(byteStr, size, &decoder);
                ret2 = sty->parseDwg(version, &buff);
                dimstylemap[sty->handle] = sty;
                if(ret)
                    ret = ret2;
            }
        }
    }

    //parse vports, start with vports Control
    oc = ObjectMap[vportCtrl];
    ObjectMap.erase(vportCtrl);
    if (oc.type == 0) {
        DBG("\nWARNING: vports control not found\n");
        ret = false;
    } else {
        DBG("\n**********Parsing vports control*******\n");
        DRW_ObjControl vportControl;
        buf->setPosition(oc.loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = vportControl.parseDwg(version, &buff);
        if(ret)
            ret = ret2;

        for (std::list<duint32>::iterator it=vportControl.hadlesList.begin(); it != vportControl.hadlesList.end(); ++it){
            oc = ObjectMap[*it];
            ObjectMap.erase(oc.handle);
            if (oc.type == 0) {
                DRW_DBG("\nWARNING: vport not found\n");
                ret = false;
            } else {
                DBG("VportMap map Handle= "); DRW_DBGH(oc.handle); DBG(" "); DBG(oc.loc); DBG("\n");
                DRW_Vport *vp = new DRW_Vport();
                buf->setPosition(oc.loc);
                int size = buf->getModularShort();
                char byteStr[size];
                buf->getBytes(byteStr, size);
                dwgBuffer buff(byteStr, size, &decoder);
                ret2 = vp->parseDwg(version, &buff);
                vportmap[vp->handle] = vp;
                if(ret)
                    ret = ret2;
            }
        }
    }

    //parse remaining object controls, TODO: implement all
    if (DRW_DBGGL == DRW_dbg::DEBUG){
        oc = ObjectMap[viewCtrl];
        ObjectMap.erase(viewCtrl);
        if (oc.type == 0) {
            DBG("\nWARNING: View control not found\n");
            ret = false;
        } else {
            DBG("\n**********Parsing View control*******\n");
            DBG("View Control Obj Handle= "); DRW_DBGH(oc.handle); DRW_DBG(" "); DRW_DBG(oc.loc); DRW_DBG("\n");
            DRW_ObjControl viewControl;
            buf->setPosition(oc.loc);
            int size = buf->getModularShort();
            char byteStr[size];
            buf->getBytes(byteStr, size);
            dwgBuffer buff(byteStr, size, &decoder);
            ret2 = viewControl.parseDwg(version, &buff);
            if(ret)
                ret = ret2;
        }
        oc = ObjectMap[ucsCtrl];
        ObjectMap.erase(ucsCtrl);
        if (oc.type == 0) {
            DBG("\nWARNING: Ucs control not found\n");
            ret = false;
        } else {
            DBG("\n**********Parsing Ucs control*******\n");
            DBG("Ucs Control Obj Handle= "); DRW_DBGH(oc.handle); DRW_DBG(" "); DRW_DBG(oc.loc); DRW_DBG("\n");
            DRW_ObjControl ucsControl;
            buf->setPosition(oc.loc);
            int size = buf->getModularShort();
            char byteStr[size];
            buf->getBytes(byteStr, size);
            dwgBuffer buff(byteStr, size, &decoder);
            ret2 = ucsControl.parseDwg(version, &buff);
            if(ret)
                ret = ret2;
        }
        oc = ObjectMap[appidCtrl];
        ObjectMap.erase(appidCtrl);
        if (oc.type == 0) {
            DBG("\nWARNING: AppId control not found\n");
            ret = false;
        } else {
            DBG("\n**********Parsing AppId control*******\n");
            DBG("AppId Control Obj Handle= "); DRW_DBGH(oc.handle); DRW_DBG(" "); DRW_DBG(oc.loc); DRW_DBG("\n");
            DRW_ObjControl appIdControl;
            buf->setPosition(oc.loc);
            int size = buf->getModularShort();
            char byteStr[size];
            buf->getBytes(byteStr, size);
            dwgBuffer buff(byteStr, size, &decoder);
            ret2 = appIdControl.parseDwg(version, &buff);
            if(ret)
                ret = ret2;
        }
        oc = ObjectMap[vpEntHeaderCtrl];
        ObjectMap.erase(vpEntHeaderCtrl);
        if (oc.type == 0) {
            DBG("\nWARNING: vpEntHeader control not found\n");
            ret = false;
        } else {
            DBG("\n**********Parsing vpEntHeader control*******\n");
            DBG("vpEntHeader Control Obj Handle= "); DRW_DBGH(oc.handle); DRW_DBG(" "); DRW_DBG(oc.loc); DRW_DBG("\n");
            DRW_ObjControl ucsControl;
            buf->setPosition(oc.loc);
            int size = buf->getModularShort();
            char byteStr[size];
            buf->getBytes(byteStr, size);
            dwgBuffer buff(byteStr, size, &decoder);
            ret2 = ucsControl.parseDwg(version, &buff);
            if(ret)
                ret = ret2;
        }
    }

    return ret;
}
/**
 * Reads all the object referenced in the object map section of the DWG file
 * (using their object file offsets)
 */
bool dwgReader15::readDwgBlocks(DRW_Interface& intfa) {
    bool ret = true;
    bool ret2 = true;
    objHandle oc = ObjectMap[blockCtrl];
    //parse block records, start with blocks Control
    oc = ObjectMap[blockCtrl];
    ObjectMap.erase(blockCtrl);
    if (oc.type == 0) {
        DBG("\nWARNING: blocks control not found\n");
        ret = false;
    } else {
        DBG("\n**********Parsing blocks control*******\n");
        DRW_ObjControl blockControl;
        buf->setPosition(oc.loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        ret2 = blockControl.parseDwg(version, &buff);
        if(ret)
            ret = ret2;

        for (std::list<duint32>::iterator it=blockControl.hadlesList.begin(); it != blockControl.hadlesList.end(); ++it){
            oc = ObjectMap[*it];
            ObjectMap.erase(oc.handle);
            if (oc.type == 0) {
                DRW_DBG("\nWARNING: block record not found\n");
                ret = false;
            } else {
                DBG("block record map Handle= "); DRW_DBGH(oc.handle); DBG(" "); DBG(oc.loc); DBG("\n");
                DRW_Block_Record *br = new DRW_Block_Record();
                buf->setPosition(oc.loc);
                int size = buf->getModularShort();
                char byteStr[size];
                buf->getBytes(byteStr, size);
                dwgBuffer buff(byteStr, size, &decoder);
                ret2 = br->parseDwg(version, &buff);
                block_recmap[br->handle] = br;
                if(ret)
                    ret = ret2;
            }
        }
    }

    //next parse blocks
    std::map<int, DRW_Block*> tmpBlockmap;//TODO: removeme ????
    for (std::map<int, DRW_Block_Record*>::iterator it=block_recmap.begin(); it != block_recmap.end(); ++it){
        DBG("\nParsing Block, record handle= "); DRW_DBGH(it->first); DBG(" Name= "); DRW_DBG(it->second->name); DBG("\n");
        DRW_Block_Record* bkr= it->second;
        oc = ObjectMap[bkr->handleBlock];
        ObjectMap.erase(oc.handle);
        if (oc.type == 0) {
            DBG("\nWARNING: block entity not found\n");
            ret = false;
            continue;
        }
        DBG("Block Handle= "); DRW_DBGH(oc.handle); DBG(" Location: "); DBG(oc.loc); DBG("\n");
        buf->setPosition(oc.loc);
        int size = buf->getModularShort();
        char byteStr[size];
        buf->getBytes(byteStr, size);
        dwgBuffer buff(byteStr, size, &decoder);
        DRW_Block *bk= new DRW_Block();
        ret2 = bk->parseDwg(version, &buff);
        parseAttribs(bk);
        //complete block entity with block record data
        bk->basePoint = bkr->basePoint;
        bk->flags = bkr->flags;
        duint32 nextH = bkr->firstEH;
        if(bk->handleBlock == 0 && bk->name[0]=='*'){
            if (bk->name[1]=='P')
                bk->handleBlock = bkr->handle;//TODO: ??? verify model/paper space
            if (bk->name[1]=='M')
                nextH = 0;//do not parse model space entities
        }
        intfa.addBlock(*bk);
        //Parses block entities
        while (nextH !=0){
            oc = ObjectMap[nextH];
            ObjectMap.erase(nextH);
            if (oc.type == 0) {
                nextH = 0;//end while if entity not foud
                DBG("\nWARNING: First block entity not found\n");
                ret = false;
                continue;
            }
            ret2 = readDwgEntity(oc, intfa);
            if (!ret2) ret = false;
            if (nextH == bkr->lastEH)
                nextH = 0; //redundant, but prevent read errors
            else
                nextH = nextEntLink;
        }

        tmpBlockmap[bk->handle] = bk;
        //end block entity, really needed to parse a dummy entity??
        oc = ObjectMap[it->second->endBlock];
        ObjectMap.erase(oc.handle);
        intfa.endBlock();
        if (oc.type == 0) {
            DBG("\nWARNING: block entity not found\n");
            ret = false;
            continue;
        }
        DBG("End block Handle= "); DRW_DBGH(oc.handle); DBG(" Location: "); DBG(oc.loc); DBG("\n");
        buf->setPosition(oc.loc);
        size = buf->getModularShort();
        char byteStr1[size];
        buf->getBytes(byteStr1, size);
        dwgBuffer buff1(byteStr1, size, &decoder);
        DRW_Block end;
        end.isEnd = true;
        ret2 = end.parseDwg(version, &buff1);
        parseAttribs(&end);
    }

    return ret;
}
/**
 * Reads a dwg drawing entity (dwg object entity) given its offset in the file
 */
bool dwgReader15::readDwgEntity(objHandle& obj, DRW_Interface& intfa){
    bool ret = true;

#define ENTRY_PARSE(e) \
            ret = e.parseDwg(version, &buff); \
            if (e.handleBlock != currBlock) { \
                currBlock = e.handleBlock; \
                intfa.setBlock(e.handleBlock); \
            } \
            parseAttribs(&e); \
    nextEntLink = e.nextEntLink; \
    prevEntLink = e.prevEntLink;

    nextEntLink = prevEntLink = 0;// set to 0 to skip unimplemented entities
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
            ENTRY_PARSE(e)
            intfa.addArc(e);
            break; }
        case 18: {
            DRW_Circle e;
            ENTRY_PARSE(e)
            intfa.addCircle(e);
            break; }
        case 19:{
            DRW_Line e;
            ENTRY_PARSE(e)
            intfa.addLine(e);
            break;}
        case 27: {
            DRW_Point e;
            ENTRY_PARSE(e)
            intfa.addPoint(e);
            break; }
        case 35: {
            DRW_Ellipse e;
            ENTRY_PARSE(e)
            intfa.addEllipse(e);
            break; }
        case 7: {//minsert = 8
            DRW_Insert e;
            ENTRY_PARSE(e)
            e.name = findTableName(DRW::BLOCK_RECORD, e.blockRecH.ref);
            intfa.addInsert(e);
            break; }
        case 77: {
            DRW_LWPolyline e;
            ENTRY_PARSE(e)
            intfa.addLWPolyline(e);
            break; }
        case 1: {
            DRW_Text e;
            ENTRY_PARSE(e)
            e.style = findTableName(DRW::STYLE, e.styleH.ref);
            intfa.addText(e);
            break; }
        case 44: {
            DRW_MText e;
            ENTRY_PARSE(e)
            e.style = findTableName(DRW::STYLE, e.styleH.ref);
            intfa.addMText(e);
            break; }
        case 28: {
            DRW_3Dface e;
            ENTRY_PARSE(e)
            intfa.add3dFace(e);
            break; }
        case 31: {
            DRW_Solid e;
            ENTRY_PARSE(e)
            intfa.addSolid(e);
            break; }
        case 34: {
            DRW_Viewport e;
            ENTRY_PARSE(e)
            intfa.addViewport(e);
            break; }

        default:
            break;
        }

    return ret;
}
//        } else if (it->type == 0x31 || it->type == 0x4 || it->type == 0x5){


///////////////////////////////////////////////////////////////////////

/*bool dwgReader15::readDwgHeader() {
return false;
}*/

/*bool dwgReader15::readDwgClasses() {
return false;
}*/
