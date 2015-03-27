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

#include <cstdlib>
#include "drw_entities.h"
#include "intern/dxfreader.h"
#include "intern/dwgbuffer.h"
#include "intern/drw_dbg.h"


//! Calculate arbitary axis
/*!
*   Calculate arbitary axis for apply extrusions
*  @author Rallaz
*/
void DRW_Entity::calculateAxis(DRW_Coord extPoint){
    //Follow the arbitrary DXF definitions for extrusion axes.
    if (fabs(extPoint.x) < 0.015625 && fabs(extPoint.y) < 0.015625) {
        //If we get here, implement Ax = Wy x N where Wy is [0,1,0] per the DXF spec.
        //The cross product works out to Wy.y*N.z-Wy.z*N.y, Wy.z*N.x-Wy.x*N.z, Wy.x*N.y-Wy.y*N.x
        //Factoring in the fixed values for Wy gives N.z,0,-N.x
        extAxisX.x = extPoint.z;
        extAxisX.y = 0;
        extAxisX.z = -extPoint.x;
    } else {
        //Otherwise, implement Ax = Wz x N where Wz is [0,0,1] per the DXF spec.
        //The cross product works out to Wz.y*N.z-Wz.z*N.y, Wz.z*N.x-Wz.x*N.z, Wz.x*N.y-Wz.y*N.x
        //Factoring in the fixed values for Wz gives -N.y,N.x,0.
        extAxisX.x = -extPoint.y;
        extAxisX.y = extPoint.x;
        extAxisX.z = 0;
    }

    extAxisX.unitize();

    //Ay = N x Ax
    extAxisY.x = (extPoint.y * extAxisX.z) - (extAxisX.y * extPoint.z);
    extAxisY.y = (extPoint.z * extAxisX.x) - (extAxisX.z * extPoint.x);
    extAxisY.z = (extPoint.x * extAxisX.y) - (extAxisX.x * extPoint.y);

    extAxisY.unitize();
}
//! Extrude a point using arbitary axis
/*!
*   apply extrusion in a point using arbitary axis (previous calculated)
*  @author Rallaz
*/
void DRW_Entity::extrudePoint(DRW_Coord extPoint, DRW_Coord *point){
    double px, py, pz;
    px = (extAxisX.x*point->x)+(extAxisY.x*point->y)+(extPoint.x*point->z);
    py = (extAxisX.y*point->x)+(extAxisY.y*point->y)+(extPoint.y*point->z);
    pz = (extAxisX.z*point->x)+(extAxisY.z*point->y)+(extPoint.z*point->z);

    point->x = px;
    point->y = py;
    point->z = pz;
}

bool DRW_Entity::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 5:
        handle = reader->getHandleString();
        break;
    case 330:
        handleBlock = reader->getHandleString();
        break;
    case 8:
        layer = reader->getUtf8String();
        break;
    case 6:
        lineType = reader->getUtf8String();
        break;
    case 62:
        color = reader->getInt32();
        break;
    case 370:
        lWeight = DRW_LW_Conv::dxfInt2lineWidth(reader->getInt32());
        break;
    case 48:
        ltypeScale = reader->getDouble();
        break;
    case 60:
        visible = reader->getBool();
        break;
    case 420:
        color24 = reader->getInt32();
        break;
    case 430:
        colorName = reader->getString();
        break;
    case 67:
        space = (DRW::Space)reader->getInt32(); //RLZ verify cast values
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
    return true;
}

bool DRW_Entity::parseDwg(DRW::Version version, dwgBuffer *buf){
    duint32 objSize;
    duint8 ltFlags; //BB
    /*dint16 oType =*/ buf->getBitShort();
    DBG("\n***************************** parsing entity *********************************************\n");

    if (version > DRW::AC1014) {//2000+
        objSize = buf->getRawLong32();  //RL 32bits
    }
    dwgHandle ho = buf->getHandle();
    handle = ho.ref;
    DBG("Entity Handle: "); DBG(ho.code); DBG(".");
    DBG(ho.size); DBG("."); DRW_DBGH(ho.ref);
    dint16 extDataSize = buf->getBitShort(); //BS
    DBG(" ext data size: "); DBG(extDataSize);
    while (extDataSize>0 && buf->isGood()) {
        /* RLZ: TODO */
        dwgHandle ah = buf->getHandle();
        DBG("App Handle: "); DBG(ah.code); DBG("."); DBG(ah.size); DBG("."); DRW_DBGH(ah.ref);
        char byteStr[extDataSize];
        buf->getBytes(byteStr, extDataSize);
        dwgBuffer buff(byteStr, extDataSize, buf->decoder);

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
        extDataSize = buf->getBitShort(); //BS
        DBG(" ext data size: "); DBG(extDataSize);
    } //end parsing extData (EED)
    duint8 graphFlag = buf->getBit(); //B
    DBG(" graphFlag: "); DBG(graphFlag); DBG("\n");
    if (graphFlag) {
        duint32 graphData = buf->getRawLong32();  //RL 32bits
        DBG("graphData in bytes: "); DBG(graphData); DBG("\n");
// RLZ: TODO
        //skip graphData bytes
        char byteStr[graphData];
        buf->getBytes(byteStr, graphData);
        dwgBuffer buff(byteStr, graphData, buf->decoder);
        DBG("graph data remaining bytes: "); DBG(buff.numRemainingBytes()); DBG("\n");
    }
    if (version < DRW::AC1015) {//14-
        objSize = buf->getRawLong32();  //RL 32bits size in bits
    }
    DBG(" objSize in bits: "); DBG(objSize); DBG("\n");
    duint8 entmode = buf->get2Bits(); //BB
    if (entmode == 0)
        entmode = 2;
    else if(entmode ==2)
        entmode = 0;
    space = (DRW::Space)entmode; //RLZ verify cast values
    DBG("entmode: "); DBG(entmode);
    duint8 numReactors = buf->getBitShort(); //BS
    DBG(", numReactors: "); DBG(numReactors);

    if (version < DRW::AC1015) {//14-
        if(buf->getBit()) //is bylayer line type
            lineType = "BYLAYER";
        else
            lineType = "";
    }
    if (version > DRW::AC1015) {//2004+
        /*duint8 xDictFlag =*/ buf->getBit();
    }
    if (version > DRW::AC1015) {//2004+
        /*duint8 xDictFlag =*/ buf->getBit();
    }

    haveNextLinks = buf->getBit(); //aka nolinks //B
    DBG(", haveNextLinks (0 yes, 1 prev next): "); DBG(haveNextLinks); DBG("\n");

    color = buf->getBitShort(); //BS or CMC
    ltypeScale = buf->getBitDouble(); //BD
    DBG(" entity color: "); DBG(color);
    DBG(" ltScale: "); DBG(ltypeScale); DBG("\n");
    if (version > DRW::AC1014) {//2000+
        ltFlags = buf->get2Bits(); //BB
        if (ltFlags == 1)
            lineType = "byblock";
        else if (ltFlags == 2)
            lineType = "continuous";
        else if (ltFlags == 0)
            lineType = "bylayer";
        else //handle at end
            lineType = "";
        DBG("ltFlags: "); DBG(ltFlags);
        DBG(" lineType: "); DBG(lineType.c_str());
        plotFlags = buf->get2Bits(); //BB
        DBG(", plotFlags: "); DBG(plotFlags);
    }
    if (version > DRW::AC1018) {//2007+
        /*duint8 materialFlag =*/ buf->get2Bits(); //BB
        /*duint8 shadowFlag =*/ buf->getRawChar8(); //RC
    }
    dint16 invisibleFlag = buf->getBitShort(); //BS
    DBG(" invisibleFlag: "); DBG(invisibleFlag);
    if (version > DRW::AC1014) {//2000+
        lWeight = DRW_LW_Conv::dwgInt2lineWidth( buf->getRawChar8() ); //RC
        DBG(" lwFlag (lWeight): "); DBG(lWeight); DBG("\n");
    }
    return buf->isGood();
}

