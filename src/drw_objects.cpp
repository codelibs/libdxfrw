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

#include <iostream>
#include <math.h>
#include "drw_objects.h"
#include "intern/dxfreader.h"
#include "intern/dxfwriter.h"
#include "intern/dwgbuffer.h"
#include "intern/drw_dbg.h"

//! Base class for tables entries
/*!
*  Base class for tables entries
*  @author Rallaz
*/
void DRW_TableEntry::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 5:
        handle = reader->getHandleString();
        break;
    case 330:
        handleBlock = reader->getHandleString();
        break;
    case 2:
        name = reader->getUtf8String();
        break;
    case 70:
        flags = reader->getInt32();
        break;
    case 1000:
    case 1001:
    case 1002:
    case 1003:
    case 1004:
    case 1005:
        extData.push_back(new DRW_Variant(code, reader->getString() ));
        break;
    case 1010:
    case 1011:
    case 1012:
    case 1013:
        curr = new DRW_Variant();
        curr->addCoord();
        curr->setCoordX(reader->getDouble());
        curr->code = code;
        extData.push_back(curr);
        break;
    case 1020:
    case 1021:
    case 1022:
    case 1023:
        if (curr)
            curr->setCoordY(reader->getDouble());
        break;
    case 1030:
    case 1031:
    case 1032:
    case 1033:
        if (curr)
            curr->setCoordZ(reader->getDouble());
        curr=NULL;
        break;
    case 1040:
    case 1041:
    case 1042:
        extData.push_back(new DRW_Variant(code, reader->getDouble() ));
        break;
    case 1070:
    case 1071:
        extData.push_back(new DRW_Variant(code, reader->getInt32() ));
        break;
    default:
        break;
    }
}

bool DRW_TableEntry::parseDwg(DRW::Version version, dwgBuffer *buf){
    duint32 objSize;
//    duint8 ltFlags; //BB
//    duint8 plotFlags; //BB
DBG("\n***************************** parsing table entry *********************************************\n");
    oType = buf->getBitShort();
    if (version > DRW::AC1014) {//2000+
        objSize = buf->getRawLong32();  //RL 32bits size in bits
    }
    dwgHandle ho = buf->getHandle();
    handle = ho.ref;
    DBG("TableEntry Handle: "); DBG(ho.code); DBG(".");
    DBG(ho.size); DBG("."); DRW_DBGH(ho.ref);
    dint16 extDataSize = buf->getBitShort(); //BS
    DBG(" ext data size: "); DBG(extDataSize);
    while (extDataSize>0 && buf->isGood()) {
        /* RLZ: TODO */
        dwgHandle ah = buf->getHandle();
        DBG("App Handle: "); DBG(ah.code); DBG("."); DBG(ah.size); DBG("."); DBG(ah.ref);
        char byteStr[extDataSize];
        buf->getBytes(byteStr, extDataSize);
        dwgBuffer buff(byteStr, extDataSize, buf->decoder);
        int pos = buff.getPosition();
        int bpos = buff.getBitPos();
        DBG("ext data pos: "); DBG(pos); DBG("."); DBG(bpos); DBG("\n");
        duint8 dxfCode = buff.getRawChar8();
        DBG(" dxfCode: "); DBG(dxfCode);
        switch (dxfCode){
        case 0:{
            duint8 strLength = buff.getRawChar8();
            DBG(" strLength: "); DBG(strLength);
            duint16 cp = buff.getBERawShort16();
            DBG(" str codepage: "); DBG(cp);
            for (int i=0;i< strLength+1;i++) {//string length + null terminating char
                duint8 dxfChar = buff.getRawChar8();
                DBG(" dxfChar: "); DBG(dxfChar);
            }
            break;
        }
        default:
            /* RLZ: TODO */
            break;
        }
        DBG("ext data pos: "); DBG(buff.getPosition()); DBG("."); DBG(buff.getBitPos()); DBG("\n");
        extDataSize = buf->getBitShort(); //BS
        DBG(" ext data size: "); DBG(extDataSize);
    } //end parsing extData (EED)
    if (version < DRW::AC1015) {//14-
        objSize = buf->getRawLong32();  //RL 32bits size in bits
    }
    DBG(" objSize in bits: "); DBG(objSize);

    numReactors = buf->getBitLong(); //BL
    DBG(", numReactors: "); DBG(numReactors); DBG("\n");
    if (version > DRW::AC1015) {//2004+
        /*duint8 xDictFlag =*/ buf->getBit();
    }
    return buf->isGood();
}

