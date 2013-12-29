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

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "dwgreader.h"
#include "drw_textcodec.h"
#include "drw_dbg.h"

dwgReader::~dwgReader(){
    for (std::map<int, DRW_LType*>::iterator it=ltypemap.begin(); it!=ltypemap.end(); ++it)
        delete(it->second);
    for (std::map<int, DRW_Layer*>::iterator it=layermap.begin(); it!=layermap.end(); ++it)
        delete(it->second);
/*    for (std::map<int, DRW_Block_Record*>::iterator it=block_recmap.begin(); it!=block_recmap.end(); ++it)
        delete(it->second);
    for (std::map<int, DRW_Block*>::iterator it=blockmap.begin(); it!=blockmap.end(); ++it)
        delete(it->second);*/
    for (std::map<int, DRW_Textstyle*>::iterator it=stylemap.begin(); it!=stylemap.end(); ++it)
        delete(it->second);
    for (std::map<int, DRW_Dimstyle*>::iterator it=dimstylemap.begin(); it!=dimstylemap.end(); ++it)
        delete(it->second);
/*    for (std::map<int, DRW_Vport*>::iterator it=vportmap.begin(); it!=vportmap.end(); ++it)
        delete(it->second);*/
    for (std::map<int, DRW_Class*>::iterator it=classesmap.begin(); it!=classesmap.end(); ++it)
        delete(it->second);

    delete buf;
}

void dwgReader::parseAttribs(DRW_Entity* e){
    if (e != NULL){
        duint32 ltref =e->lTypeH.ref;
        duint32 lyref =e->layerH.ref;
        std::map<int, DRW_LType*>::iterator lt_it = ltypemap.find(ltref);
        if (lt_it != ltypemap.end()){
            e->lineType = (lt_it->second)->name;
        }
        std::map<int, DRW_Layer*>::iterator ly_it = layermap.find(lyref);
        if (ly_it != layermap.end()){
            e->layer = (ly_it->second)->name;
        }
    }
}

std::string dwgReader::findTableName(DRW::TTYPE table, dint32 handle){
    std::string name;
    switch (table){
    case DRW::STYLE:{
        std::map<int, DRW_Textstyle*>::iterator st_it = stylemap.find(handle);
        if (st_it != stylemap.end())
            name = (st_it->second)->name;
        break;}
    case DRW::DIMSTYLE:{
        std::map<int, DRW_Dimstyle*>::iterator ds_it = dimstylemap.find(handle);
        if (ds_it != dimstylemap.end())
            name = (ds_it->second)->name;
        break;}
/*    case DRW::BLOCK_RECORD:{ //use DRW_Block because name are more correct
        std::map<int, DRW_Block*>::iterator bk_it = blockmap.find(handle);
        if (bk_it != blockmap.end())
            name = (bk_it->second)->name;
        break;}
    case DRW::VPORT:{
        std::map<int, DRW_Vport*>::iterator vp_it = vportmap.find(handle);
        if (vp_it != vportmap.end())
            name = (vp_it->second)->name;
        break;}*/
    case DRW::LAYER:{
        std::map<int, DRW_Layer*>::iterator ly_it = layermap.find(handle);
        if (ly_it != layermap.end())
            name = (ly_it->second)->name;
        break;}
    case DRW::LTYPE:{
        std::map<int, DRW_LType*>::iterator lt_it = ltypemap.find(handle);
        if (lt_it != ltypemap.end())
            name = (lt_it->second)->name;
        break;}
    default:
        break;
    }
    return name;
}

bool DRW_ObjControl::parseDwg(DRW::Version version, dwgBuffer *buf){
int unkData=0;
    bool ret = DRW_TableEntry::parseDwg(version, buf);
    DBG("\n***************************** parsing object control entry *********************************************\n");
    if (!ret)
        return ret;
    //last parsed is: XDic Missing Flag 2004+
    int numEntries = buf->getBitLong();
    DRW_DBG(" num entries: "); DRW_DBG(numEntries); DBG("\n");
    DRW_DBG("Remaining bytes: "); DRW_DBG(buf->numRemainingBytes()); DBG("\n");

    if (oType == 68 && version== DRW::AC1015){//V2000 dimstyle seems have one unknown byte hard handle counter??
        unkData = buf->getRawChar8();
        DRW_DBG(" unknown v2000 byte: "); DRW_DBG( unkData); DRW_DBG("\n");
//        DRW_DBG(" unknown byte: "); DRW_DBG( buf->getRawChar8()); DRW_DBG("\n");
    }

    dwgHandle objectH = buf->getHandle();
    DRW_DBG(" NULL Handle: "); DRW_DBG(objectH.code); DBG(".");
    DRW_DBG(objectH.size); DBG("."); DRW_DBGH(objectH.ref); DBG("\n");
    DRW_DBG("Remaining bytes: "); DRW_DBG(buf->numRemainingBytes()); DBG("\n");

//    if (oType != 68){//dimstyle seems not have XDicObjH or NULL handle
        dwgHandle XDicObjH = buf->getHandle();
        DRW_DBG(" XDicObj control Handle: "); DRW_DBG(XDicObjH.code); DBG(".");
        DRW_DBG(XDicObjH.size); DBG("."); DRW_DBGH(XDicObjH.ref); DBG("\n");
        DRW_DBG("Remaining bytes: "); DRW_DBG(buf->numRemainingBytes()); DBG("\n");
//    }
//add 2 for modelspace, paperspace blocks & bylayer, byblock linetypes
    numEntries = ((oType == 48) || (oType == 56)) ? (numEntries +2) : numEntries;

    for (int i =0; i< numEntries; i++){
        objectH = buf->getOffsetHandle(handle);
        if (objectH.ref != 0) //in vports R14  I found some NULL handles
            hadlesList.push_back (objectH.ref);
        DRW_DBG(" objectH Handle: "); DRW_DBG(objectH.code); DBG(".");
        DRW_DBG(objectH.size); DBG("."); DRW_DBGH(objectH.ref); DBG("\n");
        DRW_DBG("Remaining bytes: "); DRW_DBG(buf->numRemainingBytes()); DBG("\n");
    }
    //V2000 dimstyle seems have one unknown byte, hard handle counter??
    for (int i =0; i< unkData; i++){
        objectH = buf->getOffsetHandle(handle);
        DRW_DBG(" unknown Handle: "); DRW_DBG(objectH.code); DBG(".");
        DRW_DBG(objectH.size); DBG("."); DRW_DBGH(objectH.ref); DBG("\n");
        DRW_DBG("Remaining bytes: "); DRW_DBG(buf->numRemainingBytes()); DBG("\n");
    }
    return buf->isGood();
}

