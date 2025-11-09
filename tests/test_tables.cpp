/******************************************************************************
**  libDXFrw - Table Objects Tests (Layers, LineTypes, etc.)               **
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

bool testLayerDefinitions() {
    std::cout << "\n=== Test: Layer Definitions ===" << std::endl;

    const char* filename = "test_layers.dxf";

    // Write layers
    {
        dxfRW dxf(filename);
        class LayerWriter : public TestInterface {
        public:
            virtual void writeLayers() {
                // Layer 0 (default layer)
                DRW_Layer layer0;
                layer0.name = "0";
                layer0.color = 7;  // White
                layer0.lWeight = DRW_LW_Conv::width00;
                dxfWriter->writeLayer(&layer0);

                // Construction layer
                DRW_Layer construction;
                construction.name = "Construction";
                construction.color = 8;  // Dark gray
                construction.lineType = "DASHED";
                construction.lWeight = DRW_LW_Conv::width01;
                dxfWriter->writeLayer(&construction);

                // Dimension layer
                DRW_Layer dimensions;
                dimensions.name = "Dimensions";
                dimensions.color = 1;  // Red
                dimensions.lineType = "CONTINUOUS";
                dimensions.lWeight = DRW_LW_Conv::width02;
                dxfWriter->writeLayer(&dimensions);

                // Hidden layer
                DRW_Layer hidden;
                hidden.name = "Hidden";
                hidden.color = 9;
                hidden.lineType = "HIDDEN";
                hidden.flags = 1;  // Frozen
                hidden.lWeight = DRW_LW_Conv::width03;
                dxfWriter->writeLayer(&hidden);
            }

            virtual void writeEntities() {
                // Add a line on layer "Construction"
                DRW_Line line;
                line.basePoint.x = 0.0;
                line.basePoint.y = 0.0;
                line.basePoint.z = 0.0;
                line.secPoint.x = 100.0;
                line.secPoint.y = 100.0;
                line.secPoint.z = 0.0;
                line.layer = "Construction";
                line.color = 256;  // ByLayer
                dxfWriter->writeLine(&line);
            }

            dxfRW* dxfWriter;
        };

        LayerWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write layers" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read layers" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.layerCount < 3) {  // At least 3 custom layers
            std::cout << "✗ Expected at least 3 layers, got " << reader.layerCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Layer definitions test passed (" << reader.layerCount << " layers)" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testLineTypes() {
    std::cout << "\n=== Test: Line Type Definitions ===" << std::endl;

    const char* filename = "test_linetypes.dxf";

    // Write line types
    {
        dxfRW dxf(filename);
        class LineTypeWriter : public TestInterface {
        public:
            virtual void writeLTypes() {
                // Continuous (standard)
                DRW_LType continuous;
                continuous.name = "CONTINUOUS";
                continuous.desc = "Solid line";
                dxfWriter->writeLineType(&continuous);

                // Dashed
                DRW_LType dashed;
                dashed.name = "DASHED";
                dashed.desc = "Dashed line";
                dashed.path.push_back(0.5);   // dash
                dashed.path.push_back(-0.25); // gap
                dxfWriter->writeLineType(&dashed);

                // Dotted
                DRW_LType dotted;
                dotted.name = "DOTTED";
                dotted.desc = "Dotted line";
                dotted.path.push_back(0.0);   // dot
                dotted.path.push_back(-0.25); // gap
                dxfWriter->writeLineType(&dotted);
            }

            virtual void writeEntities() {
                // Line with dashed linetype
                DRW_Line line;
                line.basePoint.x = 0.0;
                line.basePoint.y = 0.0;
                line.basePoint.z = 0.0;
                line.secPoint.x = 100.0;
                line.secPoint.y = 0.0;
                line.secPoint.z = 0.0;
                line.lineType = "DASHED";
                dxfWriter->writeLine(&line);
            }

            dxfRW* dxfWriter;
        };

        LineTypeWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write line types" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read line types" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.ltypeCount < 2) {  // At least 2 line types
            std::cout << "✗ Expected at least 2 line types, got " << reader.ltypeCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Line type definitions test passed (" << reader.ltypeCount << " line types)" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testLayersAndLineTypes() {
    std::cout << "\n=== Test: Layers and Line Types Together ===" << std::endl;

    const char* filename = "test_layers_linetypes.dxf";

    // Write both layers and line types
    {
        dxfRW dxf(filename);
        class LayerLineTypeWriter : public TestInterface {
        public:
            virtual void writeLayers() {
                DRW_Layer layer1;
                layer1.name = "Outline";
                layer1.color = 1;  // Red
                layer1.lineType = "CONTINUOUS";
                dxfWriter->writeLayer(&layer1);

                DRW_Layer layer2;
                layer2.name = "Centerline";
                layer2.color = 2;  // Yellow
                layer2.lineType = "CENTER";
                dxfWriter->writeLayer(&layer2);
            }

            virtual void writeLTypes() {
                DRW_LType continuous;
                continuous.name = "CONTINUOUS";
                continuous.desc = "Solid";
                dxfWriter->writeLineType(&continuous);

                DRW_LType center;
                center.name = "CENTER";
                center.desc = "Center line";
                center.path.push_back(1.25);
                center.path.push_back(-0.25);
                center.path.push_back(0.25);
                center.path.push_back(-0.25);
                dxfWriter->writeLineType(&center);
            }

            virtual void writeEntities() {
                // Line on Outline layer
                DRW_Line line1;
                line1.basePoint.x = 0.0;
                line1.basePoint.y = 0.0;
                line1.basePoint.z = 0.0;
                line1.secPoint.x = 100.0;
                line1.secPoint.y = 100.0;
                line1.secPoint.z = 0.0;
                line1.layer = "Outline";
                line1.color = 256;  // ByLayer
                dxfWriter->writeLine(&line1);

                // Line on Centerline layer
                DRW_Line line2;
                line2.basePoint.x = 50.0;
                line2.basePoint.y = 0.0;
                line2.basePoint.z = 0.0;
                line2.secPoint.x = 50.0;
                line2.secPoint.y = 100.0;
                line2.secPoint.z = 0.0;
                line2.layer = "Centerline";
                line2.color = 256;  // ByLayer
                dxfWriter->writeLine(&line2);
            }

            dxfRW* dxfWriter;
        };

        LayerLineTypeWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write layers and line types" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read layers and line types" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.layerCount < 2 || reader.ltypeCount < 2 || reader.lineCount != 2) {
            std::cout << "✗ Expected at least 2 layers, 2 line types, and 2 lines" << std::endl;
            std::cout << "  Got: " << reader.layerCount << " layers, "
                      << reader.ltypeCount << " line types, "
                      << reader.lineCount << " lines" << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Layers and line types test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Table Objects Tests" << std::endl;
    std::cout << "=============================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    totalTests++;
    if (!testLayerDefinitions()) failedTests++;

    totalTests++;
    if (!testLineTypes()) failedTests++;

    totalTests++;
    if (!testLayersAndLineTypes()) failedTests++;

    std::cout << "\n=============================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All table object tests passed!" << std::endl;
        return 0;
    }
}