//! Class to handle dimstyle entries
/*!
*  Class to handle ldim style symbol table entries
*  @author Rallaz
*/
void DRW_Dimstyle::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 105:
        handle = reader->getHandleString();
        break;
    case 3:
        dimpost = reader->getUtf8String();
        break;
    case 4:
        dimapost = reader->getUtf8String();
        break;
    case 5:
        dimblk = reader->getUtf8String();
        break;
    case 6:
        dimblk1 = reader->getUtf8String();
        break;
    case 7:
        dimblk2 = reader->getUtf8String();
        break;
    case 40:
        dimscale = reader->getDouble();
        break;
    case 41:
        dimasz = reader->getDouble();
        break;
    case 42:
        dimexo = reader->getDouble();
        break;
    case 43:
        dimdli = reader->getDouble();
        break;
    case 44:
        dimexe = reader->getDouble();
        break;
    case 45:
        dimrnd = reader->getDouble();
        break;
    case 46:
        dimdle = reader->getDouble();
        break;
    case 47:
        dimtp = reader->getDouble();
        break;
    case 48:
        dimtm = reader->getDouble();
        break;
    case 140:
        dimtxt = reader->getDouble();
        break;
    case 141:
        dimcen = reader->getDouble();
        break;
    case 142:
        dimtsz = reader->getDouble();
        break;
    case 143:
        dimaltf = reader->getDouble();
        break;
    case 144:
        dimlfac = reader->getDouble();
        break;
    case 145:
        dimtvp = reader->getDouble();
        break;
    case 146:
        dimtfac = reader->getDouble();
        break;
    case 147:
        dimgap = reader->getDouble();
        break;
    case 148:
        dimaltrnd = reader->getDouble();
        break;
    case 71:
        dimtol = reader->getInt32();
        break;
    case 72:
        dimlim = reader->getInt32();
        break;
    case 73:
        dimtih = reader->getInt32();
        break;
    case 74:
        dimtoh = reader->getInt32();
        break;
    case 75:
        dimse1 = reader->getInt32();
        break;
    case 76:
        dimse2 = reader->getInt32();
        break;
    case 77:
        dimtad = reader->getInt32();
        break;
    case 78:
        dimzin = reader->getInt32();
        break;
    case 79:
        dimazin = reader->getInt32();
        break;
    case 170:
        dimalt = reader->getInt32();
        break;
    case 171:
        dimaltd = reader->getInt32();
        break;
    case 172:
        dimtofl = reader->getInt32();
        break;
    case 173:
        dimsah = reader->getInt32();
        break;
    case 174:
        dimtix = reader->getInt32();
        break;
    case 175:
        dimsoxd = reader->getInt32();
        break;
    case 176:
        dimclrd = reader->getInt32();
        break;
    case 177:
        dimclre = reader->getInt32();
        break;
    case 178:
        dimclrt = reader->getInt32();
        break;
    case 179:
        dimadec = reader->getInt32();
        break;
    case 270:
        dimunit = reader->getInt32();
        break;
    case 271:
        dimdec = reader->getInt32();
        break;
    case 272:
        dimtdec = reader->getInt32();
        break;
    case 273:
        dimaltu = reader->getInt32();
        break;
    case 274:
        dimalttd = reader->getInt32();
        break;
    case 275:
        dimaunit = reader->getInt32();
        break;
    case 276:
        dimfrac = reader->getInt32();
        break;
    case 277:
        dimlunit = reader->getInt32();
        break;
    case 278:
        dimdsep = reader->getInt32();
        break;
    case 279:
        dimtmove = reader->getInt32();
        break;
    case 280:
        dimjust = reader->getInt32();
        break;
    case 281:
        dimsd1 = reader->getInt32();
        break;
    case 282:
        dimsd2 = reader->getInt32();
        break;
    case 283:
        dimtolj = reader->getInt32();
        break;
    case 284:
        dimtzin = reader->getInt32();
        break;
    case 285:
        dimaltz = reader->getInt32();
        break;
    case 286:
        dimaltttz = reader->getInt32();
        break;
    case 287:
        dimfit = reader->getInt32();
        break;
    case 288:
        dimupt = reader->getInt32();
        break;
    case 289:
        dimatfit = reader->getInt32();
        break;
    case 340:
        dimtxsty = reader->getUtf8String();
        break;
    case 341:
        dimldrblk = reader->getUtf8String();
        break;
    case 342:
        dimblk = reader->getUtf8String();
        break;
    case 343:
        dimblk1 = reader->getUtf8String();
        break;
    case 344:
        dimblk2 = reader->getUtf8String();
        break;
    default:
        DRW_TableEntry::parseCode(code, reader);
        break;
    }
}

