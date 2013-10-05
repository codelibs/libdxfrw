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
#include "drw_dbg.h"

DRW_dbg *DRW_dbg::instance= NULL;

DRW_dbg *DRW_dbg::getInstance(){
    if (instance == NULL){
        instance = new DRW_dbg;
    }
    return instance;
}

void DRW_dbg::print(std::string s){
    std::cerr << s;
}

void DRW_dbg::print(int i){
    std::cerr << i;
}

void DRW_dbg::printH(int i){
}

void DRW_dbg::printB(int i){
}


