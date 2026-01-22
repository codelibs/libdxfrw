/******************************************************************************
**  libDXFrw - Dimension Entity Tests                                        **
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

bool testDimAligned() {
    std::cout << "\n=== Test: Aligned Dimension ===" << std::endl;

    const char* filename = "test_dim_aligned.dxf";

    // Write aligned dimension
    {
        dxfRW dxf(filename);
        class DimWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_DimAligned dim;
                dim.type = 1;  // Aligned dimension type
                dim.setDef1Point(DRW_Coord(0.0, 0.0, 0.0));
                dim.setDef2Point(DRW_Coord(100.0, 0.0, 0.0));
                dim.setDimPoint(DRW_Coord(50.0, 20.0, 0.0));
                dim.setTextPoint(DRW_Coord(50.0, 25.0, 0.0));
                dim.setText("<>");  // Use measured value
                dxfWriter->writeDimension(&dim);
            }
            dxfRW* dxfWriter;
        };

        DimWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write aligned dimension" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read aligned dimension" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.dimAlignedCount != 1) {
            std::cout << "✗ Expected 1 aligned dimension, got " << reader.dimAlignedCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Aligned dimension test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testDimLinear() {
    std::cout << "\n=== Test: Linear Dimension ===" << std::endl;

    const char* filename = "test_dim_linear.dxf";

    // Write linear dimension
    {
        dxfRW dxf(filename);
        class DimWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_DimLinear dim;
                dim.setDef1Point(DRW_Coord(0.0, 0.0, 0.0));
                dim.setDef2Point(DRW_Coord(100.0, 50.0, 0.0));
                dim.setDimPoint(DRW_Coord(50.0, 70.0, 0.0));
                dim.setAngle(0.0);  // Horizontal
                dim.setText("<>");
                dxfWriter->writeDimension(&dim);
            }
            dxfRW* dxfWriter;
        };

        DimWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write linear dimension" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read linear dimension" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.dimLinearCount != 1) {
            std::cout << "✗ Expected 1 linear dimension, got " << reader.dimLinearCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Linear dimension test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testDimRadial() {
    std::cout << "\n=== Test: Radial Dimension ===" << std::endl;

    const char* filename = "test_dim_radial.dxf";

    // Write radial dimension
    {
        dxfRW dxf(filename);
        class DimWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_DimRadial dim;
                dim.type = 4;  // Radial dimension type
                dim.setCenterPoint(DRW_Coord(50.0, 50.0, 0.0));
                dim.setDiameterPoint(DRW_Coord(80.0, 50.0, 0.0));
                dim.setLeaderLength(15.0);
                dim.setText("<>");
                dxfWriter->writeDimension(&dim);
            }
            dxfRW* dxfWriter;
        };

        DimWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write radial dimension" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read radial dimension" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.dimRadialCount != 1) {
            std::cout << "✗ Expected 1 radial dimension, got " << reader.dimRadialCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Radial dimension test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testDimDiametric() {
    std::cout << "\n=== Test: Diametric Dimension ===" << std::endl;

    const char* filename = "test_dim_diametric.dxf";

    // Write diametric dimension
    {
        dxfRW dxf(filename);
        class DimWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_DimDiametric dim;
                dim.type = 3;  // Diametric dimension type
                dim.setDiameter1Point(DRW_Coord(20.0, 50.0, 0.0));
                dim.setDiameter2Point(DRW_Coord(80.0, 50.0, 0.0));
                dim.setLeaderLength(10.0);
                dim.setText("<>");
                dxfWriter->writeDimension(&dim);
            }
            dxfRW* dxfWriter;
        };

        DimWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write diametric dimension" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read diametric dimension" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.dimDiametricCount != 1) {
            std::cout << "✗ Expected 1 diametric dimension, got " << reader.dimDiametricCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Diametric dimension test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testDimAngular() {
    std::cout << "\n=== Test: Angular Dimension ===" << std::endl;

    const char* filename = "test_dim_angular.dxf";

    // Write angular dimension
    {
        dxfRW dxf(filename);
        class DimWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_DimAngular dim;
                dim.type = 2;  // Angular dimension type
                // First line
                dim.setFirstLine1(DRW_Coord(0.0, 0.0, 0.0));
                dim.setFirstLine2(DRW_Coord(100.0, 0.0, 0.0));
                // Second line
                dim.setSecondLine1(DRW_Coord(0.0, 0.0, 0.0));
                dim.setSecondLine2(DRW_Coord(70.0, 70.0, 0.0));
                // Dimension arc location
                dim.setDimPoint(DRW_Coord(50.0, 30.0, 0.0));
                dim.setText("<>");
                dxfWriter->writeDimension(&dim);
            }
            dxfRW* dxfWriter;
        };

        DimWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write angular dimension" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read angular dimension" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.dimAngularCount != 1) {
            std::cout << "✗ Expected 1 angular dimension, got " << reader.dimAngularCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Angular dimension test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testDimAngular3P() {
    std::cout << "\n=== Test: Angular 3-Point Dimension ===" << std::endl;

    const char* filename = "test_dim_angular3p.dxf";

    // Write angular 3-point dimension
    {
        dxfRW dxf(filename);
        class DimWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_DimAngular3p dim;
                dim.type = 5;  // Angular 3-point dimension type
                dim.SetVertexPoint(DRW_Coord(50.0, 50.0, 0.0));  // Vertex
                dim.setFirstLine(DRW_Coord(100.0, 50.0, 0.0));   // First point
                dim.setSecondLine(DRW_Coord(50.0, 100.0, 0.0));  // Second point
                dim.setDimPoint(DRW_Coord(80.0, 80.0, 0.0));     // Arc location
                dim.setText("<>");
                dxfWriter->writeDimension(&dim);
            }
            dxfRW* dxfWriter;
        };

        DimWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write angular 3P dimension" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read angular 3P dimension" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.dimAngular3PCount != 1) {
            std::cout << "✗ Expected 1 angular 3P dimension, got " << reader.dimAngular3PCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Angular 3P dimension test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testDimOrdinate() {
    std::cout << "\n=== Test: Ordinate Dimension ===" << std::endl;

    const char* filename = "test_dim_ordinate.dxf";

    // Write ordinate dimension
    {
        dxfRW dxf(filename);
        class DimWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_DimOrdinate dim;
                dim.setOriginPoint(DRW_Coord(0.0, 0.0, 0.0));    // Origin
                dim.setFirstLine(DRW_Coord(75.0, 50.0, 0.0));    // Feature point
                dim.setSecondLine(DRW_Coord(75.0, 70.0, 0.0));   // Leader endpoint
                dim.type = 0x46;  // Ordinate type with X-type flag
                dim.setText("<>");
                dxfWriter->writeDimension(&dim);
            }
            dxfRW* dxfWriter;
        };

        DimWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write ordinate dimension" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read ordinate dimension" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.dimOrdinateCount != 1) {
            std::cout << "✗ Expected 1 ordinate dimension, got " << reader.dimOrdinateCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Ordinate dimension test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testMultipleDimensions() {
    std::cout << "\n=== Test: Multiple Dimensions ===" << std::endl;

    const char* filename = "test_multi_dim.dxf";

    // Write multiple dimensions
    {
        dxfRW dxf(filename);
        class DimWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // Aligned dimension
                DRW_DimAligned dimA;
                dimA.type = 1;  // Aligned type
                dimA.setDef1Point(DRW_Coord(0.0, 0.0, 0.0));
                dimA.setDef2Point(DRW_Coord(100.0, 0.0, 0.0));
                dimA.setDimPoint(DRW_Coord(50.0, -20.0, 0.0));
                dxfWriter->writeDimension(&dimA);

                // Linear dimension
                DRW_DimLinear dimL;
                dimL.type = 0;  // Linear type
                dimL.setDef1Point(DRW_Coord(0.0, 0.0, 0.0));
                dimL.setDef2Point(DRW_Coord(0.0, 80.0, 0.0));
                dimL.setDimPoint(DRW_Coord(-20.0, 40.0, 0.0));
                dimL.setAngle(M_PI / 2.0);  // Vertical
                dxfWriter->writeDimension(&dimL);

                // Radial dimension
                DRW_DimRadial dimR;
                dimR.type = 4;  // Radial type
                dimR.setCenterPoint(DRW_Coord(50.0, 50.0, 0.0));
                dimR.setDiameterPoint(DRW_Coord(75.0, 50.0, 0.0));
                dimR.setLeaderLength(10.0);
                dxfWriter->writeDimension(&dimR);
            }
            dxfRW* dxfWriter;
        };

        DimWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write multiple dimensions" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read multiple dimensions" << std::endl;
            std::remove(filename);
            return false;
        }

        int totalDims = reader.dimAlignedCount + reader.dimLinearCount + reader.dimRadialCount;
        if (totalDims != 3) {
            std::cout << "✗ Expected 3 dimensions, got " << totalDims << std::endl;
            std::cout << "  Aligned: " << reader.dimAlignedCount << std::endl;
            std::cout << "  Linear: " << reader.dimLinearCount << std::endl;
            std::cout << "  Radial: " << reader.dimRadialCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Multiple dimensions test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Dimension Entity Tests" << std::endl;
    std::cout << "===============================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    totalTests++;
    if (!testDimAligned()) failedTests++;

    totalTests++;
    if (!testDimLinear()) failedTests++;

    totalTests++;
    if (!testDimRadial()) failedTests++;

    totalTests++;
    if (!testDimDiametric()) failedTests++;

    totalTests++;
    if (!testDimAngular()) failedTests++;

    totalTests++;
    if (!testDimAngular3P()) failedTests++;

    totalTests++;
    if (!testDimOrdinate()) failedTests++;

    totalTests++;
    if (!testMultipleDimensions()) failedTests++;

    std::cout << "\n===============================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All dimension tests passed!" << std::endl;
        return 0;
    }
}