bool DRW_Entity::parseDwgEntHandle(DRW::Version version, dwgBuffer *buf){
    //    X handleAssoc;   //X
        DBG("X handleAssoc: \n");
        //lineType handle
        if(space == 2){//entity are in block
            dwgHandle ownerH = buf->getHandle();
            DBG("owner Handle: "); DRW_DBGH(ownerH.code); DBG(".");
            DBG(ownerH.size); DBG("."); DBG(ownerH.ref); DBG("\n");
            DBG("   Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
            if (ownerH.code == 12)
                handleBlock = handle - ownerH.ref;
            else if (ownerH.code == 10)
                handleBlock = handle + ownerH.ref;
            else if (ownerH.code == 8)
                handleBlock = handle - 1;
            else if (ownerH.code == 6)
                handleBlock = handle + 1;
            else
                handleBlock = ownerH.ref;
        }
        DBG(" Block Handle: "); DBG(handleBlock); DBG(".");
        dwgHandle XDicObjH = buf->getHandle();
        DBG(" XDicObj control Handle: "); DBG(XDicObjH.code); DBG(".");
        DBG(XDicObjH.size); DBG("."); DRW_DBGH(XDicObjH.ref); DBG("\n");
        DBG("Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");

        if (version > DRW::AC1014) {//2000+
            if (haveNextLinks == 0) {
                for (int i=0; i<2;i++) {
                    dwgHandle nextLinkH = buf->getOffsetHandle(handle);
                    DBG(" nextLinkers Handle: "); DBG(nextLinkH.code); DBG(".");
                    DBG(nextLinkH.size); DBG("."); DRW_DBGH(nextLinkH.ref); DBG("\n");
                    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
                }
            }
        }
        //layer handle
        layerH = buf->getOffsetHandle(handle);
        DBG(" layer Handle: "); DBG(layerH.code); DBG(".");
        DBG(layerH.size); DBG("."); DRW_DBGH(layerH.ref); DBG("\n");
        DBG("   Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
        //lineType handle
        if(lineType.empty()){
            lTypeH = buf->getOffsetHandle(handle);
            DBG("linetype Handle: "); DBG(lTypeH.code); DBG(".");
            DBG(lTypeH.size); DBG("."); DRW_DBGH(lTypeH.ref); DBG("\n");
            DBG("   Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
        }
        if (version < DRW::AC1015) {//14-
            if (haveNextLinks == 0) {
                    dwgHandle nextLinkH = buf->getOffsetHandle(handle);
                    DBG(" prev nextLinkers Handle: "); DBG(nextLinkH.code); DBG(".");
                    DBG(nextLinkH.size); DBG("."); DRW_DBGH(nextLinkH.ref); DBG("\n");
                    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
                    prevEntLink = nextLinkH.ref;
                    nextLinkH = buf->getOffsetHandle(handle);
                    DBG(" next nextLinkers Handle: "); DBG(nextLinkH.code); DBG(".");
                    DBG(nextLinkH.size); DBG("."); DRW_DBGH(nextLinkH.ref); DBG("\n");
                    DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
                    nextEntLink = nextLinkH.ref;
            } else {
                nextEntLink = handle+1;
                prevEntLink = handle-1;
            }
        }
        if (version > DRW::AC1014) {//2000+
            if (plotFlags == 3) {
                dwgHandle plotStyleH = buf->getOffsetHandle(handle);
                DBG(" plot style Handle: "); DBG(plotStyleH.code); DBG(".");
                DBG(plotStyleH.size); DBG("."); DBG(plotStyleH.ref); DBG("\n");
                DBG("\n Remaining bytes: "); DBG(buf->numRemainingBytes()); DBG("\n");
            }
        }
    return buf->isGood();
}

void DRW_Point::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 10:
        basePoint.x = reader->getDouble();
        break;
    case 20:
        basePoint.y = reader->getDouble();
        break;
    case 30:
        basePoint.z = reader->getDouble();
        break;
    case 39:
        thickness = reader->getDouble();
        break;
    case 210:
        haveExtrusion = true;
        extPoint.x = reader->getDouble();
        break;
    case 220:
        extPoint.y = reader->getDouble();
        break;
    case 230:
        extPoint.z = reader->getDouble();
        break;
    default:
        DRW_Entity::parseCode(code, reader);
        break;
    }
}

bool DRW_Point::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing point *********************************************\n");

        basePoint.x = buf->getBitDouble();
        basePoint.y = buf->getBitDouble();
        basePoint.z = buf->getBitDouble();
    DBG("startX: "); DBG(basePoint.x);
    DBG(", startY: "); DBG(basePoint.y);
    DBG(", startZ: "); DBG(basePoint.z);
    bool readOpt = true;
    if (version > DRW::AC1014) {//2000+
        readOpt = !buf->getBit();
    }
    if (readOpt) {
        thickness = buf->getBitDouble();//BD
    }
    readOpt = true;
    if (version > DRW::AC1014) {//2000+
        readOpt = !buf->getBit();
    }
    if (readOpt) {
        extPoint.x = buf->getBitDouble();//BD
        extPoint.y = buf->getBitDouble();//BD
        extPoint.z = buf->getBitDouble();//BD
    }
    double x_axis = buf->getBitDouble();//BD
    DBG("  x_axis: ");DBG(x_axis);DBG("\n");
//    X handleAssoc;   //X
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;
    //    RS crc;   //RS */

    return buf->isGood();
}

void DRW_Line::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 11:
        secPoint.x = reader->getDouble();
        break;
    case 21:
        secPoint.y = reader->getDouble();
        break;
    case 31:
        secPoint.z = reader->getDouble();
        break;
    default:
        DRW_Point::parseCode(code, reader);
        break;
    }
}

bool DRW_Line::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing line *********************************************\n");

    if (version < DRW::AC1015) {//14-
        basePoint.x = buf->getBitDouble();
        basePoint.y = buf->getBitDouble();
        basePoint.z = buf->getBitDouble();
        secPoint.x = buf->getBitDouble();
        secPoint.y = buf->getBitDouble();
        secPoint.z = buf->getBitDouble();
    }
    if (version > DRW::AC1014) {//2000+
        bool zIsZero = buf->getBit(); //B
        basePoint.x = buf->getRawDouble();//RD
        secPoint.x = buf->getDefaultDouble(basePoint.x);//DD
        basePoint.y = buf->getRawDouble();//RD
        secPoint.y = buf->getDefaultDouble(basePoint.y);//DD
        if (!zIsZero) {
            basePoint.z = buf->getRawDouble();//RD
            secPoint.z = buf->getDefaultDouble(basePoint.z);//DD
        }
    }
    DBG("startX: "); DBG(basePoint.x);
    DBG(", endX: "); DBG(secPoint.x);
    DBG(", startY: "); DBG(basePoint.y);
    DBG(", endY: "); DBG(secPoint.y);
    DBG(", startZ: "); DBG(basePoint.z);
    DBG(", endZ: "); DBG(secPoint.z); DBG("\n");
    bool readOpt = true;
    if (version > DRW::AC1014) {//2000+
        readOpt = !buf->getBit();
    }
    if (readOpt) {
        thickness = buf->getBitDouble();//BD
    }
    readOpt = true;
    if (version > DRW::AC1014) {//2000+
        readOpt = !buf->getBit();
    }
    if (readOpt) {
        extPoint.x = buf->getBitDouble();//BD
        extPoint.y = buf->getBitDouble();//BD
        extPoint.z = buf->getBitDouble();//BD
    }
//    X handleAssoc;   //X
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;
//    RS crc;   //RS */
    return buf->isGood();
}