bool DRW_Dimstyle::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_TableEntry::parseDwg(version, buf);
    DBG("\n***************************** parsing dimension style **************************************\n");
    if (!ret)
        return ret;
    if (version > DRW::AC1018) {//2007+
        name = buf->getVariableText();
    } else {//2004-
        name = buf->getVariableUtf8Text();
    }
    DBG("dimension style name: "); DBG(name.c_str()); DBG("\n");

    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
    //    RS crc;   //RS */
    return buf->isGood();
}


//! Class to handle line type entries
/*!
*  Class to handle line type symbol table entries
*  @author Rallaz
*/
void DRW_LType::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 3:
        desc = reader->getUtf8String();
        break;
    case 73:
        size = reader->getInt32();
        path.reserve(size);
        break;
    case 40:
        length = reader->getDouble();
        break;
    case 49:
        path.push_back(reader->getDouble());
        pathIdx++;
        break;
/*    case 74:
        haveShape = reader->getInt32();
        break;*/
    default:
        DRW_TableEntry::parseCode(code, reader);
        break;
    }
}

//! Update line type
/*!
*  Update the size and length of line type acording to the path
*  @author Rallaz
*/
/*TODO: control max length permited */
void DRW_LType::update(){
    double d =0;
    size = path.size();
    for (int i = 0;  i< size; i++){
        d += fabs(path.at(i));
    }
    length = d;
}

bool DRW_LType::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_TableEntry::parseDwg(version, buf);
    DBG("\n***************************** parsing line type *********************************************\n");
    if (!ret)
        return ret;
    if (version > DRW::AC1018) {//2007+
        name = buf->getVariableText();
    } else {//2004-
        name = buf->getVariableUtf8Text();
    }
    DBG("linetype name: "); DBG(name.c_str()); DBG("\n");
    flags = buf->getBit()<< 6;
    DBG("flags: "); DBG(flags);
    dint16 xrefindex = buf->getBitShort();
    DBG(" xrefindex: "); DBG(xrefindex);
    duint8 xdep = buf->getBit();
    DBG(" xdep: "); DBG(xdep);
    flags |= xdep<< 4;
    DBG(" flags: "); DBG(flags);
    desc = buf->getVariableUtf8Text();
    DBG(" desc: "); DBG(desc.c_str());
    length = buf->getBitDouble();
    DBG(" length: "); DBG(length);
    char align = buf->getRawChar8();
    DBG(" align: "); DBG(std::string(&align, 1));
    size = buf->getRawChar8();
    DBG(" num dashes, size: "); DBG(size);
    DBG("\n    dashes:\n");
    for (int i=0; i< size; i++){
        path.push_back(buf->getBitDouble());
        /*int bs1 =*/ buf->getBitShort();
        /*double d1= */buf->getRawDouble();
        /*double d2=*/ buf->getRawDouble();
        /*double d3= */buf->getBitDouble();
        /*double d4= */buf->getBitDouble();
        /*int bs2 = */buf->getBitShort();
    }
    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
    if (version < DRW::AC1021) { //2004-
        char strarea[256];
        buf->getBytes(strarea, 256);
        DBG("string area 256 bytes:\n"); DBG(strarea); DBG("\n");
    } else { //2007+
        char strarea[512];
        buf->getBytes(strarea, 512);
        DBG("string area 512 bytes:\n"); DBG(strarea); DBG("\n");
    }

