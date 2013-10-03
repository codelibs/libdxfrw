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

#ifndef LIBDWGR_H
#define LIBDWGR_H

#include <string>
//#include <deque>
#include "drw_entities.h"
#include "drw_objects.h"
#include "drw_classes.h"
#include "drw_interface.h"

#ifdef DRWG_DBG
#include <iostream> //for debug
//#define DBG(a) std::cerr << a
#include <QTextStream>
extern QTextStream readLog;
#define DBG(a) readLog << a
#define DBGH(a) readLog.setFieldWidth(2); readLog.setPadChar('0'); readLog.setIntegerBase(16); \
    readLog << a; readLog.setIntegerBase(10); readLog.setFieldWidth(0); readLog.setPadChar(' ');
#define DBGB(a) readLog.setFieldWidth(8); readLog.setPadChar('0'); readLog.setIntegerBase(2); \
    readLog << a; readLog.setIntegerBase(10); readLog.setFieldWidth(0); readLog.setPadChar(' ');
#else
#define DBG(a)
#define DBGH(a)
#define DBGB(a)
#endif


class dwgReader;
//class dxfWriter;

/*class dwgObject {
public:
    enum Type {
        NONE,
        OBJ,
        ENT
    };
    dwgObject(){
        type = NONE;
        ent = NULL;
        obj = NULL;
    }
    dwgObject(dwgObject::Type t){
        type = t;
        ent = NULL;
        obj = NULL;
    }
    ~dwgObject(){
        if (ent != NULL)
            delete ent;
        if (obj != NULL)
            delete obj;
    }

    DRW_Entity *ent;
    DRW_TableEntry *obj;
    dwgObject::Type type;
};*/

class dwgR {
public:
    dwgR(const char* name);
    ~dwgR();
    //read: return true if all ok
    bool read(DRW_Interface *interface_, bool ext);
    DRW::Version getVersion(){return version;}
    DRW::error getError(){return error;}
//    void setError(DRW::error err){error = err;}

//    void addDwgObject(dwgObject *obj);

private:
    bool processDwg();
//    void processLine(const DRW_Line *line);

public:
/*    bool processHeader();
    bool processTables();
    bool processBlocks();
    bool processBlock();
    bool processEntities(bool isblock);
    bool processObjects();

    bool processLType();
    bool processLayer();
    bool processDimStyle();
    bool processTextStyle();
    bool processVports();

    bool processPoint();
    bool processRay();
    bool processXline();
    bool processCircle();
    bool processArc();
    bool processEllipse();
    bool processTrace();
    bool processSolid();
    bool processInsert();
    bool processLWPolyline();
    bool processPolyline();
    bool processVertex(DRW_Polyline* pl);
    bool processText();
    bool processMText();
    bool processHatch();
    bool processSpline();
    bool process3dface();
    bool processViewport();
    bool processImage();
    bool processImageDef();
    bool processDimension();
    bool processLeader();*/

private:
    DRW::Version version;
    DRW::error error;
    std::string fileName;
    bool applyExt; /*apply extrusion in entities to conv in 2D?*/
    std::string codePage;
    DRW_Interface *iface;
    dwgReader *reader;
//    dwgWriter *writer;


/*    DRW_Header header;
//    int section;
    string nextentity;
    int entCount;
    bool wlayer0;
    bool dimstyleStd;
    bool writingBlock;
    std::map<std::string,int> blockMap;
    std::vector<DRW_ImageDef*> imageDef;*/  /*!< imageDef list */

//    int currHandle;

};

#endif // LIBDWGR_H
