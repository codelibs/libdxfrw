/******************************************************************************
**  dwg2dxf - Program to convert dwg/dxf to dxf(ascii & binary)              **
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
#include <algorithm>
#include <regex>
#include "dx_iface.h"
#include "libdwgr.h"
#include "libdxfrw.h"

/**
 * Strip MTEXT formatting codes and return plain text.
 * MTEXT uses various formatting codes like:
 *   \P       - paragraph break (newline)
 *   \\       - literal backslash
 *   \{       - literal opening brace
 *   \}       - literal closing brace
 *   {\fFont|...;text} - font specification
 *   \L, \l   - underline on/off
 *   \O, \o   - overline on/off
 *   \K, \k   - strikethrough on/off
 *   \S...^...; - stacked fraction
 *   \H...x;  - height change
 *   \W...x;  - width change
 *   \Q...;   - oblique angle
 *   \T...;   - tracking
 *   \A...;   - alignment
 *   \C...;   - color
 *   { }      - grouping (removed)
 */
static std::string stripMTextFormatting(const std::string& text) {
    std::string result;
    result.reserve(text.size());

    size_t i = 0;
    while (i < text.size()) {
        // Check for escape character (backslash or yen symbol)
        // Yen symbol in UTF-8 is 0xC2 0xA5
        bool isEscape = false;
        size_t escapeLen = 0;

        if (text[i] == '\\') {
            isEscape = true;
            escapeLen = 1;
        } else if (i + 1 < text.size() &&
                   static_cast<unsigned char>(text[i]) == 0xC2 &&
                   static_cast<unsigned char>(text[i + 1]) == 0xA5) {
            // UTF-8 encoded yen symbol (¥)
            isEscape = true;
            escapeLen = 2;
        }

        if (isEscape && i + escapeLen < text.size()) {
            char next = text[i + escapeLen];
            switch (next) {
                case 'P':
                case 'p':
                    // Paragraph break -> newline
                    result += '\n';
                    i += escapeLen + 1;
                    break;
                case '\\':
                    // Escaped backslash
                    result += '\\';
                    i += escapeLen + 1;
                    break;
                case '{':
                    result += '{';
                    i += escapeLen + 1;
                    break;
                case '}':
                    result += '}';
                    i += escapeLen + 1;
                    break;
                case 'L':
                case 'l':
                case 'O':
                case 'o':
                case 'K':
                case 'k':
                    // Underline/overline/strikethrough toggles - skip
                    i += escapeLen + 1;
                    break;
                case 'S': {
                    // Stacked fraction: \Snum^denom; or \Snum/denom; or \Snum#denom;
                    i += escapeLen + 1;
                    std::string num, denom;
                    while (i < text.size() && text[i] != '^' && text[i] != '/' && text[i] != '#' && text[i] != ';') {
                        num += text[i++];
                    }
                    if (i < text.size() && (text[i] == '^' || text[i] == '/' || text[i] == '#')) {
                        i++; // skip separator
                        while (i < text.size() && text[i] != ';') {
                            denom += text[i++];
                        }
                    }
                    if (i < text.size() && text[i] == ';') i++; // skip semicolon
                    if (!num.empty() && !denom.empty()) {
                        result += num + "/" + denom;
                    } else {
                        result += num + denom;
                    }
                    break;
                }
                case 'H':
                case 'W':
                case 'Q':
                case 'T':
                case 'A':
                case 'C':
                case 'h':
                case 'w':
                case 'q':
                case 't':
                case 'a':
                case 'c':
                    // Format codes with value: \Xvalue; - skip to semicolon
                    i += escapeLen + 1;
                    while (i < text.size() && text[i] != ';') {
                        i++;
                    }
                    if (i < text.size() && text[i] == ';') i++;
                    break;
                case 'F':
                case 'f':
                    // Font specification: \fFontName|options; - skip to semicolon
                    i += escapeLen + 1;
                    while (i < text.size() && text[i] != ';') {
                        i++;
                    }
                    if (i < text.size() && text[i] == ';') i++;
                    break;
                default:
                    // Unknown escape - keep character as is
                    result += text[i];
                    i++;
                    break;
            }
        } else if (text[i] == '{' || text[i] == '}') {
            // Skip grouping braces
            i++;
        } else {
            result += text[i];
            i++;
        }
    }

    return result;
}

