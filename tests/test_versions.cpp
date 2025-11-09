/******************************************************************************
**  libDXFrw - DXF Version Tests                                            **
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

bool testDXFVersion(DRW::Version version, const char* versionName) {
    std::cout << "\n=== Test: DXF Version " << versionName << " ===" << std::endl;

    std::string filename = std::string("test_") + versionName + ".dxf";

    // Write file in specific version
    {
        dxfRW dxf(filename.c_str());
        class VersionWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // Write various entities
                DRW_Point point;
                point.basePoint.x = 10.0;
                point.basePoint.y = 20.0;
                point.basePoint.z = 0.0;
                dxfWriter->writePoint(&point);

                DRW_Line line;
                line.basePoint.x = 0.0;
                line.basePoint.y = 0.0;
                line.basePoint.z = 0.0;
                line.secPoint.x = 100.0;
                line.secPoint.y = 100.0;
                line.secPoint.z = 0.0;
                dxfWriter->writeLine(&line);

                DRW_Circle circle;
                circle.basePoint.x = 50.0;
                circle.basePoint.y = 50.0;
                circle.basePoint.z = 0.0;
                circle.radious = 25.0;
                dxfWriter->writeCircle(&circle);

                DRW_Arc arc;
                arc.basePoint.x = 100.0;
                arc.basePoint.y = 100.0;
                arc.basePoint.z = 0.0;
                arc.radious = 30.0;
                arc.staangle = 0.0;
                arc.endangle = M_PI / 2.0;
                dxfWriter->writeArc(&arc);
            }
            dxfRW* dxfWriter;
        };

        VersionWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, version, false)) {
            std::cout << "✗ Failed to write DXF " << versionName << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename.c_str());
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read DXF " << versionName << std::endl;
            std::remove(filename.c_str());
            return false;
        }

        bool valid = (reader.pointCount == 1 && reader.lineCount == 1 &&
                     reader.circleCount == 1 && reader.arcCount == 1);

        if (!valid) {
            std::cout << "✗ Entity counts don't match for " << versionName << std::endl;
            std::cout << "  Expected: 1 point, 1 line, 1 circle, 1 arc" << std::endl;
            std::cout << "  Got: " << reader.pointCount << " points, "
                      << reader.lineCount << " lines, "
                      << reader.circleCount << " circles, "
                      << reader.arcCount << " arcs" << std::endl;
            std::remove(filename.c_str());
            return false;
        }

        std::cout << "✓ DXF " << versionName << " test passed" << std::endl;
    }

    std::remove(filename.c_str());
    return true;
}

bool testBinaryDXFWrite() {
    std::cout << "\n=== Test: Binary DXF Write ===" << std::endl;

    const char* binaryFile = "test_binary_write.dxf";

    // Test that we can write a binary DXF file
    {
        dxfRW dxf(binaryFile);
        class BinaryWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Line line;
                line.basePoint.x = 0.0;
                line.basePoint.y = 0.0;
                line.basePoint.z = 0.0;
                line.secPoint.x = 100.0;
                line.secPoint.y = 100.0;
                line.secPoint.z = 0.0;
                dxfWriter->writeLine(&line);

                DRW_Circle circle;
                circle.basePoint.x = 50.0;
                circle.basePoint.y = 50.0;
                circle.basePoint.z = 0.0;
                circle.radious = 25.0;
                dxfWriter->writeCircle(&circle);
            }
            dxfRW* dxfWriter;
        };

        BinaryWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, true)) {  // Binary
            std::cout << "✗ Failed to write binary DXF" << std::endl;
            return false;
        }
    }

    // Note: Binary DXF reading may have limitations in this library
    // We just verify the file was written
    std::cout << "✓ Binary DXF write test passed" << std::endl;

    std::remove(binaryFile);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw DXF Version Tests" << std::endl;
    std::cout << "==========================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    // Test R12
    totalTests++;
    if (!testDXFVersion(DRW::AC1009, "R12")) failedTests++;

    // Test R2000
    totalTests++;
    if (!testDXFVersion(DRW::AC1015, "R2000")) failedTests++;

    // Test R2004
    totalTests++;
    if (!testDXFVersion(DRW::AC1018, "R2004")) failedTests++;

    // Test R2007
    totalTests++;
    if (!testDXFVersion(DRW::AC1021, "R2007")) failedTests++;

    // Test R2010
    totalTests++;
    if (!testDXFVersion(DRW::AC1024, "R2010")) failedTests++;

    // Test R2013
    totalTests++;
    if (!testDXFVersion(DRW::AC1027, "R2013")) failedTests++;

    // Test R2018
    totalTests++;
    if (!testDXFVersion(DRW::AC1032, "R2018")) failedTests++;

    // Test binary DXF write
    totalTests++;
    if (!testBinaryDXFWrite()) failedTests++;

    std::cout << "\n==========================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All version tests passed!" << std::endl;
        return 0;
    }
}
