# libdxfrw

A free, open-source C++ library for reading and writing DXF files in both ASCII and binary formats, with support for reading DWG files from R14 to V2015.

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/gpl-2.0)

## About this Fork

This project is a fork of the original [libdxfrw project on SourceForge](http://sourceforge.net/projects/libdxfrw). This fork includes additional improvements, updated build configurations, comprehensive test suites, and enhanced documentation to make the library more accessible and easier to use.

For the original project, please visit: http://sourceforge.net/projects/libdxfrw

## Features

- **DXF Support**: Read and write DXF files in both ASCII and binary formats
- **DWG Support**: Read DWG files from AutoCAD R14 through 2015
- **Cross-Platform**: Works on Linux, macOS, Windows, and other platforms
- **Multiple Build Systems**: CMake, Autotools, Visual Studio, and MinGW support
- **Comprehensive API**: Full access to DXF entities, objects, headers, and tables
- **Open Source**: Licensed under GNU GPL v2 or later

## Table of Contents

- [About this Fork](#about-this-fork)
- [Features](#features)
- [Requirements](#requirements)
- [Building the Library](#building-the-library)
  - [Linux/macOS (CMake)](#linuxmacos-cmake)
  - [Linux/macOS (Autotools)](#linuxmacos-autotools)
  - [Windows (Visual Studio)](#windows-visual-studio)
  - [Windows (CMake)](#windows-cmake)
  - [Docker Build](#docker-build)
- [Installation](#installation)
- [Usage](#usage)
- [Examples](#examples)
- [Testing](#testing)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)

## Requirements

### Build Dependencies

- **C++ Compiler**: C++11 compatible compiler (GCC, Clang, MSVC)
- **CMake**: Version 3.10 or later (for CMake builds)
- **libiconv**: Character encoding conversion library

#### Installing Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get install build-essential cmake libiconv-hook-dev
```

**Fedora/RHEL/CentOS:**
```bash
sudo yum install gcc-c++ cmake
```

**macOS (using Homebrew):**
```bash
brew install cmake libiconv
```

**Windows:**
- Visual Studio 2013 or later, or
- MinGW with CMake

## Building the Library

### Linux/macOS (CMake)

The recommended build method using CMake:

```bash
# Create a build directory
mkdir build
cd build

# Configure the build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the library
cmake --build . --config Release

# Install (optional, requires sudo on Linux/macOS)
sudo cmake --build . --config Release --target install
```

The library will be installed to:
- Headers: `/usr/local/include/`
- Library: `/usr/local/lib/libdxfrw.a`

### Linux/macOS (Autotools)

Alternative build method using GNU Autotools:

```bash
# Generate configure script (optional, only if building from git)
autoreconf -vfi

# Configure the build
./configure

# Build the library
make

# Install (requires root/sudo)
sudo make install
```

### Windows (Visual Studio)

#### Using Visual Studio Solution:

1. Open `vs2013/libdxfrw.sln` with Visual Studio 2013 or later
2. Select your desired configuration (Debug/Release)
3. Build → Build Solution (or press F7)
4. The library will be built in `vs2013/Debug/` or `vs2013/Release/`

#### Building dwg2dxf Converter:

1. Open `dwg2dxf/vs2013/dwg2dxf.sln` with Visual Studio
2. Build Solution
3. The executable will be in the respective Debug/Release folder

### Windows (CMake)

#### Using Visual Studio 2019 or later:

1. Open Visual Studio
2. File → Open → CMake → Select `CMakeLists.txt`
3. Build → Build All

#### Using CMake Command Line:

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
cmake --build . --config Release --target install
```

### Docker Build

Build portable binaries for multiple OS distributions using Docker.

> **Note:** Dockerfiles are in `docker/` directory. Build process updated from CentOS 7 (EOL) to AlmaLinux 9.

**Available OS Distributions:**

| OS | Tag | Use Case |
|----|-----|----------|
| AlmaLinux 9 | `almalinux` | RHEL/CentOS compatible (CentOS successor) |
| Ubuntu 22.04 LTS | `ubuntu` | Debian/Ubuntu compatible |
| Amazon Linux 2023 | `amazonlinux` | AWS optimized |
| Alpine Linux | `alpine` | Lightweight musl-based |

**Quick Start:**

```bash
# Build for single OS (creates dxfrw-{os}.tar.gz)
./docker/build-docker.sh run almalinux
./docker/build-docker.sh run ubuntu

# Build for all OS distributions
./docker/build-docker.sh run all-os

# Extract and install
sudo tar xzf dxfrw-almalinux.tar.gz -C /opt
```

**Manual Docker Commands (if needed):**

```bash
# Build image and library
docker build -t codelibs/libdxfrw:ubuntu -f docker/Dockerfile.ubuntu .
docker run -t --rm -v `pwd`:/work codelibs/libdxfrw:ubuntu /work/build.sh
```

**Push to Docker Hub:**

```bash
./docker/build-docker.sh push almalinux
```

## Installation

After building, you can install the library system-wide:

### Linux/macOS:
```bash
sudo make install
# or with CMake:
sudo cmake --build build --target install
```

### Manual Installation:
Copy the headers and library files manually:
```bash
# Copy headers
sudo cp src/*.h /usr/local/include/

# Copy library
sudo cp build/libdxfrw.a /usr/local/lib/

# Update library cache (Linux only)
sudo ldconfig
```

## Usage

### Basic Reading Example

```cpp
#include "libdxfrw.h"
#include "drw_interface.h"

class MyInterface : public DRW_Interface {
public:
    virtual void addLine(const DRW_Line& data) {
        // Process line entity
        printf("Line from (%.2f, %.2f) to (%.2f, %.2f)\n",
               data.basePoint.x, data.basePoint.y,
               data.secPoint.x, data.secPoint.y);
    }

    virtual void addCircle(const DRW_Circle& data) {
        // Process circle entity
        printf("Circle at (%.2f, %.2f) radius %.2f\n",
               data.basePoint.x, data.basePoint.y, data.radious);
    }

    // Implement other virtual methods as needed...
};

int main() {
    MyInterface iface;
    dxfRW dxf("input.dxf");

    // Read the DXF file
    if (!dxf.read(&iface, false)) {
        printf("Error reading DXF file\n");
        return 1;
    }

    return 0;
}
```

### Basic Writing Example

```cpp
#include "libdxfrw.h"
#include "drw_interface.h"

class MyWriter : public DRW_Interface {
public:
    void exportToFile(const char* filename) {
        dxfRW dxf(filename);
        // Write DXF file in AutoCAD 2000 format, ASCII mode
        dxf.write(this, DRW::AC1015, false);
    }

    virtual void writeEntities() {
        // Create a line
        DRW_Line line;
        line.basePoint.x = 0;
        line.basePoint.y = 0;
        line.secPoint.x = 100;
        line.secPoint.y = 100;
        dxf->writeLine(&line);

        // Create a circle
        DRW_Circle circle;
        circle.basePoint.x = 50;
        circle.basePoint.y = 50;
        circle.radious = 25;
        dxf->writeCircle(&circle);
    }
};
```

### Linking with Your Project

**CMake:**
```cmake
find_library(DXFRW_LIBRARY NAMES dxfrw)
find_path(DXFRW_INCLUDE_DIR libdxfrw.h)

target_include_directories(your_target PRIVATE ${DXFRW_INCLUDE_DIR})
target_link_libraries(your_target ${DXFRW_LIBRARY})
```

**g++ command line:**
```bash
g++ -o myapp myapp.cpp -ldxfrw -liconv
```

## Examples

The library includes example programs that demonstrate usage:

### dwg2dxf Converter

Converts DWG files to DXF format:

```bash
cd dwg2dxf
# Build using the same method as the main library
./dwg2dxf input.dwg output.dxf
```

The source code in `dwg2dxf/` serves as a comprehensive reference implementation showing how to:
- Read DWG files
- Process entities and objects
- Write DXF output

### dwg2text Converter

Extracts text information from DWG files.

## Utility Scripts

The library includes utility scripts in the `bin/` directory for extracting text content from DXF/DWG files.

### dwg2txt - DWG to Text Converter

Extracts text entities from DWG files:

```bash
# Usage
./bin/dwg2txt <input.dwg> <output.txt>

# Example
./bin/dwg2txt drawing.dwg output.txt
```

**Requirements:**
- `dwg2text` executable (built with the library)

### dxf2txt - DXF to Text Converter

Extracts text entities from DXF files:

```bash
# Usage
./bin/dxf2txt <input.dxf> <output.txt>

# Example
./bin/dxf2txt drawing.dxf output.txt
```

**Requirements:**
- Python 3.9 or later (minimum version across all Docker build environments)
- `ezdxf` Python package: `pip install ezdxf`

**Features:**
- Supports both TEXT and MTEXT entities
- Handles multiple DXF versions (R12 through 2018+)
- Automatic encoding detection (UTF-8, CP932/Shift-JIS)
- Preserves multi-line text formatting

### Direct Python Usage

You can also use the Python script directly:

```bash
# Install ezdxf if not already installed
pip install ezdxf

# Run the script
python3 bin/dxf2txt.py input.dxf output.txt
```

## Testing

The library includes comprehensive tests covering various aspects:

### Running Tests (CMake):

```bash
cd build
cmake ..
make
ctest
```

Or run tests individually:
```bash
./test_basic        # Basic functionality tests
./test_entities     # Entity reading/writing tests
./test_polylines    # Polyline tests
./test_text         # Text entity tests
./test_tables       # Table tests
./test_blocks       # Block tests
./test_versions     # DXF version compatibility tests
./test_errors       # Error handling tests
```

### Test Files

Test files are available in the [fess-testdata repository](https://github.com/codelibs/fess-testdata/tree/master/autocad).

## Documentation

- **Original Project Website**: [http://sourceforge.net/projects/libdxfrw](http://sourceforge.net/projects/libdxfrw)
- **API Documentation**: Generate with Doxygen using `libdxfrw.dox`
- **Specifications**: See [SPECIFICATIONS.md](SPECIFICATIONS.md) for DXF format details
- **Examples**: Check the `dwg2dxf/` directory for reference implementation

### Generating API Documentation:

```bash
doxygen libdxfrw.dox
```

This creates HTML documentation in the `doc/` directory.

## Project Structure

```
libdxfrw/
├── src/              # Library source code
│   ├── intern/       # Internal implementation
│   └── *.h/cpp       # Public headers and implementation
├── include/          # Public header files
├── dwg2dxf/          # DWG to DXF converter example
├── dwg2text/         # DWG to text converter example
├── tests/            # Unit tests
├── vs2013/           # Visual Studio project files
└── CMakeLists.txt    # CMake build configuration
```

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This library is free software; you can redistribute it and/or modify it under the terms of the **GNU General Public License** as published by the Free Software Foundation; either **version 2 of the License**, or (at your option) any later version.

See the [COPYING](COPYING) file for the full license text.

## Authors

See [AUTHORS](AUTHORS) file for the list of contributors.

## Changelog

See [ChangeLog](ChangeLog) for version history and changes.

## Support

### This Fork
- **Issues**: Report bugs and request features on GitHub Issues
- **Repository**: [https://github.com/codelibs/libdxfrw](https://github.com/codelibs/libdxfrw)

### Original Project
- **Website**: [http://sourceforge.net/projects/libdxfrw](http://sourceforge.net/projects/libdxfrw)
