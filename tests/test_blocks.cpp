/******************************************************************************
**  libDXFrw - Block Tests                                                  **
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

bool testBasicBlock() {
    std::cout << "\n=== Test: Basic Block Definition ===" << std::endl;

    const char* filename = "test_block.dxf";

    // Write block
    {
        dxfRW dxf(filename);
        class BlockWriter : public TestInterface {
        public:
            int blockCount = 0;

            virtual void writeBlockRecords() {
                dxfWriter->writeBlockRecord("SimpleBlock");
            }

            virtual void writeBlocks() {
                // Define a simple block with a circle
                DRW_Block block;
                block.name = "SimpleBlock";
                block.basePoint.x = 0.0;
                block.basePoint.y = 0.0;
                block.basePoint.z = 0.0;
                dxfWriter->writeBlock(&block);

                // Add entities to block
                DRW_Circle circle;
                circle.basePoint.x = 0.0;
                circle.basePoint.y = 0.0;
                circle.basePoint.z = 0.0;
                circle.radious = 10.0;
                dxfWriter->writeCircle(&circle);

                DRW_Line line1;
                line1.basePoint.x = -10.0;
                line1.basePoint.y = 0.0;
                line1.basePoint.z = 0.0;
                line1.secPoint.x = 10.0;
                line1.secPoint.y = 0.0;
                line1.secPoint.z = 0.0;
                dxfWriter->writeLine(&line1);

                DRW_Line line2;
                line2.basePoint.x = 0.0;
                line2.basePoint.y = -10.0;
                line2.basePoint.z = 0.0;
                line2.secPoint.x = 0.0;
                line2.secPoint.y = 10.0;
                line2.secPoint.z = 0.0;
                dxfWriter->writeLine(&line2);
            }

            virtual void writeEntities() {
                // Insert block instance
                DRW_Insert insert;
                insert.name = "SimpleBlock";
                insert.basePoint.x = 50.0;
                insert.basePoint.y = 50.0;
                insert.basePoint.z = 0.0;
                insert.xscale = 1.0;
                insert.yscale = 1.0;
                insert.zscale = 1.0;
                insert.angle = 0.0;
                dxfWriter->writeInsert(&insert);
            }

            dxfRW* dxfWriter;
        };

        BlockWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write block" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read block" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.insertCount != 1) {
            std::cout << "✗ Expected 1 block insert, got " << reader.insertCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Basic block test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testMultipleBlockInserts() {
    std::cout << "\n=== Test: Multiple Block Inserts ===" << std::endl;

    const char* filename = "test_multiple_blocks.dxf";

    // Write multiple block instances
    {
        dxfRW dxf(filename);
        class MultiBlockWriter : public TestInterface {
        public:
            virtual void writeBlockRecords() {
                dxfWriter->writeBlockRecord("Square");
            }

            virtual void writeBlocks() {
                // Define a square block
                DRW_Block block;
                block.name = "Square";
                block.basePoint.x = 0.0;
                block.basePoint.y = 0.0;
                block.basePoint.z = 0.0;
                dxfWriter->writeBlock(&block);

                // Square outline
                DRW_LWPolyline square;
                square.vertlist.push_back(new DRW_Vertex2D(0.0, 0.0, 0.0));
                square.vertlist.push_back(new DRW_Vertex2D(10.0, 0.0, 0.0));
                square.vertlist.push_back(new DRW_Vertex2D(10.0, 10.0, 0.0));
                square.vertlist.push_back(new DRW_Vertex2D(0.0, 10.0, 0.0));
                square.flags = 1;  // Closed
                dxfWriter->writeLWPolyline(&square);

                for (size_t i = 0; i < square.vertlist.size(); i++) {
                    delete square.vertlist[i];
                }
            }

            virtual void writeEntities() {
                // Insert block at different positions and scales
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        DRW_Insert insert;
                        insert.name = "Square";
                        insert.basePoint.x = i * 30.0;
                        insert.basePoint.y = j * 30.0;
                        insert.basePoint.z = 0.0;
                        insert.xscale = 1.0 + i * 0.5;
                        insert.yscale = 1.0 + j * 0.5;
                        insert.zscale = 1.0;
                        insert.angle = 0.0;
                        dxfWriter->writeInsert(&insert);
                    }
                }
            }

            dxfRW* dxfWriter;
        };

        MultiBlockWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write multiple blocks" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read multiple blocks" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.insertCount != 9) {
            std::cout << "✗ Expected 9 block inserts, got " << reader.insertCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Multiple block inserts test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testRotatedAndScaledBlocks() {
    std::cout << "\n=== Test: Rotated and Scaled Blocks ===" << std::endl;

    const char* filename = "test_rotated_blocks.dxf";

    // Write rotated and scaled blocks
    {
        dxfRW dxf(filename);
        class RotatedBlockWriter : public TestInterface {
        public:
            virtual void writeBlockRecords() {
                dxfWriter->writeBlockRecord("Arrow");
            }

            virtual void writeBlocks() {
                // Define an arrow block
                DRW_Block block;
                block.name = "Arrow";
                block.basePoint.x = 0.0;
                block.basePoint.y = 0.0;
                block.basePoint.z = 0.0;
                dxfWriter->writeBlock(&block);

                // Arrow shaft
                DRW_Line shaft;
                shaft.basePoint.x = 0.0;
                shaft.basePoint.y = 0.0;
                shaft.basePoint.z = 0.0;
                shaft.secPoint.x = 20.0;
                shaft.secPoint.y = 0.0;
                shaft.secPoint.z = 0.0;
                dxfWriter->writeLine(&shaft);

                // Arrow head
                DRW_Line head1;
                head1.basePoint.x = 20.0;
                head1.basePoint.y = 0.0;
                head1.basePoint.z = 0.0;
                head1.secPoint.x = 15.0;
                head1.secPoint.y = 3.0;
                head1.secPoint.z = 0.0;
                dxfWriter->writeLine(&head1);

                DRW_Line head2;
                head2.basePoint.x = 20.0;
                head2.basePoint.y = 0.0;
                head2.basePoint.z = 0.0;
                head2.secPoint.x = 15.0;
                head2.secPoint.y = -3.0;
                head2.secPoint.z = 0.0;
                dxfWriter->writeLine(&head2);
            }

            virtual void writeEntities() {
                // Insert arrow at 0 degrees
                DRW_Insert insert1;
                insert1.name = "Arrow";
                insert1.basePoint.x = 50.0;
                insert1.basePoint.y = 50.0;
                insert1.basePoint.z = 0.0;
                insert1.xscale = 1.0;
                insert1.yscale = 1.0;
                insert1.zscale = 1.0;
                insert1.angle = 0.0;
                dxfWriter->writeInsert(&insert1);

                // Insert arrow at 90 degrees
                DRW_Insert insert2;
                insert2.name = "Arrow";
                insert2.basePoint.x = 100.0;
                insert2.basePoint.y = 50.0;
                insert2.basePoint.z = 0.0;
                insert2.xscale = 1.0;
                insert2.yscale = 1.0;
                insert2.zscale = 1.0;
                insert2.angle = M_PI / 2.0;  // 90 degrees
                dxfWriter->writeInsert(&insert2);

                // Insert scaled arrow at 45 degrees
                DRW_Insert insert3;
                insert3.name = "Arrow";
                insert3.basePoint.x = 75.0;
                insert3.basePoint.y = 75.0;
                insert3.basePoint.z = 0.0;
                insert3.xscale = 2.0;
                insert3.yscale = 2.0;
                insert3.zscale = 1.0;
                insert3.angle = M_PI / 4.0;  // 45 degrees
                dxfWriter->writeInsert(&insert3);
            }

            dxfRW* dxfWriter;
        };

        RotatedBlockWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write rotated blocks" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read rotated blocks" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.insertCount != 3) {
            std::cout << "✗ Expected 3 block inserts, got " << reader.insertCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Rotated and scaled blocks test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Block Tests" << std::endl;
    std::cout << "====================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    totalTests++;
    if (!testBasicBlock()) failedTests++;

    totalTests++;
    if (!testMultipleBlockInserts()) failedTests++;

    totalTests++;
    if (!testRotatedAndScaledBlocks()) failedTests++;

    std::cout << "\n====================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All block tests passed!" << std::endl;
        return 0;
    }
}