//    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
    dwgHandle ltControlH = buf->getHandle();
    DBG("linetype control Handle: "); DBG(ltControlH.code); DBG(".");
    DBG(ltControlH.size); DBG("."); DBG(ltControlH.ref);
    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
    dwgHandle reactorsH = buf->getHandle();
    DBG(" reactorsH control Handle: "); DBG(reactorsH.code); DBG(".");
    DBG(reactorsH.size); DBG("."); DBG(reactorsH.ref); DBG("\n");
    dwgHandle XDicObjH = buf->getHandle();
    DBG(" XDicObj control Handle: "); DBG(XDicObjH.code); DBG(".");
    DBG(XDicObjH.size); DBG("."); DBG(XDicObjH.ref); DBG("\n");
    if(size>0){
        dwgHandle XRefH = buf->getHandle();
        DBG(" XRefH control Handle: "); DBG(XRefH.code); DBG(".");
        DBG(XRefH.size); DBG("."); DBG(XRefH.ref); DBG("\n");
        dwgHandle shpHandle = buf->getHandle();
        DBG(" shapeFile Handle: "); DBG(shpHandle.code); DBG(".");
        DBG(shpHandle.size); DBG("."); DBG(shpHandle.ref); DBG("\n");
    }

    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
//    RS crc;   //RS */
    return buf->isGood();
}

//! Class to handle layer entries
/*!
*  Class to handle layer symbol table entries
*  @author Rallaz
*/
void DRW_Layer::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 6:
        lineType = reader->getUtf8String();
        break;
    case 62:
        color = reader->getInt32();
        break;
    case 290:
        plotF = reader->getBool();
        break;
    case 370:
        lWeight = DRW_LW_Conv::dxfInt2lineWidth(reader->getInt32());
        break;
    case 390:
        handlePlotS = reader->getString();
        break;
    case 347:
        handlePlotM = reader->getString();
        break;
    case 420:
        color24 = reader->getInt32();
        break;
    default:
        DRW_TableEntry::parseCode(code, reader);
        break;
    }
}

