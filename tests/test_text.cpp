/******************************************************************************
**  libDXFrw - Text Entity Tests                                            **
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
#include <string>

bool testSingleLineText() {
    std::cout << "\n=== Test: Single Line Text ===" << std::endl;

    const char* filename = "test_text.dxf";

    // Write text
    {
        dxfRW dxf(filename);
        class TextWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_Text text;
                text.basePoint.x = 10.0;
                text.basePoint.y = 20.0;
                text.basePoint.z = 0.0;
                text.text = "Hello, DXF World!";
                text.height = 5.0;
                text.angle = 0.0;
                dxfWriter->writeText(&text);
            }
            dxfRW* dxfWriter;
        };

        TextWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write text" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read text" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.textCount != 1) {
            std::cout << "✗ Expected 1 text entity, got " << reader.textCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Single line text test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testMultiLineText() {
    std::cout << "\n=== Test: Multi-Line Text (MText) ===" << std::endl;

    const char* filename = "test_mtext.dxf";

    // Write MText
    {
        dxfRW dxf(filename);
        class MTextWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                DRW_MText mtext;
                mtext.basePoint.x = 50.0;
                mtext.basePoint.y = 50.0;
                mtext.basePoint.z = 0.0;
                mtext.text = "Line 1\\PLine 2\\PLine 3";  // \\P is paragraph break
                mtext.height = 3.0;
                mtext.widthscale = 1.0;
                dxfWriter->writeMText(&mtext);
            }
            dxfRW* dxfWriter;
        };

        MTextWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write MText" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read MText" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.mtextCount != 1) {
            std::cout << "✗ Expected 1 MText entity, got " << reader.mtextCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Multi-line text test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testRotatedText() {
    std::cout << "\n=== Test: Rotated Text ===" << std::endl;

    const char* filename = "test_rotated_text.dxf";

    // Write rotated text
    {
        dxfRW dxf(filename);
        class RotatedTextWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                // Text at 0 degrees
                DRW_Text text1;
                text1.basePoint.x = 0.0;
                text1.basePoint.y = 0.0;
                text1.basePoint.z = 0.0;
                text1.text = "0 degrees";
                text1.height = 5.0;
                text1.angle = 0.0;
                dxfWriter->writeText(&text1);

                // Text at 45 degrees
                DRW_Text text2;
                text2.basePoint.x = 0.0;
                text2.basePoint.y = 20.0;
                text2.basePoint.z = 0.0;
                text2.text = "45 degrees";
                text2.height = 5.0;
                text2.angle = M_PI / 4.0;  // 45 degrees in radians
                dxfWriter->writeText(&text2);

                // Text at 90 degrees
                DRW_Text text3;
                text3.basePoint.x = 0.0;
                text3.basePoint.y = 40.0;
                text3.basePoint.z = 0.0;
                text3.text = "90 degrees";
                text3.height = 5.0;
                text3.angle = M_PI / 2.0;  // 90 degrees in radians
                dxfWriter->writeText(&text3);
            }
            dxfRW* dxfWriter;
        };

        RotatedTextWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write rotated text" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read rotated text" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.textCount != 3) {
            std::cout << "✗ Expected 3 text entities, got " << reader.textCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Rotated text test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

bool testTextWithDifferentHeights() {
    std::cout << "\n=== Test: Text with Different Heights ===" << std::endl;

    const char* filename = "test_text_heights.dxf";

    // Write text with different heights
    {
        dxfRW dxf(filename);
        class TextHeightWriter : public TestInterface {
        public:
            virtual void writeEntities() {
                for (int i = 1; i <= 5; i++) {
                    DRW_Text text;
                    text.basePoint.x = 0.0;
                    text.basePoint.y = i * 15.0;
                    text.basePoint.z = 0.0;
                    text.text = "Height " + std::to_string(i * 2);
                    text.height = i * 2.0;
                    text.angle = 0.0;
                    dxfWriter->writeText(&text);
                }
            }
            dxfRW* dxfWriter;
        };

        TextHeightWriter writer;
        writer.dxfWriter = &dxf;
        if (!dxf.write(&writer, DRW::AC1015, false)) {
            std::cout << "✗ Failed to write text with different heights" << std::endl;
            return false;
        }
    }

    // Read and verify
    {
        dxfRW dxf(filename);
        TestInterface reader;
        if (!dxf.read(&reader, false)) {
            std::cout << "✗ Failed to read text with different heights" << std::endl;
            std::remove(filename);
            return false;
        }

        if (reader.textCount != 5) {
            std::cout << "✗ Expected 5 text entities, got " << reader.textCount << std::endl;
            std::remove(filename);
            return false;
        }

        std::cout << "✓ Text with different heights test passed" << std::endl;
    }

    std::remove(filename);
    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "libdxfrw Text Entity Tests" << std::endl;
    std::cout << "==========================" << std::endl;

    int failedTests = 0;
    int totalTests = 0;

    totalTests++;
    if (!testSingleLineText()) failedTests++;

    totalTests++;
    if (!testMultiLineText()) failedTests++;

    totalTests++;
    if (!testRotatedText()) failedTests++;

    totalTests++;
    if (!testTextWithDifferentHeights()) failedTests++;

    std::cout << "\n==========================" << std::endl;
    std::cout << "Tests: " << (totalTests - failedTests) << "/" << totalTests << " passed" << std::endl;

    if (failedTests > 0) {
        std::cout << "✗ " << failedTests << " test(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << "✓ All text entity tests passed!" << std::endl;
        return 0;
    }
}
