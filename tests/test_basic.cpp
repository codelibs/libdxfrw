/******************************************************************************
**  libDXFrw - Basic Tests                                                  **
**                                                                           **
**  Copyright (C) 2025 libdxfrw contributors                                **
**                                                                           **
**  This library is free software, licensed under the terms of the GNU       **
**  General Public License as published by the Free Software Foundation,     **
**  either version 2 of the License, or (at your option) any later version.  **
**  You should have received a copy of the GNU General Public License        **
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.    **
******************************************************************************/

#include "libdxfrw.h"
#include "test_interface.h"
#include <iostream>
#include <cstdio>
#include <cmath>

bool testWriteBasicDXF() {
    std::cout << "\n=== Test: Write Basic DXF ===" << std::endl;

    const char* filename = "test_output.dxf";
    dxfRW dxf(filename);

    // Create a simple writer interface
    class WriterInterface : public DRW_Interface {
    public:
        virtual void addHeader(const DRW_Header* data) {}
        virtual void addLType(const DRW_LType& data) {}
        virtual void addLayer(const DRW_Layer& data) {}
        virtual void addDimStyle(const DRW_Dimstyle& data) {}
        virtual void addVport(const DRW_Vport& data) {}
        virtual void addTextStyle(const DRW_Textstyle& data) {}
        virtual void addAppId(const DRW_AppId& data) {}
        virtual void addBlock(const DRW_Block& data) {}
        virtual void setBlock(const int handle) {}
        virtual void endBlock() {}
        virtual void addPoint(const DRW_Point& data) {}
        virtual void addLine(const DRW_Line& data) {}
        virtual void addRay(const DRW_Ray& data) {}
        virtual void addXline(const DRW_Xline& data) {}
        virtual void addArc(const DRW_Arc& data) {}
        virtual void addCircle(const DRW_Circle& data) {}
        virtual void addEllipse(const DRW_Ellipse& data) {}
        virtual void addLWPolyline(const DRW_LWPolyline& data) {}
        virtual void addPolyline(const DRW_Polyline& data) {}
        virtual void addSpline(const DRW_Spline* data) {}
        virtual void addKnot(const DRW_Entity& data) {}
        virtual void addInsert(const DRW_Insert& data) {}
        virtual void addTrace(const DRW_Trace& data) {}
        virtual void add3dFace(const DRW_3Dface& data) {}
        virtual void addSolid(const DRW_Solid& data) {}
        virtual void addMText(const DRW_MText& data) {}
        virtual void addText(const DRW_Text& data) {}
        virtual void addDimAlign(const DRW_DimAligned *data) {}
        virtual void addDimLinear(const DRW_DimLinear *data) {}
        virtual void addDimRadial(const DRW_DimRadial *data) {}
        virtual void addDimDiametric(const DRW_DimDiametric *data) {}
        virtual void addDimAngular(const DRW_DimAngular *data) {}
        virtual void addDimAngular3P(const DRW_DimAngular3p *data) {}
        virtual void addDimOrdinate(const DRW_DimOrdinate *data) {}
        virtual void addLeader(const DRW_Leader *data) {}
        virtual void addHatch(const DRW_Hatch *data) {}
        virtual void addViewport(const DRW_Viewport& data) {}
        virtual void addImage(const DRW_Image *data) {}
        virtual void linkImage(const DRW_ImageDef *data) {}
        virtual void addComment(const char* comment) {}

        virtual void writeHeader(DRW_Header& data) {}
        virtual void writeBlocks() {}
        virtual void writeBlockRecords() {}

        virtual void writeEntities() {
            // Write a point
            DRW_Point point;
            point.basePoint.x = 10.0;
            point.basePoint.y = 20.0;
            point.basePoint.z = 0.0;
            dxfWriter->writePoint(&point);

            // Write a line
            DRW_Line line;
            line.basePoint.x = 0.0;
            line.basePoint.y = 0.0;
            line.basePoint.z = 0.0;
            line.secPoint.x = 100.0;
            line.secPoint.y = 100.0;
            line.secPoint.z = 0.0;
            dxfWriter->writeLine(&line);

            // Write a circle
            DRW_Circle circle;
            circle.basePoint.x = 50.0;
            circle.basePoint.y = 50.0;
            circle.basePoint.z = 0.0;
            circle.radious = 25.0;
            dxfWriter->writeCircle(&circle);
        }

        virtual void writeLTypes() {}
        virtual void writeLayers() {}
        virtual void writeTextstyles() {}
        virtual void writeVports() {}
        virtual void writeDimstyles() {}
        virtual void writeAppId() {}

        dxfRW* dxfWriter;
    };

    WriterInterface writer;
    writer.dxfWriter = &dxf;

    bool success = dxf.write(&writer, DRW::AC1015, false); // R2000, ASCII

    if (success) {
        std::cout << "✓ DXF file written successfully: " << filename << std::endl;
    } else {
        std::cout << "✗ Failed to write DXF file" << std::endl;
    }

    return success;
}

