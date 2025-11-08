/******************************************************************************
**  libDXFrw - Test Interface                                               **
**                                                                           **
**  Copyright (C) 2025 libdxfrw contributors                                **
**                                                                           **
**  This library is free software, licensed under the terms of the GNU       **
**  General Public License as published by the Free Software Foundation,     **
**  either version 2 of the License, or (at your option) any later version.  **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.    **
******************************************************************************/

#ifndef TEST_INTERFACE_H
#define TEST_INTERFACE_H

#include "drw_interface.h"
#include <vector>
#include <iostream>

class TestInterface : public DRW_Interface {
public:
    TestInterface() : pointCount(0), lineCount(0), circleCount(0), arcCount(0),
                      ellipseCount(0), lwPolylineCount(0), polylineCount(0),
                      splineCount(0), textCount(0), mtextCount(0), insertCount(0),
                      rayCount(0), xlineCount(0), traceCount(0), solidCount(0),
                      face3dCount(0), hatchCount(0), layerCount(0), ltypeCount(0) {}
    ~TestInterface() {}

    // Entity counters for validation
    int pointCount;
    int lineCount;
    int circleCount;
    int arcCount;
    int ellipseCount;
    int lwPolylineCount;
    int polylineCount;
    int splineCount;
    int textCount;
    int mtextCount;
    int insertCount;
    int rayCount;
    int xlineCount;
    int traceCount;
    int solidCount;
    int face3dCount;
    int hatchCount;

    // Table object counters
    int layerCount;
    int ltypeCount;

    // Implement required virtual methods
    virtual void addHeader(const DRW_Header* data) {
        std::cout << "Header added" << std::endl;
    }

    virtual void addLType(const DRW_LType& data) {
        ltypeCount++;
    }
    virtual void addLayer(const DRW_Layer& data) {
        layerCount++;
    }
    virtual void addDimStyle(const DRW_Dimstyle& data) {}
    virtual void addVport(const DRW_Vport& data) {}
    virtual void addTextStyle(const DRW_Textstyle& data) {}
    virtual void addAppId(const DRW_AppId& data) {}

    virtual void addBlock(const DRW_Block& data) {}
    virtual void setBlock(const int handle) {}
    virtual void endBlock() {}

    virtual void addPoint(const DRW_Point& data) {
        pointCount++;
        std::cout << "Point added at (" << data.basePoint.x << ", "
                  << data.basePoint.y << ", " << data.basePoint.z << ")" << std::endl;
    }

    virtual void addLine(const DRW_Line& data) {
        lineCount++;
        std::cout << "Line added from (" << data.basePoint.x << ", "
                  << data.basePoint.y << ") to (" << data.secPoint.x << ", "
                  << data.secPoint.y << ")" << std::endl;
    }

    virtual void addRay(const DRW_Ray& data) {
        rayCount++;
    }
    virtual void addXline(const DRW_Xline& data) {
        xlineCount++;
    }

    virtual void addArc(const DRW_Arc& data) {
        arcCount++;
        std::cout << "Arc added at (" << data.basePoint.x << ", "
                  << data.basePoint.y << "), radius=" << data.radious << std::endl;
    }

    virtual void addCircle(const DRW_Circle& data) {
        circleCount++;
        std::cout << "Circle added at (" << data.basePoint.x << ", "
                  << data.basePoint.y << "), radius=" << data.radious << std::endl;
    }

    virtual void addEllipse(const DRW_Ellipse& data) {
        ellipseCount++;
    }
    virtual void addLWPolyline(const DRW_LWPolyline& data) {
        lwPolylineCount++;
    }
    virtual void addPolyline(const DRW_Polyline& data) {
        polylineCount++;
    }
    virtual void addSpline(const DRW_Spline* data) {
        splineCount++;
    }
    virtual void addKnot(const DRW_Entity& data) {}
    virtual void addInsert(const DRW_Insert& data) {
        insertCount++;
    }
    virtual void addTrace(const DRW_Trace& data) {
        traceCount++;
    }
    virtual void add3dFace(const DRW_3Dface& data) {
        face3dCount++;
    }
    virtual void addSolid(const DRW_Solid& data) {
        solidCount++;
    }
    virtual void addMText(const DRW_MText& data) {
        mtextCount++;
    }
    virtual void addText(const DRW_Text& data) {
        textCount++;
    }
    virtual void addDimAlign(const DRW_DimAligned *data) {}
    virtual void addDimLinear(const DRW_DimLinear *data) {}
    virtual void addDimRadial(const DRW_DimRadial *data) {}
    virtual void addDimDiametric(const DRW_DimDiametric *data) {}
    virtual void addDimAngular(const DRW_DimAngular *data) {}
    virtual void addDimAngular3P(const DRW_DimAngular3p *data) {}
    virtual void addDimOrdinate(const DRW_DimOrdinate *data) {}
    virtual void addLeader(const DRW_Leader *data) {}
    virtual void addHatch(const DRW_Hatch *data) {
        hatchCount++;
    }
    virtual void addViewport(const DRW_Viewport& data) {}
    virtual void addImage(const DRW_Image *data) {}
    virtual void linkImage(const DRW_ImageDef *data) {}
    virtual void addComment(const char* comment) {}

    // Write methods
    virtual void writeHeader(DRW_Header& data) {}
    virtual void writeBlocks() {}
    virtual void writeBlockRecords() {}
    virtual void writeEntities() {}
    virtual void writeLTypes() {}
    virtual void writeLayers() {}
    virtual void writeTextstyles() {}
    virtual void writeVports() {}
    virtual void writeDimstyles() {}
    virtual void writeAppId() {}

    void reset() {
        pointCount = 0;
        lineCount = 0;
        circleCount = 0;
        arcCount = 0;
        ellipseCount = 0;
        lwPolylineCount = 0;
        polylineCount = 0;
        splineCount = 0;
        textCount = 0;
        mtextCount = 0;
        insertCount = 0;
        rayCount = 0;
        xlineCount = 0;
        traceCount = 0;
        solidCount = 0;
        face3dCount = 0;
        hatchCount = 0;
        layerCount = 0;
        ltypeCount = 0;
    }
};

#endif // TEST_INTERFACE_H
