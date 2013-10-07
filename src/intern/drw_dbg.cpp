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
#include <iomanip>
#include "drw_dbg.h"

DRW_dbg *DRW_dbg::instance= NULL;

/*********private clases*************/
class print_none {
public:
    virtual void print(std::string s){(void)s;}
    virtual void print(double d){(void)d;}
    virtual void printH(int i){(void)i;}
    virtual void printB(int i){(void)i;}
    print_none(){}
};

class print_debug : public print_none {
public:
    virtual void print(std::string s);
    virtual void print(double d);
    virtual void printH(int i);
    virtual void printB(int i);
    print_debug();
private:
    std::ios_base::fmtflags flags;
};

/********* debug class *************/
DRW_dbg *DRW_dbg::getInstance(){
    if (instance == NULL){
        instance = new DRW_dbg;
    }
    return instance;
}

DRW_dbg::DRW_dbg(){
    level = NONE;
    prClass = new print_none;
    flags = std::cerr.flags();
}

void DRW_dbg::setLevel(LEVEL lvl){
    level = lvl;
    delete prClass;
    switch (level){
    case DEBUG:
        prClass = new print_debug;
        break;
    default:
        prClass = new print_none;
    }
}

void DRW_dbg::print(std::string s){
    prClass->print(s);
}

void DRW_dbg::print(double d){
    prClass->print(d);
}

void DRW_dbg::printH(int i){
    prClass->printH(i);
}

void DRW_dbg::printB(int i){
    prClass->printB(i);
}

print_debug::print_debug(){
    flags = std::cerr.flags();
}

void print_debug::print(std::string s){
    std::cerr << s;
}

void print_debug::print(double d){
    std::cerr << d;
}

void print_debug::printH(int i){
    std::cerr << "0x" << std::setw(2) << std::setfill('0');
    std::cerr << std::hex << i;
    std::cerr.flags(flags);
}

void print_debug::printB(int i){
    std::cerr << std::setw(8) << std::setfill('0');
    std::cerr << std::setbase(2) << i;
    std::cerr.flags(flags);
}
