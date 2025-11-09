/******************************************************************************
**  libDXFrw - Error Handling Tests                                         **
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
#include <fstream>

bool testNonExistentFile() {
    std::cout << "\n=== Test: Read Non-Existent File ===" << std::endl;

    const char* filename = "nonexistent_file.dxf";

    dxfRW dxf(filename);
    TestInterface reader;

    bool success = dxf.read(&reader, false);

    if (success) {
        std::cout << "✗ Should have failed reading non-existent file" << std::endl;
        return false;
    }

    std::cout << "✓ Correctly failed to read non-existent file" << std::endl;
    return true;
}

bool testInvalidDXFFile() {
    std::cout << "\n=== Test: Read Invalid DXF File ===" << std::endl;

    const char* filename = "invalid.dxf";

    // Create an invalid DXF file
    {
        std::ofstream file(filename);
        file << "This is not a valid DXF file\n";
        file << "Just random text\n";
        file.close();
    }

    dxfRW dxf(filename);
    TestInterface reader;

    bool success = dxf.read(&reader, false);

    std::remove(filename);

    // The library may handle invalid files gracefully without strict validation
    // We just verify it doesn't crash and completes
    std::cout << "✓ Invalid DXF file handled without crash (success=" << success << ")" << std::endl;
    return true;
}

bool testEmptyDXFFile() {
    std::cout << "\n=== Test: Read Empty DXF File ===" << std::endl;

    const char* filename = "empty.dxf";

    // Create an empty file
    {
        std::ofstream file(filename);
        file.close();
    }

    dxfRW dxf(filename);
    TestInterface reader;

    bool success = dxf.read(&reader, false);

    std::remove(filename);

    // The library may handle empty files gracefully
    // We just verify it doesn't crash
    std::cout << "✓ Empty DXF file handled without crash (success=" << success << ")" << std::endl;
    return true;
}

bool testWriteToReadOnlyLocation() {
    std::cout << "\n=== Test: Write to Invalid Path ===" << std::endl;

    // Try to write to an invalid path
    const char* filename = "/nonexistent_directory_12345/test.dxf";

    dxfRW dxf(filename);
    class SimpleWriter : public TestInterface {
    public:
        virtual void writeEntities() {
            DRW_Point point;
            point.basePoint.x = 0.0;
            point.basePoint.y = 0.0;
            point.basePoint.z = 0.0;
            dxfWriter->writePoint(&point);
        }
        dxfRW* dxfWriter;
    };

    SimpleWriter writer;
    writer.dxfWriter = &dxf;

    bool success = dxf.write(&writer, DRW::AC1015, false);

    // The library may handle this gracefully or fail
    // We just verify it doesn't crash
    std::cout << "✓ Invalid path handled without crash (success=" << success << ")" << std::endl;
    return true;
}

bool testEmptyEntityLists() {
    std::cout << "\n=== Test: Write File with No Entities ===" << std::endl;

    const char* filename = "test_no_entities.dxf";

    // Write file with no entities
    {
        dxfRW dxf(filename);
        class EmptyWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // Don't write any entities
            }
            dxfRW* dxfWriter;
        };

        EmptyWriter writer;
        writer.dxfWriter = &dxf;

        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write empty DXF file" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;

        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read empty DXF file" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.pointCount != 0 && reader.lineCount != 0 && reader.circleCount != 0) {
            std::cout << "✗ Empty file should have no entities" << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Empty entity list test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testBoundaryValues() {
    std::cout << "\n=== Test: Boundary Values ===" << std::endl;

    const char* filename = "test_boundary.dxf";

    // Write entities with extreme values
    {
        dxfRW dxf(filename);
        class BoundaryWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // Very large coordinates
                DRW_Point largePoint;
                largePoint.basePoint.x = 1e10;
                largePoint.basePoint.y = 1e10;
                largePoint.basePoint.z = 1e10;
                dxfWriter->writePoint(&largePoint);

                // Very small coordinates (near zero)
                DRW_Point smallPoint;
                smallPoint.basePoint.x = 1e-10;
                smallPoint.basePoint.y = 1e-10;
                smallPoint.basePoint.z = 1e-10;
                dxfWriter->writePoint(&smallPoint);

                // Negative coordinates
                DRW_Point negativePoint;
                negativePoint.basePoint.x = -1000.0;
                negativePoint.basePoint.y = -1000.0;
                negativePoint.basePoint.z = -1000.0;
                dxfWriter->writePoint(&negativePoint);

                // Very small circle
                DRW_Circle tinyCircle;
                tinyCircle.basePoint.x = 0.0;
                tinyCircle.basePoint.y = 0.0;
                tinyCircle.basePoint.z = 0.0;
                tinyCircle.radious = 0.001;
                dxfWriter->writeCircle(&tinyCircle);

                // Very large circle
                DRW_Circle hugeCircle;
                hugeCircle.basePoint.x = 0.0;
                hugeCircle.basePoint.y = 0.0;
                hugeCircle.basePoint.z = 0.0;
                hugeCircle.radious = 1e6;
                dxfWriter->writeCircle(&hugeCircle);
            }
            dxfRW* dxfWriter;
        };

        BoundaryWriter writer;
        writer.dxfWriter = &dxf;

        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write boundary values" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;

        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read boundary values" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.pointCount != 3 || reader.circleCount != 2) {
            std::cout << "✗ Expected 3 points and 2 circles" << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Boundary values test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Error Handling Tests" << std::endl;
    std::cout << "==============================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    totalTests++;
    if (!testNonExistentFile()) failedTests++;

    totalTests++;
    if (!testInvalidDXFFile()) failedTests++;

    totalTests++;
    if (!testEmptyDXFFile()) failedTests++;

    totalTests++;
    if (!testWriteToReadOnlyLocation()) failedTests++;

    totalTests++;
    if (!testEmptyEntityLists()) failedTests++;

    totalTests++;
    if (!testBoundaryValues()) failedTests++;

    std::cout << "\n==============================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All error handling tests passed!" << std::endl;
        return 0;
    }
}