void DRW_Circle::applyExtrusion(){
    if (haveExtrusion) {
        //NOTE: Commenting these out causes the the arcs being tested to be located
        //on the other side of the y axis (all x dimensions are negated).
        calculateAxis(extPoint);
        extrudePoint(extPoint, &basePoint);
    }
}

void DRW_Circle::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 40:
        radious = reader->getDouble();
        break;
    default:
        DRW_Point::parseCode(code, reader);
        break;
    }
}

bool DRW_Circle::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing circle *********************************************\n");

    basePoint.x = buf->getBitDouble();
    basePoint.y = buf->getBitDouble();
    basePoint.z = buf->getBitDouble();
    DBG("startX: "); DBG(basePoint.x);
    DBG(", startY: "); DBG(basePoint.y);
    DBG(", startZ: "); DBG(basePoint.z);
    radious = buf->getBitDouble();
    DBG(" radius: "); DBG(radious);
    bool readOpt = true;
    if (version > DRW::AC1014) {//2000+
        readOpt = !buf->getBit();
    }
    if (readOpt) {
        thickness = buf->getBitDouble();//BD
    }
    readOpt = true;
    if (version > DRW::AC1014) {//2000+
        readOpt = !buf->getBit();
    }
    if (readOpt) {
        extPoint.x = buf->getBitDouble();//BD
        extPoint.y = buf->getBitDouble();//BD
        extPoint.z = buf->getBitDouble();//BD
    }
//    X handleAssoc;   //X
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;
//    RS crc;   //RS */
    return buf->isGood();

void DRW_Arc::applyExtrusion(){
    DRW_Circle::applyExtrusion();

    if(haveExtrusion){
        // If the extrusion vector has a z value less than 0, the angles for the arc
        // have to be mirrored since DXF files use the right hand rule.
        // Note that the following code only handles the special case where there is a 2D
        // drawing with the z axis heading into the paper (or rather screen). An arbitrary
        // extrusion axis (with x and y values greater than 1/64) may still have issues.
        if (fabs(extPoint.x) < 0.015625 && fabs(extPoint.y) < 0.015625 && extPoint.z < 0.0) {
            staangle=M_PI-staangle;
            endangle=M_PI-endangle;

            double temp = staangle;
            staangle=endangle;
            endangle=temp;
        }
    }
}

void DRW_Arc::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 50:
        staangle = reader->getDouble()/ ARAD;
        break;
    case 51:
        endangle = reader->getDouble()/ ARAD;
        break;
    default:
        DRW_Circle::parseCode(code, reader);
        break;
    }
}

bool DRW_Arc::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing circle arc *********************************************\n");

    basePoint.x = buf->getBitDouble();
    basePoint.y = buf->getBitDouble();
    basePoint.z = buf->getBitDouble();
    DBG("startX: "); DBG(basePoint.x); DBG(", Y: "); DBG(basePoint.y); DBG(", : "); DBG(basePoint.z);
    radious = buf->getBitDouble();
    DBG(" radius: "); DBG(radious);
    bool readOpt = true;
    if (version > DRW::AC1014) {//2000+
        readOpt = !buf->getBit();
    }
    if (readOpt) {
        thickness = buf->getBitDouble();//BD
    }
    readOpt = true;
    if (version > DRW::AC1014) {//2000+
        readOpt = !buf->getBit();
    }
    if (readOpt) {
        extPoint.x = buf->getBitDouble();//BD
        extPoint.y = buf->getBitDouble();//BD
        extPoint.z = buf->getBitDouble();//BD
    }
    staangle = buf->getBitDouble();
    DBG(" start angle: "); DBG(staangle);
    endangle = buf->getBitDouble();
    DBG(" end angle: "); DBG(endangle);
//    X handleAssoc;   //X
//    RS crc;   //RS */
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;
    return buf->isGood();
}

void DRW_Ellipse::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 40:
        ratio = reader->getDouble();
        break;
    case 41:
        staparam = reader->getDouble();
        break;
    case 42:
        endparam = reader->getDouble();
        break;
    default:
        DRW_Line::parseCode(code, reader);
        break;
    }
}

void DRW_Ellipse::applyExtrusion(){
    if (haveExtrusion) {
        calculateAxis(extPoint);
        extrudePoint(extPoint, &secPoint);
        double intialparam = staparam;
        if (extPoint.z < 0.){
            staparam = M_PIx2 - endparam;
            endparam = M_PIx2 - intialparam;
        }
    }
}

//if ratio > 1 minor axis are greather than major axis, correct it
void DRW_Ellipse::correctAxis(){
    bool complete = false;
    if (staparam == endparam) {
        staparam = 0.0;
        endparam = M_PIx2; //2*M_PI;
        complete = true;
    }
    if (ratio > 1){
        if ( fabs(endparam - staparam - M_PIx2) < 1.0e-10)
            complete = true;
        double incX = secPoint.x;
        secPoint.x = -(secPoint.y * ratio);
        secPoint.y = incX*ratio;
        ratio = 1/ratio;
        if (!complete){
            if (staparam < M_PI_2)
                staparam += M_PI *2;
            if (endparam < M_PI_2)
                endparam += M_PI *2;
            endparam -= M_PI_2;
            staparam -= M_PI_2;
        }
    }
}

bool DRW_Ellipse::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing ellipse *********************************************\n");

    basePoint.x = buf->getBitDouble();
    basePoint.y = buf->getBitDouble();
    basePoint.z = buf->getBitDouble();
    secPoint.x = buf->getBitDouble();
    secPoint.y = buf->getBitDouble();
    secPoint.z = buf->getBitDouble();
    DBG("center X: "); DBG(basePoint.x); DBG(", Y: "); DBG(basePoint.y); DBG(", Z: "); DBG(basePoint.z);
    DBG(", axis X: "); DBG(secPoint.x); DBG(", Y: "); DBG(secPoint.y); DBG(", Z: "); DBG(secPoint.z); DBG("\n");
    extPoint.x = buf->getBitDouble();//BD
    extPoint.y = buf->getBitDouble();//BD
    extPoint.z = buf->getBitDouble();//BD
    ratio = buf->getBitDouble();//BD
    DBG("ratio: "); DBG(ratio);
    staparam = buf->getBitDouble();//BD
    DBG(" start param: "); DBG(staparam);
    endparam = buf->getBitDouble();//BD
    DBG(" end param: "); DBG(endparam); DBG("\n");
//    X handleAssoc;   //X
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;
//    RS crc;   //RS */
    return buf->isGood();
}

//parts are the number of vertex to split polyline, default 128
void DRW_Ellipse::toPolyline(DRW_Polyline *pol, int parts){
    double radMajor, radMinor, cosRot, sinRot, incAngle, curAngle;
    double cosCurr, sinCurr;
    radMajor = sqrt(secPoint.x*secPoint.x + secPoint.y*secPoint.y);
    radMinor = radMajor*ratio;
    //calculate sin & cos of included angle
    incAngle = atan2(secPoint.y, secPoint.x);
    cosRot = cos(incAngle);
    sinRot = sin(incAngle);
    incAngle = M_PIx2 / parts;
    curAngle = staparam;
    int i = curAngle/incAngle;
    do {
        if (curAngle > endparam) {
            curAngle = endparam;
            i = parts+2;
        }
        cosCurr = cos(curAngle);
        sinCurr = sin(curAngle);
        double x = basePoint.x + (cosCurr*cosRot*radMajor) - (sinCurr*sinRot*radMinor);
        double y = basePoint.y + (cosCurr*sinRot*radMajor) + (sinCurr*cosRot*radMinor);
        pol->addVertex( DRW_Vertex(x, y, 0.0, 0.0));
        curAngle = (++i)*incAngle;
    } while (i<parts);
    if ( fabs(endparam - staparam - M_PIx2) < 1.0e-10){
        pol->flags = 1;
    }
    pol->layer = this->layer;
    pol->lineType = this->lineType;
    pol->color = this->color;
    pol->lWeight = this->lWeight;
    pol->extPoint = this->extPoint;
}

