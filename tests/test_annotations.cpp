/******************************************************************************
**  libDXFrw - Annotation Entity Tests (Leader, Hatch)                       **
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

bool testBasicLeader() {
    std::cout << "\n=== Test: Basic Leader ===" << std::endl;

    const char* filename = "test_basic_leader.dxf";

    // Write leader
    {
        dxfRW dxf(filename);
        class LeaderWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Leader leader;
                leader.arrow = 1;  // Enable arrowhead
                leader.leadertype = 0;  // Straight line segments
                leader.flag = 3;

                // Add vertices (leader path)
                leader.vertexlist.push_back(new DRW_Coord(0.0, 0.0, 0.0));
                leader.vertexlist.push_back(new DRW_Coord(30.0, 30.0, 0.0));
                leader.vertexlist.push_back(new DRW_Coord(60.0, 30.0, 0.0));

                dxfWriter->writeLeader(&leader);

                // Clean up vertices
                for (size_t i = 0; i < leader.vertexlist.size(); i++) {
                    delete leader.vertexlist[i];
                }
                leader.vertexlist.clear();
            }
            dxfRW* dxfWriter;
        };

        LeaderWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write leader" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read leader" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.leaderCount != 1) {
            std::cout << "✗ Expected 1 leader, got " << reader.leaderCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Basic leader test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testSplineLeader() {
    std::cout << "\n=== Test: Spline Leader ===" << std::endl;

    const char* filename = "test_spline_leader.dxf";

    // Write spline leader
    {
        dxfRW dxf(filename);
        class LeaderWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Leader leader;
                leader.arrow = 1;
                leader.leadertype = 1;  // Spline path
                leader.flag = 3;

                // Add vertices for spline path
                leader.vertexlist.push_back(new DRW_Coord(0.0, 0.0, 0.0));
                leader.vertexlist.push_back(new DRW_Coord(20.0, 40.0, 0.0));
                leader.vertexlist.push_back(new DRW_Coord(50.0, 50.0, 0.0));
                leader.vertexlist.push_back(new DRW_Coord(80.0, 40.0, 0.0));

                dxfWriter->writeLeader(&leader);

                for (size_t i = 0; i < leader.vertexlist.size(); i++) {
                    delete leader.vertexlist[i];
                }
                leader.vertexlist.clear();
            }
            dxfRW* dxfWriter;
        };

        LeaderWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write spline leader" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read spline leader" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.leaderCount != 1) {
            std::cout << "✗ Expected 1 leader, got " << reader.leaderCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Spline leader test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testSolidHatch() {
    std::cout << "\n=== Test: Solid Hatch ===" << std::endl;

    const char* filename = "test_solid_hatch.dxf";

    // Write solid hatch
    {
        dxfRW dxf(filename);
        class HatchWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Hatch hatch;
                hatch.name = "SOLID";
                hatch.solid = 1;  // Solid fill
                hatch.associative = 0;
                hatch.hstyle = 0;
                hatch.hpattern = 1;
                hatch.loopsnum = 1;

                // Create boundary loop (rectangle)
                DRW_HatchLoop* loop = new DRW_HatchLoop(0);

                // Add lines for rectangular boundary
                DRW_Line* line1 = new DRW_Line();
                line1->basePoint = DRW_Coord(0.0, 0.0, 0.0);
                line1->secPoint = DRW_Coord(100.0, 0.0, 0.0);
                loop->objlist.push_back(line1);

                DRW_Line* line2 = new DRW_Line();
                line2->basePoint = DRW_Coord(100.0, 0.0, 0.0);
                line2->secPoint = DRW_Coord(100.0, 50.0, 0.0);
                loop->objlist.push_back(line2);

                DRW_Line* line3 = new DRW_Line();
                line3->basePoint = DRW_Coord(100.0, 50.0, 0.0);
                line3->secPoint = DRW_Coord(0.0, 50.0, 0.0);
                loop->objlist.push_back(line3);

                DRW_Line* line4 = new DRW_Line();
                line4->basePoint = DRW_Coord(0.0, 50.0, 0.0);
                line4->secPoint = DRW_Coord(0.0, 0.0, 0.0);
                loop->objlist.push_back(line4);

                loop->update();
                hatch.appendLoop(loop);

                dxfWriter->writeHatch(&hatch);
            }
            dxfRW* dxfWriter;
        };

        HatchWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write solid hatch" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read solid hatch" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.hatchCount != 1) {
            std::cout << "✗ Expected 1 hatch, got " << reader.hatchCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Solid hatch test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testPatternHatch() {
    std::cout << "\n=== Test: Pattern Hatch ===" << std::endl;

    const char* filename = "test_pattern_hatch.dxf";

    // Write pattern hatch
    {
        dxfRW dxf(filename);
        class HatchWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Hatch hatch;
                hatch.name = "ANSI31";  // Standard hatch pattern
                hatch.solid = 0;  // Pattern fill
                hatch.associative = 0;
                hatch.hstyle = 0;
                hatch.hpattern = 0;  // User-defined
                hatch.loopsnum = 1;
                hatch.angle = M_PI / 4.0;  // 45 degrees
                hatch.scale = 1.0;

                // Create boundary loop (triangle)
                DRW_HatchLoop* loop = new DRW_HatchLoop(0);

                DRW_Line* line1 = new DRW_Line();
                line1->basePoint = DRW_Coord(0.0, 0.0, 0.0);
                line1->secPoint = DRW_Coord(100.0, 0.0, 0.0);
                loop->objlist.push_back(line1);

                DRW_Line* line2 = new DRW_Line();
                line2->basePoint = DRW_Coord(100.0, 0.0, 0.0);
                line2->secPoint = DRW_Coord(50.0, 86.6, 0.0);
                loop->objlist.push_back(line2);

                DRW_Line* line3 = new DRW_Line();
                line3->basePoint = DRW_Coord(50.0, 86.6, 0.0);
                line3->secPoint = DRW_Coord(0.0, 0.0, 0.0);
                loop->objlist.push_back(line3);

                loop->update();
                hatch.appendLoop(loop);

                dxfWriter->writeHatch(&hatch);
            }
            dxfRW* dxfWriter;
        };

        HatchWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write pattern hatch" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read pattern hatch" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.hatchCount != 1) {
            std::cout << "✗ Expected 1 hatch, got " << reader.hatchCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Pattern hatch test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testHatchWithArcBoundary() {
    std::cout << "\n=== Test: Hatch with Arc Boundary ===" << std::endl;

    const char* filename = "test_hatch_arc.dxf";

    // Write hatch with arc boundary
    {
        dxfRW dxf(filename);
        class HatchWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Hatch hatch;
                hatch.name = "SOLID";
                hatch.solid = 1;
                hatch.associative = 0;
                hatch.hstyle = 0;
                hatch.hpattern = 1;
                hatch.loopsnum = 1;

                // Create boundary with arc (semicircle + line)
                DRW_HatchLoop* loop = new DRW_HatchLoop(0);

                // Arc from (-50,0) to (50,0) going through top
                DRW_Arc* arc = new DRW_Arc();
                arc->basePoint = DRW_Coord(0.0, 0.0, 0.0);  // Center
                arc->radious = 50.0;
                arc->staangle = 0.0;
                arc->endangle = M_PI;  // 180 degrees
                loop->objlist.push_back(arc);

                // Line closing the bottom
                DRW_Line* line = new DRW_Line();
                line->basePoint = DRW_Coord(-50.0, 0.0, 0.0);
                line->secPoint = DRW_Coord(50.0, 0.0, 0.0);
                loop->objlist.push_back(line);

                loop->update();
                hatch.appendLoop(loop);

                dxfWriter->writeHatch(&hatch);
            }
            dxfRW* dxfWriter;
        };

        HatchWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write hatch with arc" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read hatch with arc" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.hatchCount != 1) {
            std::cout << "✗ Expected 1 hatch, got " << reader.hatchCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Hatch with arc boundary test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testMultipleAnnotations() {
    std::cout << "\n=== Test: Multiple Annotations ===" << std::endl;

    const char* filename = "test_multi_annot.dxf";

    // Write multiple annotations
    {
        dxfRW dxf(filename);
        class AnnotWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // First leader
                DRW_Leader leader1;
                leader1.arrow = 1;
                leader1.leadertype = 0;
                leader1.flag = 3;
                leader1.vertexlist.push_back(new DRW_Coord(10.0, 10.0, 0.0));
                leader1.vertexlist.push_back(new DRW_Coord(30.0, 30.0, 0.0));
                dxfWriter->writeLeader(&leader1);
                for (size_t i = 0; i < leader1.vertexlist.size(); i++) {
                    delete leader1.vertexlist[i];
                }

                // Second leader
                DRW_Leader leader2;
                leader2.arrow = 1;
                leader2.leadertype = 0;
                leader2.flag = 3;
                leader2.vertexlist.push_back(new DRW_Coord(100.0, 10.0, 0.0));
                leader2.vertexlist.push_back(new DRW_Coord(80.0, 30.0, 0.0));
                dxfWriter->writeLeader(&leader2);
                for (size_t i = 0; i < leader2.vertexlist.size(); i++) {
                    delete leader2.vertexlist[i];
                }

                // Hatch
                DRW_Hatch hatch;
                hatch.name = "SOLID";
                hatch.solid = 1;
                hatch.associative = 0;
                hatch.loopsnum = 1;

                DRW_HatchLoop* loop = new DRW_HatchLoop(0);
                DRW_Line* l1 = new DRW_Line();
                l1->basePoint = DRW_Coord(50.0, 50.0, 0.0);
                l1->secPoint = DRW_Coord(70.0, 50.0, 0.0);
                loop->objlist.push_back(l1);
                DRW_Line* l2 = new DRW_Line();
                l2->basePoint = DRW_Coord(70.0, 50.0, 0.0);
                l2->secPoint = DRW_Coord(70.0, 70.0, 0.0);
                loop->objlist.push_back(l2);
                DRW_Line* l3 = new DRW_Line();
                l3->basePoint = DRW_Coord(70.0, 70.0, 0.0);
                l3->secPoint = DRW_Coord(50.0, 70.0, 0.0);
                loop->objlist.push_back(l3);
                DRW_Line* l4 = new DRW_Line();
                l4->basePoint = DRW_Coord(50.0, 70.0, 0.0);
                l4->secPoint = DRW_Coord(50.0, 50.0, 0.0);
                loop->objlist.push_back(l4);
                loop->update();
                hatch.appendLoop(loop);

                dxfWriter->writeHatch(&hatch);
            }
            dxfRW* dxfWriter;
        };

        AnnotWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write multiple annotations" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read multiple annotations" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.leaderCount != 2) {
            std::cout << "✗ Expected 2 leaders, got " << reader.leaderCount << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.hatchCount != 1) {
            std::cout << "✗ Expected 1 hatch, got " << reader.hatchCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Multiple annotations test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Annotation Entity Tests" << std::endl;
    std::cout << "=================================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    totalTests++;
    if (!testBasicLeader()) failedTests++;

    totalTests++;
    if (!testSplineLeader()) failedTests++;

    totalTests++;
    if (!testSolidHatch()) failedTests++;

    totalTests++;
    if (!testPatternHatch()) failedTests++;

    totalTests++;
    if (!testHatchWithArcBoundary()) failedTests++;

    totalTests++;
    if (!testMultipleAnnotations()) failedTests++;

    std::cout << "\n=================================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All annotation tests passed!" << std::endl;
        return 0;
    }
}