bool dx_iface::printText(const std::string& fileI, dx_data *fData){
    unsigned int found = fileI.find_last_of(".");
    std::string fileExt = fileI.substr(found+1);
    std::transform(fileExt.begin(), fileExt.end(),fileExt.begin(), ::toupper);
    cData = fData;
    currentBlock = cData->mBlock;

    bool success = false;
    if (fileExt == "DXF"){
        //loads dxf
        dxfRW* dxf = new dxfRW(fileI.c_str());
        success = dxf->read(this, false);
        delete dxf;
    } else if (fileExt == "DWG"){
        //loads dwg
        dwgR* dwg = new dwgR(fileI.c_str());
        success = dwg->read(this, false);
        delete dwg;
    }

    if (success) {
        for (std::list<DRW_Entity*>::const_iterator it =
                cData->mBlock->ent.begin(); it != cData->mBlock->ent.end();
                ++it) {
            DRW_Entity* e = *it;
            switch (e->eType) {
            case DRW::TEXT:
                std::cout << static_cast<DRW_Text*>(e)->text << std::endl;
                break;
            case DRW::MTEXT: {
                std::string mtextContent = static_cast<DRW_MText*>(e)->text;
                std::string plainText = stripMTextFormatting(mtextContent);
                if (!plainText.empty()) {
                    std::cout << plainText << std::endl;
                }
                break;
            }
            case DRW::DIMLINEAR:
            case DRW::DIMALIGNED:
            case DRW::DIMANGULAR:
            case DRW::DIMANGULAR3P:
            case DRW::DIMRADIAL:
            case DRW::DIMDIAMETRIC:
            case DRW::DIMORDINATE: {
                DRW_Dimension* dim = static_cast<DRW_Dimension*>(e);
                std::string dimText = dim->getText();
                if (!dimText.empty()) {
                    std::cout << dimText << std::endl;
                }
                break;
            }
            default:
                break;
            }
        }
    }
    return success;
}

void dx_iface::writeEntity(DRW_Entity* e){
    switch (e->eType) {
    case DRW::POINT:
        dxfW->writePoint(static_cast<DRW_Point*>(e));
        break;
    case DRW::LINE:
        dxfW->writeLine(static_cast<DRW_Line*>(e));
        break;
    case DRW::CIRCLE:
        dxfW->writeCircle(static_cast<DRW_Circle*>(e));
        break;
    case DRW::ARC:
        dxfW->writeArc(static_cast<DRW_Arc*>(e));
        break;
    case DRW::SOLID:
        dxfW->writeSolid(static_cast<DRW_Solid*>(e));
        break;
    case DRW::ELLIPSE:
        dxfW->writeEllipse(static_cast<DRW_Ellipse*>(e));
        break;
    case DRW::LWPOLYLINE:
        dxfW->writeLWPolyline(static_cast<DRW_LWPolyline*>(e));
        break;
    case DRW::POLYLINE:
        dxfW->writePolyline(static_cast<DRW_Polyline*>(e));
        break;
    case DRW::SPLINE:
        dxfW->writeSpline(static_cast<DRW_Spline*>(e));
        break;
//    case RS2::EntitySplinePoints:
//        writeSplinePoints(static_cast<DRW_Point*>(e));
//        break;
//    case RS2::EntityVertex:
//        break;
    case DRW::INSERT:
        dxfW->writeInsert(static_cast<DRW_Insert*>(e));
        break;
    case DRW::MTEXT:
        dxfW->writeMText(static_cast<DRW_MText*>(e));
        break;
    case DRW::TEXT:
        dxfW->writeText(static_cast<DRW_Text*>(e));
        break;
    case DRW::DIMLINEAR:
    case DRW::DIMALIGNED:
    case DRW::DIMANGULAR:
    case DRW::DIMANGULAR3P:
    case DRW::DIMRADIAL:
    case DRW::DIMDIAMETRIC:
    case DRW::DIMORDINATE:
        dxfW->writeDimension(static_cast<DRW_Dimension*>(e));
        break;
    case DRW::LEADER:
        dxfW->writeLeader(static_cast<DRW_Leader*>(e));
        break;
    case DRW::HATCH:
        dxfW->writeHatch(static_cast<DRW_Hatch*>(e));
        break;
    case DRW::IMAGE:
        dxfW->writeImage(static_cast<DRW_Image*>(e), static_cast<dx_ifaceImg*>(e)->path);
        break;
    default:
        break;
    }
}