void DRW_Trace::applyExtrusion(){
    if (haveExtrusion) {
        calculateAxis(extPoint);
        extrudePoint(extPoint, &basePoint);
        extrudePoint(extPoint, &secPoint);
        extrudePoint(extPoint, &thirdPoint);
        extrudePoint(extPoint, &fourPoint);
    }
}

void DRW_Trace::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 12:
        thirdPoint.x = reader->getDouble();
        break;
    case 22:
        thirdPoint.y = reader->getDouble();
        break;
    case 32:
        thirdPoint.z = reader->getDouble();
        break;
    case 13:
        fourPoint.x = reader->getDouble();
        break;
    case 23:
        fourPoint.y = reader->getDouble();
        break;
    case 33:
        fourPoint.z = reader->getDouble();
        break;
    default:
        DRW_Line::parseCode(code, reader);
        break;
    }
}

void DRW_Solid::parseCode(int code, dxfReader *reader){
    DRW_Trace::parseCode(code, reader);
}

bool DRW_Solid::parseDwg(DRW::Version v, dwgBuffer *buf)
{
    bool ret = DRW_Entity::parseDwg(v, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing Solid *********************************************\n");

    thickness = buf->getThickness(v>DRW::AC1014);
    basePoint.z = buf->getRawDouble();
    basePoint.x = buf->getRawDouble();
    basePoint.y = buf->getRawDouble();
    secPoint.x = buf->getRawDouble();
    secPoint.y = buf->getRawDouble();
    secPoint.z = basePoint.z;
    thirdPoint.x = buf->getRawDouble();
    thirdPoint.y = buf->getRawDouble();
    thirdPoint.z = basePoint.z;
    fourPoint.x = buf->getRawDouble();
    fourPoint.y = buf->getRawDouble();
    fourPoint.z = basePoint.z;
    extPoint = buf->getExtrusion(v>DRW::AC1014);

    DBG(" - base X: "); DBG(basePoint.x);
    DBG(", Y: "); DBG(basePoint.y);
    DBG(", Z: "); DBG(basePoint.z); DBG("\n");
    DBG(" - sec X: "); DBG(secPoint.x);
    DBG(", Y: "); DBG(secPoint.y);
    DBG(", Z: "); DBG(secPoint.z); DBG("\n");
    DBG(" - third X: "); DBG(thirdPoint.x);
    DBG(", Y: "); DBG(thirdPoint.y);
    DBG(", Z: "); DBG(thirdPoint.z); DBG("\n");
    DBG(" - fourth X: "); DBG(fourPoint.x);
    DBG(", Y: "); DBG(fourPoint.y);
    DBG(", Z: "); DBG(fourPoint.z); DBG("\n");
    DBG(" - extrusion: "); DBG(extPoint.x);
    DBG(", Y: "); DBG(extPoint.y);
    DBG(", Z: "); DBG(extPoint.z); DBG("\n");
    DBG(" - thickness: "); DBG(thickness); DBG("\n");

    /* Common Entity Handle Data */
    ret = DRW_Entity::parseDwgEntHandle(v, buf);
    if (!ret)
        return ret;

    /* CRC X --- */
    return buf->isGood();
}

void DRW_3Dface::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 70:
        invisibleflag = reader->getInt32();
        break;
    default:
        DRW_Trace::parseCode(code, reader);
        break;
    }
}

bool DRW_3Dface::parseDwg(DRW::Version v, dwgBuffer *buf)
{
    bool ret = DRW_Entity::parseDwg(v, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing 3Dface *********************************************\n");

    if ( v < DRW::AC1015 ) {// R13 & R14
        basePoint.x = buf->getBitDouble();
        basePoint.y = buf->getBitDouble();
        basePoint.z = buf->getBitDouble();
        secPoint.x = buf->getBitDouble();
        secPoint.y = buf->getBitDouble();
        secPoint.z = buf->getBitDouble();
        thirdPoint.x = buf->getBitDouble();
        thirdPoint.y = buf->getBitDouble();
        thirdPoint.z = buf->getBitDouble();
        fourPoint.x = buf->getBitDouble();
        fourPoint.y = buf->getBitDouble();
        fourPoint.z = buf->getBitDouble();
        invisibleflag = buf->getBitShort();
    } else { // 2000+
        bool has_no_flag = buf->getBit();
        bool z_is_zero = buf->getBit();
        basePoint.x = buf->getRawDouble();
        basePoint.y = buf->getRawDouble();
        basePoint.z = z_is_zero ? 0.0 : buf->getRawDouble();
        secPoint.x = buf->getDefaultDouble(basePoint.x);
        secPoint.y = buf->getDefaultDouble(basePoint.y);
        secPoint.z = buf->getDefaultDouble(basePoint.z);
        thirdPoint.x = buf->getDefaultDouble(secPoint.x);
        thirdPoint.y = buf->getDefaultDouble(secPoint.y);
        thirdPoint.z = buf->getDefaultDouble(secPoint.z);
        fourPoint.x = buf->getDefaultDouble(thirdPoint.x);
        fourPoint.y = buf->getDefaultDouble(thirdPoint.y);
        fourPoint.z = buf->getDefaultDouble(thirdPoint.z);
        invisibleflag = has_no_flag ? (int)AllEdges : buf->getBitShort();
    }
    drw_assert(invisibleflag>=NoEdge);
    drw_assert(invisibleflag<=AllEdges);

    DBG(" - base X: "); DBG(basePoint.x);
    DBG(", Y: "); DBG(basePoint.y);
    DBG(", Z: "); DBG(basePoint.z); DBG("\n");
    DBG(" - sec X: "); DBG(secPoint.x);
    DBG(", Y: "); DBG(secPoint.y);
    DBG(", Z: "); DBG(secPoint.z); DBG("\n");
    DBG(" - third X: "); DBG(thirdPoint.x);
    DBG(", Y: "); DBG(thirdPoint.y);
    DBG(", Z: "); DBG(thirdPoint.z); DBG("\n");
    DBG(" - fourth X: "); DBG(fourPoint.x);
    DBG(", Y: "); DBG(fourPoint.y);
    DBG(", Z: "); DBG(fourPoint.z); DBG("\n");
    DBG(" - Invisibility mask: "); DBG(invisibleflag); DBG("\n");

    /* Common Entity Handle Data */
    ret = DRW_Entity::parseDwgEntHandle(v, buf);
    if (!ret)
        return ret;

    /* CRC X --- */
    return buf->isGood();
}

void DRW_Block::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 2:
        name = reader->getUtf8String();
        break;
    case 70:
        flags = reader->getInt32();
        break;
    default:
        DRW_Point::parseCode(code, reader);
        break;
    }
}

bool DRW_Block::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    if (!isEnd){
        DBG("\n***************************** parsing block *********************************************\n");
        if (version > DRW::AC1018) {//2007+
            name = buf->getVariableText();
        } else {//2004-
            name = buf->getVariableUtf8Text();
        }
        DBG("Block name: "); DBG(name.c_str()); DBG("\n");
    } else {
        DBG("\n***************************** parsing end block *********************************************\n");
    }
//    X handleAssoc;   //X
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;
//    RS crc;   //RS */
    return buf->isGood();
}

void DRW_Insert::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 2:
        name = reader->getUtf8String();
        break;
    case 41:
        xscale = reader->getDouble();
        break;
    case 42:
        yscale = reader->getDouble();
        break;
    case 43:
        zscale = reader->getDouble();
        break;
    case 50:
        angle = reader->getDouble();
        break;
    case 70:
        colcount = reader->getInt32();
        break;
    case 71:
        rowcount = reader->getInt32();
        break;
    case 44:
        colspace = reader->getDouble();
        break;
    case 45:
        rowspace = reader->getDouble();
        break;
    default:
        DRW_Point::parseCode(code, reader);
        break;
    }
}

