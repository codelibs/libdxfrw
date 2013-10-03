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

#include "drw_classes.h"
#include "dxfreader.h"
#include "dxfwriter.h"
#include "dwgbuffer.h"


void DRW_Class::parseCode(int code, dxfReader *reader){
    switch (code) {
    case 1:
        recNane = reader->getUtf8String();
        break;
    case 2:
        className = reader->getUtf8String();
        break;
    case 3:
        appName = reader->getUtf8String();
        break;
    case 90:
        proxyCapabilities = reader->getInt32();
        break;
    case 280:
        proxyFlag = reader->getInt32();
        break;
    case 281:
        entityFlag = reader->getInt32();
        break;
    default:
        break;
    }
}

bool DRW_Class::parseDwg(DRW::Version version, dwgBuffer *buf){
    DBG("\n***************************** parsing Class *********************************************\n");

    classNum = buf->getBitShort();
    proxyCapabilities = buf->getBitShort(); //in dwg specs says "version"

    if (version > DRW::AC1018) {//2007+
        appName = buf->getVariableText();
        className = buf->getVariableText();
        recNane = buf->getVariableText();
    } else {//2004-
        appName = buf->getVariableUtf8Text();
        className = buf->getVariableUtf8Text();
        recNane = buf->getVariableUtf8Text();
    }
    DBG("record name: "); DBG(recNane.c_str()); DBG("\n");
    DBG("class name: "); DBG(className.c_str()); DBG("\n");
    DBG("app name: "); DBG(appName.c_str()); DBG("\n");
    proxyFlag = buf->getBit(); //in dwg says wasazombie
    entityFlag = buf->getBitShort();
    entityFlag = entityFlag == 0x1F2 ? 1: 0;

    DBG("proxy capabilities: "); DBG(proxyCapabilities);
    DBG(", proxy flag: "); DBG(proxyFlag);
    DBG(", entity flag: "); DBGH(entityFlag); DBG("\n");

    if (version > DRW::AC1018) {//2007+
//TODO: complete it
    }
    toDwgType();
    return buf->isGood();
}

void DRW_Class::write(dxfWriter *writer, DRW::Version ver){
/*RLZ: TODO implement it */
    if (ver > DRW::AC1009) {
        writer->writeString(0, "CLASS");
        writer->writeString(1, recNane);
        writer->writeString(2, className);
        writer->writeString(3, appName);
        writer->writeInt32(90, proxyCapabilities);
        writer->writeInt16(280, proxyFlag);
        writer->writeInt16(281, entityFlag);
    }
}

void DRW_Class::toDwgType(){
    if (recNane == "LWPOLYLINE")
        dwgType = 77;
    else if (recNane == "HATCH")
        dwgType = 78;
    else if (recNane == "GROUP")
        dwgType = 72;
    else if (recNane == "GROUP")
        dwgType = 72;
    else if (recNane == "LAYOUT")
        dwgType = 82;
    else
        dwgType =0;
}
