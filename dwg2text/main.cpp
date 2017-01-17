/******************************************************************************
**  dwg2text - Program to extract text from dwg/dxf                          **
**                                                                           **
**  Copyright (C) 2015 José F. Soriano, rallazz@gmail.com                    **
**                                                                           **
**  This library is free software, licensed under the terms of the GNU       **
**  General Public License as published by the Free Software Foundation,     **
**  either version 2 of the License, or (at your option) any later version.  **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.    **
******************************************************************************/

#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "dx_iface.h"
#include "dx_data.h"

void usage(){
    std::cout << "Usage: " << std::endl;
    std::cout << "   dwg2text <input>" << std::endl << std::endl;
    std::cout << "   input      dwg or dxf file to extract text" << std::endl;
}

bool extractText(std::string inName){
    bool badState = false;
    //verify if input file exist
    std::ifstream ifs;
    ifs.open (inName.c_str(), std::ifstream::in);
    badState = ifs.fail();
    ifs.close();
    if (badState) {
        std::cout << "Error can't open " << inName << std::endl;
        return false;
    }

    dx_data fData;
    dx_iface *input = new dx_iface();
    badState = input->printText( inName, &fData );
    if (!badState) {
        std::cout << "Error reading file " << inName << std::endl;
        return false;
    }
    delete input;

    return badState;
}

int main(int argc, char *argv[]) {
    bool badState = false;
    std::string outName;
    if (argc != 2) {
        usage();
        return 1;
    }

    std::string fileName = argv[1];

    if (badState) {
        std::cout << "Bad options." << std::endl;
        usage();
        return 1;
    }

    bool ok = extractText(fileName);
    if (ok)
        return 0;
    else
        return 1;
}