bool DRW_Insert::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing insert *********************************************\n");

    basePoint.x = buf->getBitDouble();
    basePoint.y = buf->getBitDouble();
    basePoint.z = buf->getBitDouble();
    DBG("insertionpoint X: "); DBG(basePoint.x); DBG(", Y: "); DBG(basePoint.y); DBG(", Z: "); DBG(basePoint.z); DBG("\n");
    if (version < DRW::AC1015) {//14-
        xscale = buf->getBitDouble();
        yscale = buf->getBitDouble();
        zscale = buf->getBitDouble();
    } else {
        duint8 dataFlags = buf->get2Bits();
        if (dataFlags == 3){
            //none default value 1,1,1
        } else if (dataFlags == 1){ //x default value 1, y & z can be x value
            yscale = buf->getDefaultDouble(xscale);
            zscale = buf->getDefaultDouble(xscale);
        } else if (dataFlags == 2){
            xscale = buf->getRawDouble();
            yscale = zscale = xscale;
        } else { //dataFlags == 0
            xscale = buf->getRawDouble();
            yscale = buf->getDefaultDouble(xscale);
            zscale = buf->getDefaultDouble(xscale);
        }
    }
    angle = buf->getBitDouble();
    DBG("scale X: "); DBG(xscale); DBG(", Y: "); DBG(yscale); DBG(", Z: "); DBG(zscale); DBG(", angle: "); DBG(angle); DBG("\n");
        extPoint = buf->getExtrusion(false); //3BD R14 style

    bool hasAttrib = buf->getBit();
    DRW_UNUSED(hasAttrib);

    if (version > DRW::AC1015) {//2004+
        dint32 objCount = buf->getBitLong();
        DRW_UNUSED(objCount);
    }
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    blockRecH = buf->getHandle(); /* H 2 BLOCK HEADER (hard pointer) */
    DBG("BLOCK HEADER Handle: "); DBG(blockRecH.code); DBG(".");
    DBG(blockRecH.size); DBG("."); DBG(blockRecH.ref); DBG("\n");
    /*RLZ: TODO attribs follows*/

//    X handleAssoc;   //X
    if (!ret)
        return ret;
//    RS crc;   //RS */
    return buf->isGood();
}

void DRW_LWPolyline::applyExtrusion(){
    if (haveExtrusion) {
        calculateAxis(extPoint);
        for (unsigned int i=0; i<vertlist.size(); i++) {
            DRW_Vertex2D *vert = vertlist.at(i);
            DRW_Coord v(vert->x, vert->y, elevation);
            extrudePoint(extPoint, &v);
            vert->x = v.x;
            vert->y = v.y;
        }
    }
}

void DRW_LWPolyline::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 10: {
        vertex = new DRW_Vertex2D();
        vertlist.push_back(vertex);
        vertex->x = reader->getDouble();
        break; }
    case 20:
        if(vertex != NULL)
            vertex->y = reader->getDouble();
        break;
    case 40:
        if(vertex != NULL)
            vertex->stawidth = reader->getDouble();
        break;
    case 41:
        if(vertex != NULL)
            vertex->endwidth = reader->getDouble();
        break;
    case 42:
        if(vertex != NULL)
            vertex->bulge = reader->getDouble();
        break;
    case 38:
        elevation = reader->getDouble();
        break;
    case 39:
        thickness = reader->getDouble();
        break;
    case 43:
        width = reader->getDouble();
        break;
    case 70:
        flags = reader->getInt32();
        break;
    case 90:
        vertexnum = reader->getInt32();
        vertlist.reserve(vertexnum);
        break;
    case 210:
        haveExtrusion = true;
        extPoint.x = reader->getDouble();
        break;
    case 220:
        extPoint.y = reader->getDouble();
        break;
    case 230:
        extPoint.z = reader->getDouble();
        break;
    default:
        DRW_Entity::parseCode(code, reader);
        break;
    }
}

bool DRW_LWPolyline::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing LWPolyline *******************************************\n");

    flags = buf->getBitShort();
    DBG("flags value: "); DBG(flags); DBG("\n");
    if (flags & 4)
        width = buf->getBitDouble();
    if (flags & 8)
        elevation = buf->getBitDouble();
    if (flags & 2)
        thickness = buf->getBitDouble();
    if (flags & 1)
        extPoint = buf->getExtrusion(false);
    vertexnum = buf->getBitLong();
    vertlist.reserve(vertexnum);
    unsigned int bulgesnum = 0;
    if (flags & 16)
        bulgesnum = buf->getBitLong();
    int vertexIdCount = 0;
    if (version > DRW::AC1021) {//2010+
        if (flags & 1024)
            vertexIdCount = buf->getBitLong();
    }
    unsigned int widthsnum = 0;
    if (flags & 32)
        widthsnum = buf->getBitLong();

    if (vertexnum > 0) { //verify if is lwpol without vertex (empty)
        // add vertexs
        vertex = new DRW_Vertex2D();
        vertex->x = buf->getRawDouble();
        vertex->y = buf->getRawDouble();
        vertlist.push_back(vertex);
        for (int i = 1; i< vertexnum; i++){
            vertex = new DRW_Vertex2D();
            if (version < DRW::AC1015) {//14-
                vertex->x = buf->getRawDouble();
                vertex->y = buf->getRawDouble();
            } else {
                DRW_Vertex2D *pv = vertlist.back();
                vertex->x = buf->getDefaultDouble(pv->x);
                vertex->y = buf->getDefaultDouble(pv->y);
            }
            vertlist.push_back(vertex);
        }
        //add bulges
        for (unsigned int i = 0; i < bulgesnum; i++){
            double bulge = buf->getBitDouble();
            if (vertlist.size()> i)
                vertlist.at(i)->bulge = bulge;
        }
        //add vertexId
        if (version > DRW::AC1021) {//2010+
            for (int i = 0; i < vertexIdCount; i++){
                dint32 vertexId = buf->getBitLong();
                //TODO implement vertexId, do not exist in dxf
                DRW_UNUSED(vertexId);
//                if (vertlist.size()< i)
//                    vertlist.at(i)->vertexId = vertexId;
            }
        }
        //add widths
        for (unsigned int i = 0; i < widthsnum; i++){
            double staW = buf->getBitDouble();
            double endW = buf->getBitDouble();
            if (vertlist.size()< i) {
                vertlist.at(i)->stawidth = staW;
                vertlist.at(i)->endwidth = endW;
            }
        }
    }
//set flags to open/close
    flags = (flags & 512)? 1:0;
    /* Common Entity Handle Data */
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;
    /* CRC X --- */
    return buf->isGood();
}


void DRW_Text::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 40:
        height = reader->getDouble();
        break;
    case 41:
        widthscale = reader->getDouble();
        break;
    case 50:
        angle = reader->getDouble();
        break;
    case 51:
        oblique = reader->getDouble();
        break;
    case 71:
        textgen = reader->getInt32();
        break;
    case 72:
        alignH = (HAlign)reader->getInt32();
        break;
    case 73:
        alignV = (VAlign)reader->getInt32();
        break;
    case 1:
        text = reader->getUtf8String();
        break;
    case 7:
        style = reader->getUtf8String();
        break;
    default:
        DRW_Line::parseCode(code, reader);
        break;
    }
}