bool DRW_Layer::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_TableEntry::parseDwg(version, buf);
    DBG("\n***************************** parsing layer *********************************************\n");
    if (!ret)
        return ret;
    if (version > DRW::AC1018) {//2007+
        name = buf->getVariableText();
    } else {//2004-
        name = buf->getVariableUtf8Text();
    }
    DBG("layer name: "); DBG(name.c_str());

    flags |= buf->getBit()<< 6;//layer have entity
    /*dint16 xrefindex =*/ buf->getBitShort();
    flags |= buf->getBit() << 4;//is refx dependent
    if (version < DRW::AC1015) {//14-
        flags |= buf->getBit(); //layer frozen
        /*flags |=*/ buf->getBit(); //unused, negate the color
        flags |= buf->getBit() << 1;//frozen in new
        flags |= buf->getBit()<< 3;//locked
    }
    if (version > DRW::AC1014) {//2000+
        dint16 f = buf->getBitShort();//bit2 are layer on
        DBG(", flags 2000+: "); DBG(f); DBG("\n");
        flags |= f & 0x0001; //layer frozen
        flags |= ( f>> 1) & 0x0002;//frozen in new
        flags |= ( f>> 1) & 0x0004;//locked
        plotF = ( f>> 4) & 0x0001;
        lWeight = DRW_LW_Conv::dwgInt2lineWidth( (f & 0x03E0) >> 5 );
    }
    color = buf->getBitShort(); //BS or CMC //ok for R14 or negate
    DBG(", entity color: "); DBG(color); DBG("\n");

    dwgHandle layerControlH = buf->getHandle();
    DBG("layer control Handle: "); DBG(layerControlH.code); DBG(".");
    DBG(layerControlH.size); DBG("."); DBG(layerControlH.ref);

    dwgHandle XDicObjH = buf->getHandle();
    DBG(" XDicObj control Handle: "); DBG(XDicObjH.code); DBG(".");
    DBG(XDicObjH.size); DBG("."); DBG(XDicObjH.ref); DBG("\n");
    dwgHandle XRefH = buf->getHandle();
    DBG(" XRefH control Handle: "); DBG(XRefH.code); DBG(".");
    DBG(XRefH.size); DBG("."); DBG(XRefH.ref); DBG("\n");
    if (version > DRW::AC1014) {//2000+
        dwgHandle plotStyH = buf->getHandle();
        DBG(" PLot style control Handle: "); DBG(plotStyH.code); DBG(".");
        DBG(plotStyH.size); DBG("."); DBG(plotStyH.ref); DBG("\n");
    }
    //lineType handle
    lTypeH = buf->getHandle();
    DBG("line type Handle: "); DBG(lTypeH.code); DBG(".");
    DBG(lTypeH.size); DBG("."); DBG(lTypeH.ref);
    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
//    RS crc;   //RS */
    return buf->isGood();
}

