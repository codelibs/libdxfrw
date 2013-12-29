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

#ifndef DWGREADER_H
#define DWGREADER_H

#include <map>
#include <list>
//#include <unordered_map>//c++11 only
#include "drw_textcodec.h"
#include "dwgbuffer.h"
#include "../libdwgr.h"

class objHandle{
public:
    objHandle(){ handle = type = loc = 0; }
    objHandle(duint32 t, duint32 h, duint32 l){
        type = t;
        handle = h;
        loc = l;
    }
    duint32 type;
    duint32 handle;
    duint32 loc;
};

//! Class to handle dwg obj control entries
/*!
*  Class to handle dwg obj control entries
*  @author Rallaz
*/
class DRW_ObjControl : public DRW_TableEntry {
public:
    DRW_ObjControl() { reset();}

    void reset(){
    }
    bool parseDwg(DRW::Version version, dwgBuffer *buf);
    std::list<duint32>hadlesList;
};


class dwgReader {
public:
    dwgReader(std::ifstream *stream, dwgR *p){
//        filestr = stream;
        buf = new dwgBuffer(stream);
        parent = p;
        decoder.setVersion(DRW::AC1021);//default 2007 in utf8(no convert)
        decoder.setCodePage("UTF-8");
        blockCtrl=layerCtrl=styleCtrl=linetypeCtrl=viewCtrl=0;
        ucsCtrl=vportCtrl=appidCtrl=dimstyleCtrl=vpEntHeaderCtrl=0;
        nextEntLink = prevEntLink = 0;
    }
    virtual ~dwgReader();
    virtual bool readFileHeader() = 0;
    //RLZ todo    virtual bool readDwgHeader() = 0;
    virtual bool readDwgClasses() = 0;
    virtual bool readDwgObjectOffsets() = 0;
    virtual bool readDwgTables() = 0;
    virtual bool readDwgBlocks(DRW_Interface& intfa) = 0;
    virtual bool readDwgEntity(objHandle& obj, DRW_Interface& intfa) = 0;
    void parseAttribs(DRW_Entity* e);
    std::string findTableName(DRW::TTYPE table, dint32 handle);

    void setCodePage(std::string *c){decoder.setCodePage(c);}
    std::string getCodePage(){ return decoder.getCodePage();}

public:
//    std::list<objHandle>ObjectMap;
//    std::unordered_map<duint32, objHandle>ObjectMap;//c++11 only
    std::map<duint32, objHandle>ObjectMap;
    std::map<int, DRW_LType*> ltypemap;
    std::map<int, DRW_Layer*> layermap;
    std::map<int, DRW_Block_Record*> block_recmap;
//    std::map<int, DRW_Block*> blockmap;
    std::map<int, DRW_Textstyle*> stylemap;
    std::map<int, DRW_Dimstyle*> dimstylemap;
    std::map<int, DRW_Vport*> vportmap;
    duint32 currBlock;

protected:
    dwgBuffer *buf;
    dwgR *parent;
    DRW::Version version;

    duint32 seekerImageData;
//sections map
    std::map<std::string, std::pair<dint32,dint32 > >sections;
    std::map<int, DRW_Class*> classesmap;

protected:
    DRW_TextCodec decoder;
private:
//    DRW_ObjControl blockCtrl;
//    DRW_ObjControl layerCtrl;
//    DRW_ObjControl styleCtrl;
//    DRW_ObjControl linetypeCtrl;
//    DRW_ObjControl viewCtrl;
//    DRW_ObjControl ucsCtrl;
//    DRW_ObjControl vportCtrl;
//    DRW_ObjControl appidCtrl;
//    DRW_ObjControl dimstyleCtrl;
//    DRW_ObjControl vpEntHeaderCtrl;

protected:
    duint32 linetypeCtrl;
    duint32 layerCtrl;
    duint32 styleCtrl;
    duint32 dimstyleCtrl;
    duint32 appidCtrl;
    duint32 blockCtrl;
    duint32 viewCtrl;
    duint32 ucsCtrl;
    duint32 vportCtrl;
    duint32 vpEntHeaderCtrl;
    duint32 nextEntLink;
    duint32 prevEntLink;
};



#endif // DWGREADER_H