bool DRW_Text::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing text *********************************************\n");

    if (version < DRW::AC1015) {//14-
        basePoint.z = buf->getBitDouble(); /* Elevation BD --- */
        basePoint.x = buf->getRawDouble(); /* Insertion pt 2RD 10 */
        basePoint.y = buf->getRawDouble();
        DBG("Insertion X: "); DBG(basePoint.x); DBG(", Y: "); DBG(basePoint.y); DBG(", Z: "); DBG(basePoint.z); DBG("\n");
        secPoint.x = buf->getRawDouble();  /* Alignment pt 2RD 11 */
        secPoint.y = buf->getRawDouble();
        DBG("Alignment X: "); DBG(secPoint.x); DBG(", Y: "); DBG(secPoint.y); DBG("\n");
        extPoint.x = buf->getBitDouble(); /* Extrusion 3BD 210 */
        extPoint.y = buf->getBitDouble();
        extPoint.z = buf->getBitDouble();
        thickness = buf->getBitDouble(); /* Thickness BD 39 */
        oblique = buf->getBitDouble(); /* Oblique ang BD 51 */
        angle = buf->getBitDouble(); /* Rotation ang BD 50 */
        height = buf->getBitDouble(); /* Height BD 40 */
        widthscale = buf->getBitDouble(); /* Width factor BD 41 */
        DBG("thickness: "); DBG(thickness); DBG(", Oblique ang: "); DBG(oblique); DBG(", Width: "); DBG(widthscale);
        DBG(", Rotation: "); DBG(angle); DBG(", height: "); DBG(height); DBG("\n");
        text = buf->getVariableUtf8Text(); /* Text value TV 1 */
        DBG("text string: "); DBG(text.c_str());DBG("\n");
        textgen = buf->getBitShort(); /* Generation BS 71 */
        alignH = (HAlign)buf->getBitShort(); /* Horiz align. BS 72 */
        alignV = (VAlign)buf->getBitShort(); /* Vert align. BS 73 */
    }
    if (version > DRW::AC1014) {//2000+
        duint8 data_flags = buf->getRawChar8(); /* DataFlags RC Used to determine presence of subsquent data */
        DBG("data_flags: "); DBG(data_flags); DBG("\n");
        if ( !(data_flags & 0x01) )
        { /* Elevation RD --- present if !(DataFlags & 0x01) */
            basePoint.z = buf->getRawDouble();
        }
        basePoint.x = buf->getRawDouble(); /* Insertion pt 2RD 10 */
        basePoint.y = buf->getRawDouble();
        DBG("Insertion X: "); DBG(basePoint.x); DBG(", Y: "); DBG(basePoint.y); DBG(", Z: "); DBG(basePoint.z); DBG("\n");
        if ( !(data_flags & 0x02) )
        { /* Alignment pt 2DD 11 present if !(DataFlags & 0x02), use 10 & 20 values for 2 default values.*/
            secPoint.x = buf->getDefaultDouble(basePoint.x);
            secPoint.y = buf->getDefaultDouble(basePoint.y);
        }
        else
        {
            secPoint = basePoint;
        }
        DBG("Alignment X: "); DBG(secPoint.x); DBG(", Y: "); DBG(secPoint.y); DBG("\n");
        extPoint = buf->getExtrusion(true); /* Extrusion BE 210 */
        thickness = buf->getThickness(true); /* Thickness BT 39 */
        if ( !(data_flags & 0x04) )
        { /* Oblique ang RD 51 present if !(DataFlags & 0x04) */
            oblique = buf->getRawDouble();
        }
        if ( !(data_flags & 0x08) )
        { /* Rotation ang RD 50 present if !(DataFlags & 0x08) */
            angle = buf->getRawDouble();
        }
        height = buf->getRawDouble(); /* Height RD 40 */
        if ( !(data_flags & 0x10) )
        { /* Width factor RD 41 present if !(DataFlags & 0x10) */
            widthscale = buf->getRawDouble();
        }
        DBG("thickness: "); DBG(thickness); DBG(", Oblique ang: "); DBG(oblique); DBG(", Width: "); DBG(widthscale);
        DBG(", Rotation: "); DBG(angle); DBG(", height: "); DBG(height); DBG("\n");
        text = buf->getVariableUtf8Text(); /* Text value TV 1 */
        DBG("text string: "); DBG(text.c_str());DBG("\n");
        if ( !(data_flags & 0x20) )
        { /* Generation BS 71 present if !(DataFlags & 0x20) */
            textgen = buf->getBitShort();
        }
        if ( !(data_flags & 0x40) )
        { /* Horiz align. BS 72 present if !(DataFlags & 0x40) */
            alignH = (HAlign)buf->getBitShort();
        }
        if ( !(data_flags & 0x80) )
        { /* Vert align. BS 73 present if !(DataFlags & 0x80) */
            alignV = (VAlign)buf->getBitShort();
        }
    }

    /* Common Entity Handle Data */
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;

    styleH = buf->getHandle(); /* H 7 STYLE (hard pointer) */
    DBG("text style Handle: "); DBG(styleH.code); DBG(".");
    DBG(styleH.size); DBG("."); DBG(styleH.ref); DBG("\n");

    /* CRC X --- */
    return buf->isGood();
}

void DRW_MText::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 1:
        text += reader->getString();
        text = reader->toUtf8String(text);
        break;
    case 11:
        haveXAxis = true;
        DRW_Text::parseCode(code, reader);
        break;
    case 3:
        text += reader->getString();
        break;
    case 44:
        interlin = reader->getDouble();
        break;
    default:
        DRW_Text::parseCode(code, reader);
        break;
    }
}

bool DRW_MText::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing mtext *********************************************\n");

    basePoint.x = buf->getBitDouble(); /* Insertion pt 3BD 10 - First picked point. */
    basePoint.y = buf->getBitDouble(); /* (Location relative to text depends */
    basePoint.z = buf->getBitDouble(); /* on attachment point (71) */
    DBG("Insertion X: "); DBG(basePoint.x); DBG(", Y: "); DBG(basePoint.y); DBG(", Z: "); DBG(basePoint.z); DBG("\n");
    extPoint.x = buf->getBitDouble(); /* Extrusion 3BD 210 Undocumented; */
    extPoint.y = buf->getBitDouble(); /* appears in DXF and entget, but ACAD */
    extPoint.z = buf->getBitDouble(); /* doesn't even bother to adjust it to unit length. */
    DRW_Coord x_axis;
    x_axis.x = buf->getBitDouble(); /* X-axis dir 3BD 11 */
    x_axis.y = buf->getBitDouble(); /* Apparently the text x-axis vector. */
    x_axis.z = buf->getBitDouble(); /* ACAD maintains it as a unit vector. */
    /** @todo compute the angle from this */
    widthscale = buf->getBitDouble(); /* Rect width BD 41 */
    if (version > DRW::AC1018) {//2007+
        /* Rect height BD 46 Reference rectangle height. */
        /** @todo */buf->getBitDouble();
    }
    height = buf->getBitDouble();/* Text height BD 40 Undocumented */
    textgen = buf->getBitShort(); /* Attachment BS 71 Similar to justification; */
    /* Drawing dir BS 72 Left to right, etc.; see DXF doc */
    dint16 draw_dir = buf->getBitShort();
    DRW_UNUSED(draw_dir);
    /* Extents ht BD Undocumented and not present in DXF or entget */
    double ext_ht = buf->getBitDouble();
    DRW_UNUSED(ext_ht);
    /* Extents wid BD Undocumented and not present in DXF or entget The extents
    rectangle, when rotated the same as the text, fits the actual text image on
    the screen (altough we've seen it include an extra row of text in height). */
    double ext_wid = buf->getBitDouble();
    DRW_UNUSED(ext_wid);
    /* Text TV 1 All text in one long string (without '\n's 3 for line wrapping).
    ACAD seems to add braces ({ }) and backslash-P's to indicate paragraphs
    based on the "\r\n"'s found in the imported file. But, all the text is in
    this one long string -- not broken into 1- and 3-groups as in DXF and
    entget. ACAD's entget breaks this string into 250-char pieces (not 255 as
    doc'd) – even if it's mid-word. The 1-group always gets the tag end;
    therefore, the 3's are always 250 chars long. */
    text = buf->getVariableUtf8Text(); /* Text value TV 1 */
    if (version > DRW::AC1014) {//2000+
        buf->getBitShort();/* Linespacing Style BS 73 */
        buf->getBitDouble();/* Linespacing Factor BD 44 */
        buf->getBit();/* Unknown bit B */
    }
    if (version > DRW::AC1015) {//2004+
        /* Background flags BL 0 = no background, 1 = background fill, 2 =background
        fill with drawing fill color. */
        dint32 bk_flags = buf->getBitLong(); /** @todo add to DRW_MText */
        if ( bk_flags == 1 )
        {
            /* Background scale factor BL Present if background flags = 1, default = 1.5*/
            buf->getBitLong();
            /* Background color CMC Present if background flags = 1 */
            /** @todo buf->getCMC */
            /* Background transparency BL Present if background flags = 1 */
            buf->getBitLong();
        }
    }

    /* Common Entity Handle Data */
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
    if (!ret)
        return ret;

    styleH = buf->getHandle(); /* H 7 STYLE (hard pointer) */
    DBG("text style Handle: "); DBG(styleH.code); DBG(".");
    DBG(styleH.size); DBG("."); DBG(styleH.ref); DBG("\n");

    /* CRC X --- */
    return buf->isGood();
}