bool DRW_Block_Record::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_TableEntry::parseDwg(version, buf);
    DBG("\n***************************** parsing block record ******************************************\n");
    if (!ret)
        return ret;
    duint32 insertCount = 0;//only 2000+
    duint32 objectCount = 0; //only 2004+

    if (version > DRW::AC1018) {//2007+
        name = buf->getVariableText();
    } else {//2004-
        name = buf->getVariableUtf8Text();
    }
    DBG("block record name: "); DBG(name.c_str());DBG("\n");

    flags |= buf->getBit()<< 6;//referenced external reference, block code 70, bit 7 (64)
    /*dint16 xrefindex =*/ buf->getBitShort();
    flags |= buf->getBit() << 4;//is refx dependent, block code 70, bit 5 (16)
    flags |= buf->getBit(); //if is anonimous block (*U) block code 70, bit 1 (1)
    flags |= buf->getBit() << 1; //if block contains attdefs, block code 70, bit 2 (2)
    bool blockIsXref = buf->getBit(); //if is a Xref, block code 70, bit 3 (4)
    bool xrefOverlaid = buf->getBit(); //if is a overlaid Xref, block code 70, bit 4 (8)
    flags |= blockIsXref << 2; //if is a Xref, block code 70, bit 3 (4)
    flags |= xrefOverlaid << 3; //if is a overlaid Xref, block code 70, bit 4 (8)
    if (version > DRW::AC1014) {//2000+
        flags |= buf->getBit() << 5; //if is a loaded Xref, block code 70, bit 6 (32)
    }
    if (version > DRW::AC1015) {//2004+
        objectCount = buf->getBitLong(); //Number of objects owned by this
    }
    basePoint.x = buf->getBitDouble();
    basePoint.y = buf->getBitDouble();
    basePoint.z = buf->getBitDouble();
    DBG("insertion point X: "); DBG(basePoint.x); DBG(", Y: "); DBG(basePoint.y); DBG(", Z: "); DBG(basePoint.z); DBG("\n");
    if (version > DRW::AC1018) {//2007+
        UTF8STRING path = buf->getVariableText();
    } else {//2004-
        UTF8STRING path = buf->getVariableUtf8Text();
    }
    if (version > DRW::AC1014) {//2000+
        insertCount = 0;
        while (duint8 i = buf->getRawChar8() != 0)
            insertCount +=i;
        if (version > DRW::AC1018) {//2007+
            UTF8STRING bkdesc = buf->getVariableText();
        } else {//2004-
            UTF8STRING bkdesc = buf->getVariableUtf8Text();
        }
        duint32 prevData = buf->getBitLong();
        for (unsigned int i= 0; i < prevData; i++)
            buf->getRawChar8();
    }
    if (version > DRW::AC1018) {//2007+
        duint16 insUnits = buf->getBitShort();
        bool canExplode = buf->getBit(); //if block can be exploded
        duint8 bkScaling = buf->getRawChar8();

        DRW_UNUSED(insUnits);
        DRW_UNUSED(canExplode);
        DRW_UNUSED(bkScaling);
    }

    dwgHandle blockControlH = buf->getHandle();
    DRW_DBG("block control Handle: "); DRW_DBG(blockControlH.code); DRW_DBG(".");
    DRW_DBG(blockControlH.size); DRW_DBG("."); DRW_DBGH(blockControlH.ref);

    for (int i=0; i<numReactors; i++){
        dwgHandle reactorH = buf->getHandle();
        DRW_DBG(" reactor Handle #"); DRW_DBG(i); DBG(": "); DRW_DBG(reactorH.code); DRW_DBG(".");
        DRW_DBG(reactorH.size); DRW_DBG("."); DRW_DBGH(reactorH.ref); DRW_DBG("\n");
    }
    dwgHandle XDicObjH = buf->getHandle();
    DRW_DBG(" XDicObj control Handle: "); DRW_DBG(XDicObjH.code); DRW_DBG(".");
    DRW_DBG(XDicObjH.size); DBG("."); DRW_DBGH(XDicObjH.ref); DBG("\n");
    dwgHandle NullH = buf->getHandle();
    DRW_DBG(" NullH control Handle: "); DRW_DBG(NullH.code); DRW_DBG(".");
    DRW_DBG(NullH.size); DBG("."); DRW_DBGH(NullH.ref); DRW_DBG("\n");
    dwgHandle blockH = buf->getOffsetHandle(handle);
    DRW_DBG(" blockH Handle: "); DRW_DBG(blockH.code); DRW_DBG(".");
    DRW_DBG(blockH.size); DRW_DBG("."); DRW_DBGH(blockH.ref); DRW_DBG("\n");
    handleBlock = blockH.ref;

    if (version > DRW::AC1015) {//2004+
        for (unsigned int i=0; i< objectCount; i++){
            dwgHandle entityH = buf->getHandle();
            DRW_DBG(" entityH Handle #"); DRW_DBG(i); DRW_DBG(": "); DRW_DBG(entityH.code); DRW_DBG(".");
            DRW_DBG(entityH.size); DRW_DBG("."); DRW_DBGH(entityH.ref); DRW_DBG("\n");
        }
    } else {//2000-
        if(!blockIsXref && !xrefOverlaid){
            dwgHandle firstH = buf->getHandle();
            DRW_DBG(" firstH entity Handle: "); DRW_DBG(firstH.code); DBG(".");
            DRW_DBG(firstH.size); DRW_DBG("."); DRW_DBGH(firstH.ref); DBG("\n");
            firstEH = firstH.ref;
            dwgHandle lastH = buf->getHandle();
            DRW_DBG(" lastH entity Handle: "); DRW_DBG(lastH.code); DBG(".");
            DRW_DBG(lastH.size); DRW_DBG("."); DRW_DBGH(lastH.ref); DBG("\n");
            lastEH = lastH.ref;
        }
    }
    dwgHandle endBlockH = buf->getOffsetHandle(handle);
    DRW_DBG(" endBlockH Handle: "); DRW_DBG(endBlockH.code); DRW_DBG(".");
    DRW_DBG(endBlockH.size); DRW_DBG("."); DRW_DBGH(endBlockH.ref); DRW_DBG("\n");
    endBlock = endBlockH.ref;

    if (version > DRW::AC1014) {//2000+
        for (unsigned int i=0; i< insertCount; i++){
            dwgHandle insertsH = buf->getHandle();
            DRW_DBG(" insertsH Handle #"); DRW_DBG(i); DRW_DBG(": "); DRW_DBG(insertsH.code); DRW_DBG(".");
            DRW_DBG(insertsH.size); DBG("."); DRW_DBGH(insertsH.ref); DRW_DBG("\n");
        }
        dwgHandle layoutH = buf->getHandle();
        DRW_DBG(" layoutH Handle: "); DRW_DBG(layoutH.code); DRW_DBG(".");
        DRW_DBG(layoutH.size); DRW_DBG("."); DRW_DBGH(layoutH.ref); DRW_DBG("\n");
    }
    DRW_DBG("\n Remaining bytes: "); DRW_DBG(buf->numRemainingBytes()); DRW_DBG("\n");
