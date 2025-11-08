/******************************************************************************
**  libDXFrw - Polyline and Spline Tests                                    **
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

bool testLWPolyline() {
    std::cout << "\n=== Test: LWPolyline (Lightweight Polyline) ===" << std::endl;

    const char* filename = "test_lwpolyline.dxf";

    // Write LWPolyline
    {
        dxfRW dxf(filename);
        class LWPolylineWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_LWPolyline lwpoly;
                lwpoly.vertlist.push_back(new DRW_Vertex2D(0.0, 0.0, 0.0));
                lwpoly.vertlist.push_back(new DRW_Vertex2D(100.0, 0.0, 0.0));
                lwpoly.vertlist.push_back(new DRW_Vertex2D(100.0, 100.0, 0.0));
                lwpoly.vertlist.push_back(new DRW_Vertex2D(0.0, 100.0, 0.0));
                lwpoly.flags = 1;  // Closed polyline
                dxfWriter->writeLWPolyline(&lwpoly);

                // Clean up vertices
                for (size_t i = 0; i < lwpoly.vertlist.size(); i++) {
                    delete lwpoly.vertlist[i];
                }
            }
            dxfRW* dxfWriter;
        };

        LWPolylineWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write LWPolyline" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read LWPolyline" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.lwPolylineCount != 1) {
            std::cout << "✗ Expected 1 LWPolyline, got " << reader.lwPolylineCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ LWPolyline test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testPolyline() {
    std::cout << "\n=== Test: Polyline (3D Polyline) ===" << std::endl;

    const char* filename = "test_polyline.dxf";

    // Write Polyline
    {
        dxfRW dxf(filename);
        class PolylineWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Polyline poly;
                poly.flags = 8;  // 3D polyline
                poly.vertlist.push_back(new DRW_Vertex(0.0, 0.0, 0.0, 0.0));
                poly.vertlist.push_back(new DRW_Vertex(50.0, 0.0, 10.0, 0.0));
                poly.vertlist.push_back(new DRW_Vertex(50.0, 50.0, 20.0, 0.0));
                poly.vertlist.push_back(new DRW_Vertex(0.0, 50.0, 10.0, 0.0));
                dxfWriter->writePolyline(&poly);

                // Clean up vertices
                for (size_t i = 0; i < poly.vertlist.size(); i++) {
                    delete poly.vertlist[i];
                }
            }
            dxfRW* dxfWriter;
        };

        PolylineWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write Polyline" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read Polyline" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.polylineCount != 1) {
            std::cout << "✗ Expected 1 Polyline, got " << reader.polylineCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Polyline test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testSpline() {
    std::cout << "\n=== Test: Spline ===" << std::endl;

    const char* filename = "test_spline.dxf";

    // Write Spline
    {
        dxfRW dxf(filename);
        class SplineWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Spline spline;
                spline.degree = 3;  // Cubic spline
                spline.ncontrol = 5;  // Number of control points

                // Add control points
                spline.controllist.push_back(new DRW_Coord(0.0, 0.0, 0.0));
                spline.controllist.push_back(new DRW_Coord(25.0, 50.0, 0.0));
                spline.controllist.push_back(new DRW_Coord(50.0, 50.0, 0.0));
                spline.controllist.push_back(new DRW_Coord(75.0, 0.0, 0.0));
                spline.controllist.push_back(new DRW_Coord(100.0, 0.0, 0.0));

                // Add knots
                spline.nknots = 9;  // degree + control points + 1
                spline.knotslist.push_back(0.0);
                spline.knotslist.push_back(0.0);
                spline.knotslist.push_back(0.0);
                spline.knotslist.push_back(0.0);
                spline.knotslist.push_back(0.5);
                spline.knotslist.push_back(1.0);
                spline.knotslist.push_back(1.0);
                spline.knotslist.push_back(1.0);
                spline.knotslist.push_back(1.0);

                dxfWriter->writeSpline(&spline);

                // Clean up
                for (size_t i = 0; i < spline.controllist.size(); i++) {
                    delete spline.controllist[i];
                }
            }
            dxfRW* dxfWriter;
        };

        SplineWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write Spline" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read Spline" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.splineCount != 1) {
            std::cout << "✗ Expected 1 Spline, got " << reader.splineCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Spline test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testClosedLWPolyline() {
    std::cout << "\n=== Test: Closed LWPolyline ===" << std::endl;

    const char* filename = "test_closed_lwpoly.dxf";

    // Write closed LWPolyline (pentagon)
    {
        dxfRW dxf(filename);
        class ClosedPolyWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_LWPolyline lwpoly;
                lwpoly.flags = 1;  // Closed

                // Pentagon vertices
                for (int i = 0; i < 5; i++) {
                    double angle = i * 2.0 * M_PI / 5.0;
                    double x = 50.0 + 30.0 * cos(angle);
                    double y = 50.0 + 30.0 * sin(angle);
                    lwpoly.vertlist.push_back(new DRW_Vertex2D(x, y, 0.0));
                }

                dxfWriter->writeLWPolyline(&lwpoly);

                // Clean up
                for (size_t i = 0; i < lwpoly.vertlist.size(); i++) {
                    delete lwpoly.vertlist[i];
                }
            }
            dxfRW* dxfWriter;
        };

        ClosedPolyWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write closed LWPolyline" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read closed LWPolyline" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.lwPolylineCount != 1) {
            std::cout << "✗ Expected 1 closed LWPolyline, got " << reader.lwPolylineCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Closed LWPolyline test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testPolylineWithBulge() {
    std::cout << "\n=== Test: LWPolyline with Bulge (Arc Segments) ===" << std::endl;

    const char* filename = "test_bulge_lwpoly.dxf";

    // Write LWPolyline with bulge
    {
        dxfRW dxf(filename);
        class BulgePolyWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_LWPolyline lwpoly;

                // Rectangle with rounded corners using bulge
                DRW_Vertex2D* v1 = new DRW_Vertex2D(0.0, 0.0, 0.5);  // bulge for arc
                DRW_Vertex2D* v2 = new DRW_Vertex2D(100.0, 0.0, 0.5);
                DRW_Vertex2D* v3 = new DRW_Vertex2D(100.0, 100.0, 0.5);
                DRW_Vertex2D* v4 = new DRW_Vertex2D(0.0, 100.0, 0.5);

                lwpoly.vertlist.push_back(v1);
                lwpoly.vertlist.push_back(v2);
                lwpoly.vertlist.push_back(v3);
                lwpoly.vertlist.push_back(v4);
                lwpoly.flags = 1;  // Closed

                dxfWriter->writeLWPolyline(&lwpoly);

                // Clean up
                for (size_t i = 0; i < lwpoly.vertlist.size(); i++) {
                    delete lwpoly.vertlist[i];
                }
            }
            dxfRW* dxfWriter;
        };

        BulgePolyWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write LWPolyline with bulge" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read LWPolyline with bulge" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.lwPolylineCount != 1) {
            std::cout << "✗ Expected 1 LWPolyline with bulge, got " << reader.lwPolylineCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ LWPolyline with bulge test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Polyline and Spline Tests" << std::endl;
    std::cout << "===================================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    totalTests++;
    if (!testLWPolyline()) failedTests++;

    totalTests++;
    if (!testPolyline()) failedTests++;

    totalTests++;
    if (!testSpline()) failedTests++;

    totalTests++;
    if (!testClosedLWPolyline()) failedTests++;

    totalTests++;
    if (!testPolylineWithBulge()) failedTests++;

    std::cout << "\n===================================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All polyline/spline tests passed!" << std::endl;
        return 0;
    }
}