void DRW_MText::updateAngle(){
    if (haveXAxis) {
            angle = atan2(secPoint.y, secPoint.x)*180/M_PI;
    }
}

void DRW_Polyline::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 70:
        flags = reader->getInt32();
        break;
    case 40:
        defstawidth = reader->getDouble();
        break;
    case 41:
        defendwidth = reader->getDouble();
        break;
    case 71:
        vertexcount = reader->getInt32();
        break;
    case 72:
        facecount = reader->getInt32();
        break;
    case 73:
        smoothM = reader->getInt32();
        break;
    case 74:
        smoothN = reader->getInt32();
        break;
    case 75:
        curvetype = reader->getInt32();
        break;
    default:
        DRW_Point::parseCode(code, reader);
        break;
    }
}

void DRW_Vertex::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 70:
        flags = reader->getInt32();
        break;
    case 40:
        stawidth = reader->getDouble();
        break;
    case 41:
        endwidth = reader->getDouble();
        break;
    case 42:
        bulge = reader->getDouble();
        break;
    case 50:
        tgdir = reader->getDouble();
        break;
    case 71:
        vindex1 = reader->getInt32();
        break;
    case 72:
        vindex2 = reader->getInt32();
        break;
    case 73:
        vindex3 = reader->getInt32();
        break;
    case 74:
        vindex4 = reader->getInt32();
        break;
    case 91:
        identifier = reader->getInt32();
        break;
    default:
        DRW_Point::parseCode(code, reader);
        break;
    }
}

void DRW_Hatch::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 2:
        name = reader->getUtf8String();
        break;
    case 70:
        solid = reader->getInt32();
        break;
    case 71:
        associative = reader->getInt32();
        break;
    case 72:        /*edge type*/
        if (ispol){ //if is polyline is a as_bulge flag
            break;
        } else if (reader->getInt32() == 1){ //line
            addLine();
        } else if (reader->getInt32() == 2){ //arc
            addArc();
        } else if (reader->getInt32() == 3){ //elliptic arc
            addEllipse();
        } else if (reader->getInt32() == 4){ //spline
            addSpline();
        }
        break;
    case 10:
        if (pt) pt->basePoint.x = reader->getDouble();
        else if (pline) {
            plvert = pline->addVertex();
            plvert->x = reader->getDouble();
        }
        break;
    case 20:
        if (pt) pt->basePoint.y = reader->getDouble();
        else if (plvert) plvert ->y = reader->getDouble();
        break;
    case 11:
        if (line) line->secPoint.x = reader->getDouble();
        else if (ellipse) ellipse->secPoint.x = reader->getDouble();
        break;
    case 21:
        if (line) line->secPoint.y = reader->getDouble();
        else if (ellipse) ellipse->secPoint.y = reader->getDouble();
        break;
    case 40:
        if (arc) arc->radious = reader->getDouble();
        else if (ellipse) ellipse->ratio = reader->getDouble();
        break;
    case 41:
        scale = reader->getDouble();
        break;
    case 42:
        if (plvert) plvert ->bulge = reader->getDouble();
        break;
    case 50:
        if (arc) arc->staangle = reader->getDouble()/ARAD;
        else if (ellipse) ellipse->staparam = reader->getDouble()/ARAD;
        break;
    case 51:
        if (arc) arc->endangle = reader->getDouble()/ARAD;
        else if (ellipse) ellipse->endparam = reader->getDouble()/ARAD;
        break;
    case 52:
        angle = reader->getDouble();
        break;
    case 73:
        if (arc) arc->isccw = reader->getInt32();
        else if (pline) pline->flags = reader->getInt32();
        break;
    case 75:
        hstyle = reader->getInt32();
        break;
    case 76:
        hpattern = reader->getInt32();
        break;
    case 77:
        doubleflag = reader->getInt32();
        break;
    case 78:
        deflines = reader->getInt32();
        break;
    case 91:
        loopsnum = reader->getInt32();
        looplist.reserve(loopsnum);
        break;
    case 92:
        loop = new DRW_HatchLoop(reader->getInt32());
        looplist.push_back(loop);
        if (reader->getInt32() & 2) {
            ispol = true;
            clearEntities();
            pline = new DRW_LWPolyline;
            loop->objlist.push_back(pline);
        } else ispol = false;
        break;
    case 93:
        if (pline) pline->vertexnum = reader->getInt32();
        else loop->numedges = reader->getInt32();//aqui reserve
        break;
    case 98: //seed points ??
        clearEntities();
        break;
    default:
        DRW_Point::parseCode(code, reader);
        break;
    }
}

void DRW_Spline::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 210:
        ex = reader->getDouble();
        break;
    case 220:
        ey = reader->getDouble();
        break;
    case 230:
        ez = reader->getDouble();
        break;
    case 12:
        tgsx = reader->getDouble();
        break;
    case 22:
        tgsy = reader->getDouble();
        break;
    case 32:
        tgsz = reader->getDouble();
        break;
    case 13:
        tgex = reader->getDouble();
        break;
    case 23:
        tgey = reader->getDouble();
        break;
    case 33:
        tgez = reader->getDouble();
        break;
    case 70:
        flags = reader->getInt32();
        break;
    case 71:
        degree = reader->getInt32();
        break;
    case 72:
        nknots = reader->getInt32();
        break;
    case 73:
        ncontrol = reader->getInt32();
        break;
    case 74:
        nfit = reader->getInt32();
        break;
    case 42:
        tolknot = reader->getDouble();
        break;
    case 43:
        tolcontrol = reader->getDouble();
        break;
    case 44:
        tolfit = reader->getDouble();
        break;
    case 10: {
        controlpoint = new DRW_Coord();
        controllist.push_back(controlpoint);
        controlpoint->x = reader->getDouble();
        break; }
    case 20:
        if(controlpoint != NULL)
            controlpoint->y = reader->getDouble();
        break;
    case 30:
        if(controlpoint != NULL)
            controlpoint->z = reader->getDouble();
        break;
    case 11: {
        fitpoint = new DRW_Coord();
        fitlist.push_back(fitpoint);
        fitpoint->x = reader->getDouble();
        break; }
    case 21:
        if(fitpoint != NULL)
            fitpoint->y = reader->getDouble();
        break;
    case 31:
        if(fitpoint != NULL)
            fitpoint->z = reader->getDouble();
        break;
    case 40:
        knotslist.push_back(reader->getDouble());
        break;
//    case 41:
//        break;
    default:
        DRW_Entity::parseCode(code, reader);
        break;
    }
}