//    RS crc;   //RS */
    return buf->isGood();
}

//! Class to handle text style entries
/*!
*  Class to handle text style symbol table entries
*  @author Rallaz
*/
void DRW_Textstyle::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 3:
        font = reader->getUtf8String();
        break;
    case 4:
        bigFont = reader->getUtf8String();
        break;
    case 40:
        height = reader->getDouble();
        break;
    case 41:
        width = reader->getDouble();
        break;
    case 50:
        oblique = reader->getDouble();
        break;
    case 42:
        lastHeight = reader->getDouble();
        break;
    case 71:
        genFlag = reader->getInt32();
        break;
    case 1071:
        fontFamily = reader->getInt32();
        break;
    default:
        DRW_TableEntry::parseCode(code, reader);
        break;
    }
}

bool DRW_Textstyle::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_TableEntry::parseDwg(version, buf);
    DBG("\n***************************** parsing text style *********************************************\n");
    if (!ret)
        return ret;
    if (version > DRW::AC1018) {//2007+
        name = buf->getVariableText();
    } else {//2004-
        name = buf->getVariableUtf8Text();
    }
    DBG("text style name: "); DBG(name.c_str()); DBG("\n");
    flags |= buf->getBit()<< 6;//style are referenced for a entity, style code 70, bit 7 (64)
    /*dint16 xrefindex =*/ buf->getBitShort();
    flags |= buf->getBit() << 4; //is refx dependent, style code 70, bit 5 (16)
    flags |= buf->getBit() << 2; //vertical text, stile code 70, bit 3 (4)
    flags |= buf->getBit(); //if is a shape file instead of text, style code 70, bit 1 (1)
    height = buf->getBitDouble();
    width = buf->getBitDouble();
    oblique = buf->getBitDouble();
    genFlag = buf->getRawChar8();
    lastHeight = buf->getBitDouble();
    if (version > DRW::AC1018) {//2007+
        font = buf->getVariableText();
        bigFont = buf->getVariableText();
    } else {//2004-
        font = buf->getVariableUtf8Text();
        bigFont = buf->getVariableUtf8Text();
    }
    dwgHandle shpControlH = buf->getHandle();
    DBG(" shpControlH Handle: "); DBG(shpControlH.code); DBG("."); DBG(shpControlH.size); DBG("."); DBG(shpControlH.ref); DBG("\n");
