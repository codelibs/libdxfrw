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

#ifndef DRW_HEADER_H
#define DRW_HEADER_H


#include <map>
#include "drw_base.h"

class dxfReader;
class dxfWriter;

//! Class to handle header entries
/*!
*  Class to handle layer symbol table entries
*  @author Rallaz
*/
class DRW_Header {
public:
    DRW_Header() {
    }
    ~DRW_Header() {
        for (std::map<std::string,DRW_Variant*>::iterator it=vars.begin(); it!=vars.end(); ++it)
            delete it->second;
        vars.clear();
    }

    void addDouble(std::string key, double value, int code);
    void addInt(std::string key, int value, int code);
    void addStr(std::string key, std::string value, int code);
    void addCoord(std::string key, DRW_Coord value, int code);
     std::string getComments() const {return comments;}

     void parseCode(int code, dxfReader *reader);
    void write(dxfWriter *writer, DRW::Version ver);
    void addComment(std::string c);

private:
    bool getDouble(std::string key, double *varDouble);
    bool getInt(std::string key, int *varInt);
    bool getStr(std::string key, std::string *varStr);
    bool getCoord(std::string key, DRW_Coord *varStr);

public:
    std::map<std::string,DRW_Variant*> vars;
private:
    std::string comments;
    std::string name;
    DRW_Variant *curr;
    int version; //to use on read
};

#endif

// EOF