void DRW_Image::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 12:
        vx = reader->getDouble();
        break;
    case 22:
        vy = reader->getDouble();
        break;
    case 32:
        vz = reader->getDouble();
        break;
    case 13:
        sizeu = reader->getDouble();
        break;
    case 23:
        sizev = reader->getDouble();
        break;
    case 340:
        ref = reader->getString();
        break;
    case 280:
        clip = reader->getInt32();
        break;
    case 281:
        brightness = reader->getInt32();
        break;
    case 282:
        contrast = reader->getInt32();
        break;
    case 283:
        fade = reader->getInt32();
        break;
    default:
        DRW_Line::parseCode(code, reader);
        break;
    }
}

void DRW_Dimension::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 1:
        text = reader->getUtf8String();
        break;
    case 2:
        name = reader->getString();
        break;
    case 3:
        style = reader->getUtf8String();
        break;
    case 70:
        type = reader->getInt32();
        break;
    case 71:
        align = reader->getInt32();
        break;
    case 72:
        linesty = reader->getInt32();
        break;
    case 10:
        defPoint.x = reader->getDouble();
        break;
    case 20:
        defPoint.y = reader->getDouble();
        break;
    case 30:
        defPoint.z = reader->getDouble();
        break;
    case 11:
        textPoint.x = reader->getDouble();
        break;
    case 21:
        textPoint.y = reader->getDouble();
        break;
    case 31:
        textPoint.z = reader->getDouble();
        break;
    case 12:
        clonePoint.x = reader->getDouble();
        break;
    case 22:
        clonePoint.y = reader->getDouble();
        break;
    case 32:
        clonePoint.z = reader->getDouble();
        break;
    case 13:
        def1.x = reader->getDouble();
        break;
    case 23:
        def1.y = reader->getDouble();
        break;
    case 33:
        def1.z = reader->getDouble();
        break;
    case 14:
        def2.x = reader->getDouble();
        break;
    case 24:
        def2.y = reader->getDouble();
        break;
    case 34:
        def2.z = reader->getDouble();
        break;
    case 15:
        circlePoint.x = reader->getDouble();
        break;
    case 25:
        circlePoint.y = reader->getDouble();
        break;
    case 35:
        circlePoint.z = reader->getDouble();
        break;
    case 16:
        arcPoint.x = reader->getDouble();
        break;
    case 26:
        arcPoint.y = reader->getDouble();
        break;
    case 36:
        arcPoint.z = reader->getDouble();
        break;
    case 41:
        linefactor = reader->getDouble();
        break;
    case 53:
        rot = reader->getDouble();
        break;
    case 50:
        angle = reader->getDouble();
        break;
    case 52:
        oblique = reader->getDouble();
        break;
    case 40:
        length = reader->getDouble();
        break;
/*    case 51:
        hdir = reader->getDouble();
        break;*/
    default:
        DRW_Entity::parseCode(code, reader);
        break;
    }
}

void DRW_Leader::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 3:
        style = reader->getUtf8String();
        break;
    case 71:
        arrow = reader->getInt32();
        break;
    case 72:
        leadertype = reader->getInt32();
        break;
    case 73:
        flag = reader->getInt32();
        break;
    case 74:
        hookline = reader->getInt32();
        break;
    case 75:
        hookflag = reader->getInt32();
        break;
    case 76:
        vertnum = reader->getInt32();
        break;
    case 77:
        coloruse = reader->getInt32();
        break;
    case 40:
        textheight = reader->getDouble();
        break;
    case 41:
        textwidth = reader->getDouble();
        break;
    case 10: {
        vertexpoint = new DRW_Coord();
        vertexlist.push_back(vertexpoint);
        vertexpoint->x = reader->getDouble();
        break; }
    case 20:
        if(vertexpoint != NULL)
            vertexpoint->y = reader->getDouble();
        break;
    case 30:
        if(vertexpoint != NULL)
            vertexpoint->z = reader->getDouble();
        break;
    case 340:
        handle = reader->getString();
        break;
    case 210:
        extrusionPoint.x = reader->getDouble();
        break;
    case 220:
        extrusionPoint.y = reader->getDouble();
        break;
    case 230:
        extrusionPoint.z = reader->getDouble();
        break;
    case 211:
        horizdir.x = reader->getDouble();
        break;
    case 221:
        horizdir.y = reader->getDouble();
        break;
    case 231:
        horizdir.z = reader->getDouble();
        break;
    case 212:
        offsetblock.x = reader->getDouble();
        break;
    case 222:
        offsetblock.y = reader->getDouble();
        break;
    case 232:
        offsetblock.z = reader->getDouble();
        break;
    case 213:
        offsettext.x = reader->getDouble();
        break;
    case 223:
        offsettext.y = reader->getDouble();
        break;
    case 233:
        offsettext.z = reader->getDouble();
        break;
    default:
        DRW_Entity::parseCode(code, reader);
        break;
    }
}

void DRW_Viewport::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 40:
        pswidth = reader->getDouble();
        break;
    case 41:
        psheight = reader->getDouble();
        break;
    case 68:
        vpstatus = reader->getInt32();
        break;
    case 69:
        vpID = reader->getInt32();
        break;
    case 12: {
        centerPX = reader->getDouble();
        break; }
    case 22:
        centerPY = reader->getDouble();
        break;
    default:
        DRW_Point::parseCode(code, reader);
        break;
    }
}
//ex 22 dec 34
bool DRW_Viewport::parseDwg(DRW::Version version, dwgBuffer *buf){
    bool ret = DRW_Entity::parseDwg(version, buf);
    if (!ret)
        return ret;
    DBG("\n***************************** parsing viewport *****************************************\n");

    basePoint.x = buf->getBitDouble();
    basePoint.y = buf->getBitDouble();
    basePoint.z = buf->getBitDouble();
    DBG("center X: "); DBG(basePoint.x); DBG(", Y: "); DBG(basePoint.y); DBG(", Z: "); DBG(basePoint.z); DBG("\n");
    pswidth = buf->getBitDouble();
    psheight = buf->getBitDouble();
    //TODO: complete in dxf
    if (version > DRW::AC1014) {//2000+
        viewTarget.x = buf->getBitDouble();
        viewTarget.y = buf->getBitDouble();
        viewTarget.z = buf->getBitDouble();
        DBG("view Target X: "); DBG(viewTarget.x); DBG(", Y: "); DBG(viewTarget.y); DBG(", Z: "); DBG(viewTarget.z); DBG("\n");
        viewDir.x = buf->getBitDouble();
        viewDir.y = buf->getBitDouble();
        viewDir.z = buf->getBitDouble();
        DBG("view direction X: "); DBG(viewDir.x); DBG(", Y: "); DBG(viewDir.y); DBG(", Z: "); DBG(viewDir.z); DBG("\n");
        twistAngle = buf->getBitDouble();
        DBG("twist Angle: "); DBG(twistAngle);DBG("\n");
        viewHeight = buf->getBitDouble();
        DBG("view Height: "); DBG(viewHeight);
        viewLength = buf->getBitDouble();
        DBG(" view Length: "); DBG(viewLength);DBG("\n");
        frontClip = buf->getBitDouble();
        DBG("front Clip: "); DBG(frontClip);
        backClip = buf->getBitDouble();
        DBG(" back Clip: "); DBG(backClip);DBG("\n");
        snapAngle = buf->getBitDouble();
        DBG("twist Angle: "); DBG(twistAngle);DBG("\n");
        centerPX = buf->getRawDouble();
        centerPY = buf->getRawDouble();
        DBG("view center X: "); DBG(twistAngle); DBG(", Y: "); DBG(twistAngle);DBG("\n");
    }
    ret = DRW_Entity::parseDwgEntHandle(version, buf);
//    X handleAssoc;   //X
    if (!ret)
        return ret;
//    RS crc;   //RS */
    return buf->isGood();
}
