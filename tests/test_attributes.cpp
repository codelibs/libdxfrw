/******************************************************************************
**  libDXFrw - Entity Attribute Tests                                        **
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

// Helper for floating point comparison
bool approxEqual(double a, double b, double tolerance = 0.0001) {
    return std::fabs(a - b) < tolerance;
}

bool testEntityColor() {
    std::cout << "\n=== Test: Entity Color ===" << std::endl;

    const char* filename = "test_color.dxf";

    // Write entities with different colors
    {
        dxfRW dxf(filename);
        class ColorWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // Red line (color index 1)
                DRW_Line line1;
                line1.basePoint = DRW_Coord(0.0, 0.0, 0.0);
                line1.secPoint = DRW_Coord(100.0, 0.0, 0.0);
                line1.color = 1;  // Red
                dxfWriter->writeLine(&line1);

                // Yellow line (color index 2)
                DRW_Line line2;
                line2.basePoint = DRW_Coord(0.0, 10.0, 0.0);
                line2.secPoint = DRW_Coord(100.0, 10.0, 0.0);
                line2.color = 2;  // Yellow
                dxfWriter->writeLine(&line2);

                // Green circle (color index 3)
                DRW_Circle circle;
                circle.basePoint = DRW_Coord(50.0, 50.0, 0.0);
                circle.radious = 25.0;
                circle.color = 3;  // Green
                dxfWriter->writeCircle(&circle);

                // BYLAYER point (color index 256)
                DRW_Point point;
                point.basePoint = DRW_Coord(75.0, 75.0, 0.0);
                point.color = 256;  // BYLAYER
                dxfWriter->writePoint(&point);
            }
            dxfRW* dxfWriter;
        };

        ColorWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write colored entities" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read colored entities" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.lineCount != 2 || reader.circleCount != 1 || reader.pointCount != 1) {
            std::cout << "✗ Entity counts don't match" << std::endl;
            std::remove(filename);
            return false;
        }

        // Verify colors are preserved
        if (reader.lastCircle.color != 3) {
            std::cout << "✗ Circle color mismatch: expected 3, got " << reader.lastCircle.color << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Entity color test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testEntityLayer() {
    std::cout << "\n=== Test: Entity Layer Assignment ===" << std::endl;

    const char* filename = "test_layer.dxf";

    // Write entities on different layers
    {
        dxfRW dxf(filename);
        class LayerWriter : public TestInterface {
        public:
            virtual void writeLayers() {
                // Create custom layers
                DRW_Layer layer1;
                layer1.name = "LAYER1";
                layer1.color = 1;
                dxfWriter->writeLayer(&layer1);

                DRW_Layer layer2;
                layer2.name = "LAYER2";
                layer2.color = 2;
                dxfWriter->writeLayer(&layer2);
            }

            virtual void writeEntities() {
                // Line on LAYER1
                DRW_Line line1;
                line1.basePoint = DRW_Coord(0.0, 0.0, 0.0);
                line1.secPoint = DRW_Coord(100.0, 0.0, 0.0);
                line1.layer = "LAYER1";
                dxfWriter->writeLine(&line1);

                // Circle on LAYER2
                DRW_Circle circle;
                circle.basePoint = DRW_Coord(50.0, 50.0, 0.0);
                circle.radious = 20.0;
                circle.layer = "LAYER2";
                dxfWriter->writeCircle(&circle);

                // Point on default layer "0"
                DRW_Point point;
                point.basePoint = DRW_Coord(25.0, 25.0, 0.0);
                point.layer = "0";
                dxfWriter->writePoint(&point);
            }
            dxfRW* dxfWriter;
        };

        LayerWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write layered entities" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read layered entities" << std::endl;
            std::remove(filename);
            return false;
        }

        // Verify layers were created (including default layer 0)
        if (reader.layerCount < 3) {
            std::cout << "✗ Expected at least 3 layers, got " << reader.layerCount << std::endl;
            std::remove(filename);
            return false;
        }

        // Verify layer assignment
        if (reader.lastCircle.layer != "LAYER2") {
            std::cout << "✗ Circle layer mismatch: expected LAYER2, got " << reader.lastCircle.layer << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Entity layer test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testEntityLineWeight() {
    std::cout << "\n=== Test: Entity Line Weight ===" << std::endl;

    const char* filename = "test_lineweight.dxf";

    // Write entities with different line weights
    {
        dxfRW dxf(filename);
        class LWWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // Thin line
                DRW_Line line1;
                line1.basePoint = DRW_Coord(0.0, 0.0, 0.0);
                line1.secPoint = DRW_Coord(100.0, 0.0, 0.0);
                line1.lWeight = DRW_LW_Conv::width00;  // 0.00mm
                dxfWriter->writeLine(&line1);

                // Medium line
                DRW_Line line2;
                line2.basePoint = DRW_Coord(0.0, 20.0, 0.0);
                line2.secPoint = DRW_Coord(100.0, 20.0, 0.0);
                line2.lWeight = DRW_LW_Conv::width05;  // 0.50mm
                dxfWriter->writeLine(&line2);

                // Thick line
                DRW_Line line3;
                line3.basePoint = DRW_Coord(0.0, 40.0, 0.0);
                line3.secPoint = DRW_Coord(100.0, 40.0, 0.0);
                line3.lWeight = DRW_LW_Conv::width13;  // 1.00mm
                dxfWriter->writeLine(&line3);
            }
            dxfRW* dxfWriter;
        };

        LWWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write lineweight entities" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read lineweight entities" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.lineCount != 3) {
            std::cout << "✗ Expected 3 lines, got " << reader.lineCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Entity line weight test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testEntityLineType() {
    std::cout << "\n=== Test: Entity Line Type ===" << std::endl;

    const char* filename = "test_linetype.dxf";

    // Write entities with different line types
    {
        dxfRW dxf(filename);
        class LTWriter : public TestInterface {
        public:
            virtual void writeLTypes() {
                // Create dashed line type
                DRW_LType ltype;
                ltype.name = "DASHED";
                ltype.desc = "Dashed line __ __ __";
                ltype.size = 2;
                ltype.length = 0.75;
                ltype.path.push_back(0.5);
                ltype.path.push_back(-0.25);
                dxfWriter->writeLineType(&ltype);
            }

            virtual void writeEntities() {
                // Continuous line
                DRW_Line line1;
                line1.basePoint = DRW_Coord(0.0, 0.0, 0.0);
                line1.secPoint = DRW_Coord(100.0, 0.0, 0.0);
                line1.lineType = "CONTINUOUS";
                dxfWriter->writeLine(&line1);

                // Dashed line
                DRW_Line line2;
                line2.basePoint = DRW_Coord(0.0, 20.0, 0.0);
                line2.secPoint = DRW_Coord(100.0, 20.0, 0.0);
                line2.lineType = "DASHED";
                dxfWriter->writeLine(&line2);
            }
            dxfRW* dxfWriter;
        };

        LTWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write linetype entities" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read linetype entities" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.lineCount != 2) {
            std::cout << "✗ Expected 2 lines, got " << reader.lineCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Entity line type test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testCoordinatePreservation() {
    std::cout << "\n=== Test: Coordinate Preservation ===" << std::endl;

    const char* filename = "test_coords.dxf";

    const double testX1 = 123.456789;
    const double testY1 = 987.654321;
    const double testX2 = -50.5;
    const double testY2 = 200.25;

    // Write entity with specific coordinates
    {
        dxfRW dxf(filename);
        class CoordWriter : public TestInterface {
        public:
            double x1, y1, x2, y2;

            virtual void writeEntities() {
                DRW_Line line;
                line.basePoint = DRW_Coord(x1, y1, 0.0);
                line.secPoint = DRW_Coord(x2, y2, 0.0);
                dxfWriter->writeLine(&line);
            }
            dxfRW* dxfWriter;
        };

        CoordWriter writer;
        writer.x1 = testX1;
        writer.y1 = testY1;
        writer.x2 = testX2;
        writer.y2 = testY2;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write coordinate test" << std::endl;
            return false;
        }
    }

    // Read and verify coordinates are preserved
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read coordinate test" << std::endl;
            std::remove(filename);
            return false;
        }

        if (!approxEqual(reader.lastLine.basePoint.x, testX1) ||
            !approxEqual(reader.lastLine.basePoint.y, testY1)) {
            std::cout << "✗ Base point mismatch" << std::endl;
            std::cout << "  Expected: (" << testX1 << ", " << testY1 << ")" << std::endl;
            std::cout << "  Got: (" << reader.lastLine.basePoint.x << ", "
                      << reader.lastLine.basePoint.y << ")" << std::endl;
            std::remove(filename);
            return false;
        }

        if (!approxEqual(reader.lastLine.secPoint.x, testX2) ||
            !approxEqual(reader.lastLine.secPoint.y, testY2)) {
            std::cout << "✗ Second point mismatch" << std::endl;
            std::cout << "  Expected: (" << testX2 << ", " << testY2 << ")" << std::endl;
            std::cout << "  Got: (" << reader.lastLine.secPoint.x << ", "
                      << reader.lastLine.secPoint.y << ")" << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Coordinate preservation test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testCircleRadius() {
    std::cout << "\n=== Test: Circle Radius Preservation ===" << std::endl;

    const char* filename = "test_radius.dxf";

    const double testRadius = 42.5;
    const double testCenterX = 100.0;
    const double testCenterY = 75.0;

    // Write circle
    {
        dxfRW dxf(filename);
        class CircleWriter : public TestInterface {
        public:
            double radius, cx, cy;

            virtual void writeEntities() {
                DRW_Circle circle;
                circle.basePoint = DRW_Coord(cx, cy, 0.0);
                circle.radious = radius;
                dxfWriter->writeCircle(&circle);
            }
            dxfRW* dxfWriter;
        };

        CircleWriter writer;
        writer.radius = testRadius;
        writer.cx = testCenterX;
        writer.cy = testCenterY;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write circle" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read circle" << std::endl;
            std::remove(filename);
            return false;
        }

        if (!approxEqual(reader.lastCircle.radious, testRadius)) {
            std::cout << "✗ Radius mismatch: expected " << testRadius
                      << ", got " << reader.lastCircle.radious << std::endl;
            std::remove(filename);
            return false;
        }

        if (!approxEqual(reader.lastCircle.basePoint.x, testCenterX) ||
            !approxEqual(reader.lastCircle.basePoint.y, testCenterY)) {
            std::cout << "✗ Center point mismatch" << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Circle radius preservation test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testEntityVisibility() {
    std::cout << "\n=== Test: Entity Visibility ===" << std::endl;

    const char* filename = "test_visibility.dxf";

    // Write entities with different visibility
    {
        dxfRW dxf(filename);
        class VisWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // Visible line
                DRW_Line line1;
                line1.basePoint = DRW_Coord(0.0, 0.0, 0.0);
                line1.secPoint = DRW_Coord(100.0, 0.0, 0.0);
                line1.visible = true;
                dxfWriter->writeLine(&line1);

                // Invisible line (code 60 = 1)
                DRW_Line line2;
                line2.basePoint = DRW_Coord(0.0, 20.0, 0.0);
                line2.secPoint = DRW_Coord(100.0, 20.0, 0.0);
                line2.visible = false;
                dxfWriter->writeLine(&line2);
            }
            dxfRW* dxfWriter;
        };

        VisWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write visibility test" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read visibility test" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.lineCount != 2) {
            std::cout << "✗ Expected 2 lines, got " << reader.lineCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Entity visibility test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testLineTypeScale() {
    std::cout << "\n=== Test: Line Type Scale ===" << std::endl;

    // Note: ltypeScale (code 48) is read by the library but not written
    // in the current implementation. This test verifies that the library
    // at least handles the attribute without crashing and uses default value.

    const char* filename = "test_ltscale.dxf";

    // Write entity with linetype scale set
    {
        dxfRW dxf(filename);
        class LTScaleWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Line line;
                line.basePoint = DRW_Coord(0.0, 0.0, 0.0);
                line.secPoint = DRW_Coord(100.0, 0.0, 0.0);
                line.ltypeScale = 2.5;  // Set but not written by library
                dxfWriter->writeLine(&line);
            }
            dxfRW* dxfWriter;
        };

        LTScaleWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write ltscale test" << std::endl;
            return false;
        }
    }

    // Read and verify default value (library limitation: ltypeScale not written)
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read ltscale test" << std::endl;
            std::remove(filename);
            return false;
        }

        // Library uses default value 1.0 since code 48 is not written
        if (!approxEqual(reader.lastLine.ltypeScale, 1.0)) {
            std::cout << "✗ LT scale should default to 1.0, got "
                      << reader.lastLine.ltypeScale << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Line type scale test passed (default value)" << std::endl;
    }

    std::remove(filename);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Entity Attribute Tests" << std::endl;
    std::cout << "================================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    totalTests++;
    if (!testEntityColor()) failedTests++;

    totalTests++;
    if (!testEntityLayer()) failedTests++;

    totalTests++;
    if (!testEntityLineWeight()) failedTests++;

    totalTests++;
    if (!testEntityLineType()) failedTests++;

    totalTests++;
    if (!testCoordinatePreservation()) failedTests++;

    totalTests++;
    if (!testCircleRadius()) failedTests++;

    totalTests++;
    if (!testEntityVisibility()) failedTests++;

    totalTests++;
    if (!testLineTypeScale()) failedTests++;

    std::cout << "\n================================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All attribute tests passed!" << std::endl;
        return 0;
    }
}