//    if(!blockIsXref && !xrefOverlaid){
//        dwgHandle firstH = buf->getHandle();
//        DBG(" firstH entity Handle: "); DBG(firstH.code); DBG(".");
//        DBG(firstH.size); DBG("."); DBG(firstH.ref); DBG("\n");
//        dwgHandle lastH = buf->getHandle();
//        DBG(" lastH entity Handle: "); DBG(lastH.code); DBG(".");
//        DBG(lastH.size); DBG("."); DBG(lastH.ref); DBG("\n");
//    }

    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
    //    RS crc;   //RS */
    return buf->isGood();
}

//! Class to handle vport entries
/*!
*  Class to handle vport symbol table entries
*  @author Rallaz
*/
void DRW_Vport::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 10:
        lowerLeft.x = reader->getDouble();
        break;
    case 20:
        lowerLeft.y = reader->getDouble();
        break;
    case 11:
        UpperRight.x = reader->getDouble();
        break;
    case 21:
        UpperRight.y = reader->getDouble();
        break;
    case 12:
        center.x = reader->getDouble();
        break;
    case 22:
        center.y = reader->getDouble();
        break;
    case 13:
        snapBase.x = reader->getDouble();
        break;
    case 23:
        snapBase.y = reader->getDouble();
        break;
    case 14:
        snapSpacing.x = reader->getDouble();
        break;
    case 24:
        snapSpacing.y = reader->getDouble();
        break;
    case 15:
        gridSpacing.x = reader->getDouble();
        break;
    case 25:
        gridSpacing.y = reader->getDouble();
        break;
    case 16:
        viewDir.x = reader->getDouble();
        break;
    case 26:
        viewDir.y = reader->getDouble();
        break;
    case 36:
        viewDir.z = reader->getDouble();
        break;
    case 17:
        viewTarget.x = reader->getDouble();
        break;
    case 27:
        viewTarget.y = reader->getDouble();
        break;
    case 37:
        viewTarget.z = reader->getDouble();
        break;
    case 40:
        height = reader->getDouble();
        break;
    case 41:
        ratio = reader->getDouble();
        break;
    case 42:
        lensHeight = reader->getDouble();
        break;
    case 43:
        frontClip = reader->getDouble();
        break;
    case 44:
        backClip = reader->getDouble();
        break;
    case 50:
        snapAngle = reader->getDouble();
        break;
    case 51:
        twistAngle = reader->getDouble();
        break;
    case 71:
        viewMode = reader->getInt32();
        break;
    case 72:
        circleZoom = reader->getInt32();
        break;
    case 73:
        fastZoom = reader->getInt32();
        break;
    case 74:
        ucsIcon = reader->getInt32();
        break;
    case 75:
        snap = reader->getInt32();
        break;
    case 76:
        grid = reader->getInt32();
        break;
    case 77:
        snapStyle = reader->getInt32();
        break;
    case 78:
        snapIsopair = reader->getInt32();
        break;
    default:
        DRW_TableEntry::parseCode(code, reader);
        break;
    }
}

bool DRW_Vport::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_TableEntry::parseDwg(version, buf);
    DBG("\n***************************** parsing VPort ************************************************\n");
    if (!ret)
        return ret;
    if (version > DRW::AC1018) {//2007+
        name = buf->getVariableText();
    } else {//2004-
        name = buf->getVariableUtf8Text();
    }
    DBG("vport name: "); DBG(name.c_str()); DBG("\n");

    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
    //    RS crc;   //RS */
    return buf->isGood();
}

void DRW_ImageDef::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 1:
        name = reader->getUtf8String();
        break;
    case 5:
        handle = reader->getString();
        break;
    case 10:
        u = reader->getDouble();
        break;
    case 20:
        v = reader->getDouble();
        break;
    case 11:
        up = reader->getDouble();
        break;
    case 12:
        vp = reader->getDouble();
        break;
    case 21:
        vp = reader->getDouble();
        break;
    case 280:
        loaded = reader->getInt32();
        break;
    case 281:
        resolution = reader->getInt32();
        break;
    default:
        break;
    }
}