bool testReadBasicDXF() {
    std::cout << "\n=== Test: Read Basic DXF ===" << std::endl;

    const char* filename = "test_output.dxf";
    dxfRW dxf(filename);

    TestInterface reader;

    bool success = dxf.read(&reader, false);

    if (success) {
        std::cout << "✓ DXF file read successfully" << std::endl;
        std::cout << "  Points read: " << reader.pointCount << std::endl;
        std::cout << "  Lines read: " << reader.lineCount << std::endl;
        std::cout << "  Circles read: " << reader.circleCount << std::endl;

        // Validate counts
        if (reader.pointCount == 1 && reader.lineCount == 1 && reader.circleCount == 1) {
            std::cout << "✓ Entity counts match expected values" << std::endl;
        } else {
            std::cout << "✗ Entity counts don't match (expected 1 point, 1 line, 1 circle)" << std::endl;
            return false;
        }
    } else {
        std::cout << "✗ Failed to read DXF file" << std::endl;
    }

    return success;
}

bool testBinaryDXF() {
    std::cout << "\n=== Test: Binary DXF ===" << std::endl;

    const char* filename = "test_binary.dxf";
    dxfRW dxf(filename);

    // Create a simple writer interface
    class WriterInterface : public DRW_Interface {
    public:
        virtual void addHeader(const DRW_Header* data) {}
        virtual void addLType(const DRW_LType& data) {}
        virtual void addLayer(const DRW_Layer& data) {}
        virtual void addDimStyle(const DRW_Dimstyle& data) {}
        virtual void addVport(const DRW_Vport& data) {}
        virtual void addTextStyle(const DRW_Textstyle& data) {}
        virtual void addAppId(const DRW_AppId& data) {}
        virtual void addBlock(const DRW_Block& data) {}
        virtual void setBlock(const int handle) {}
        virtual void endBlock() {}
        virtual void addPoint(const DRW_Point& data) {}
        virtual void addLine(const DRW_Line& data) {}
        virtual void addRay(const DRW_Ray& data) {}
        virtual void addXline(const DRW_Xline& data) {}
        virtual void addArc(const DRW_Arc& data) {}
        virtual void addCircle(const DRW_Circle& data) {}
        virtual void addEllipse(const DRW_Ellipse& data) {}
        virtual void addLWPolyline(const DRW_LWPolyline& data) {}
        virtual void addPolyline(const DRW_Polyline& data) {}
        virtual void addSpline(const DRW_Spline* data) {}
        virtual void addKnot(const DRW_Entity& data) {}
        virtual void addInsert(const DRW_Insert& data) {}
        virtual void addTrace(const DRW_Trace& data) {}
        virtual void add3dFace(const DRW_3Dface& data) {}
        virtual void addSolid(const DRW_Solid& data) {}
        virtual void addMText(const DRW_MText& data) {}
        virtual void addText(const DRW_Text& data) {}
        virtual void addDimAlign(const DRW_DimAligned *data) {}
        virtual void addDimLinear(const DRW_DimLinear *data) {}
        virtual void addDimRadial(const DRW_DimRadial *data) {}
        virtual void addDimDiametric(const DRW_DimDiametric *data) {}
        virtual void addDimAngular(const DRW_DimAngular *data) {}
        virtual void addDimAngular3P(const DRW_DimAngular3p *data) {}
        virtual void addDimOrdinate(const DRW_DimOrdinate *data) {}
        virtual void addLeader(const DRW_Leader *data) {}
        virtual void addHatch(const DRW_Hatch *data) {}
        virtual void addViewport(const DRW_Viewport& data) {}
        virtual void addImage(const DRW_Image *data) {}
        virtual void linkImage(const DRW_ImageDef *data) {}
        virtual void addComment(const char* comment) {}

        virtual void writeHeader(DRW_Header& data) {}
        virtual void writeBlocks() {}
        virtual void writeBlockRecords() {}

        virtual void writeEntities() {
            DRW_Line line;
            line.basePoint.x = 0.0;
            line.basePoint.y = 0.0;
            line.basePoint.z = 0.0;
            line.secPoint.x = 50.0;
            line.secPoint.y = 50.0;
            line.secPoint.z = 0.0;
            dxfWriter->writeLine(&line);
        }

        virtual void writeLTypes() {}
        virtual void writeLayers() {}
        virtual void writeTextstyles() {}
        virtual void writeVports() {}
        virtual void writeDimstyles() {}
        virtual void writeAppId() {}

        dxfRW* dxfWriter;
    };

    WriterInterface writer;
    writer.dxfWriter = &dxf;

    bool success = dxf.write(&writer, DRW::AC1015, true); // R2000, Binary

    if (success) {
        std::cout << "✓ Binary DXF file written successfully" << std::endl;

        // Try to read it back
        TestInterface reader;
        dxfRW dxfRead(filename);
        if (dxfRead.read(&reader, false)) {
            std::cout << "✓ Binary DXF file read successfully" << std::endl;
            std::cout << "  Lines read: " << reader.lineCount << std::endl;
        } else {
            std::cout << "✗ Failed to read binary DXF file" << std::endl;
            return false;
        }
    } else {
        std::cout << "✗ Failed to write binary DXF file" << std::endl;
        return false;
    }

    return success;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Basic Tests" << std::endl;
    std::cout << "===================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    // Test 1: Write a basic DXF file
    totalTests++;
    if (!testWriteBasicDXF()) {
        failedTests++;
    }

    // Test 2: Read the DXF file back
    totalTests++;
    if (!testReadBasicDXF()) {
        failedTests++;
    }

    // Test 3: Binary DXF
    totalTests++;
    if (!testBinaryDXF()) {
        failedTests++;
    }

    // Clean up test files
    std::remove("test_output.dxf");
    std::remove("test_binary.dxf");

    std::cout << "\n===================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All tests passed!" << std::endl;
        return 0;
    }
}
