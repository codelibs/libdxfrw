# libdxfrw - Technical Specifications

## Project Overview

**Project Name**: libdxfrw
**Version**: 0.6.3
**License**: GNU General Public License v2 (or later)
**Original Author**: José F. Soriano (Rallaz)
**Maintainers**: codelibs, Nicu Tofan, Miguel E. Hernández Cuervo

### Purpose

libdxfrw is a free C++ library for reading and writing DXF (Drawing eXchange Format) files in both ASCII and binary formats. It also supports reading DWG (AutoCAD drawing) files from versions R14 through V2015.

### Main Features

1. **DXF File Read/Write**
   - Read and write DXF files in ASCII format
   - Read and write DXF files in binary format
   - Support for multiple DXF versions: R12, 2000, 2004, 2007, 2010

2. **DWG File Reading**
   - Supported versions: R14 to V2015
   - Version-specific reader implementations:
     - dwgReader15: R14 version
     - dwgReader18: R2004 version
     - dwgReader21: R2007 version
     - dwgReader24: R2010 version
     - dwgReader27: R2013/R2015 versions

3. **Supported CAD Drawing Entities**
   - Basic shapes: Points, Lines, Rays, XLines
   - Curves: Circles, Arcs, Ellipses, Splines
   - Polylines: 2D/3D Lightweight Polylines
   - 3D shapes: 3D Faces, Traces, Solids
   - Text: Single-line text, Multi-line text
   - Annotations: Hatches, Dimensions, Leaders
   - Others: Block references, Images, Viewports

4. **Supported CAD Objects**
   - Layers
   - Line Types
   - Text Styles
   - Dimension Styles
   - Viewports
   - Application IDs
   - Block Definitions

---

## Architecture

### Core Class Structure

```
dxfRW (Main API Class)
├── DRW_Interface (Abstract Interface)
├── dxfReader/dxfWriter (DXF I/O Handlers)
├── dwgReader (DWG Reader Base)
│   ├── dwgReader15 (R14)
│   ├── dwgReader18 (R2004)
│   ├── dwgReader21 (R2007)
│   ├── dwgReader24 (R2010)
│   └── dwgReader27 (R2013-2015)
├── DRW_Header (Header Variables)
├── DRW_Entity (Geometric Entities)
│   ├── DRW_Point
│   ├── DRW_Line
│   ├── DRW_Arc
│   ├── DRW_Circle
│   ├── DRW_Ellipse
│   ├── DRW_LWPolyline
│   ├── DRW_Polyline
│   ├── DRW_Spline
│   ├── DRW_Text
│   ├── DRW_MText
│   ├── DRW_Hatch
│   ├── DRW_Dimension (various dimension types)
│   └── Other entities
└── DRW_Object (Non-geometric Objects)
    ├── DRW_Layer
    ├── DRW_LType
    ├── DRW_Textstyle
    ├── DRW_Dimstyle
    └── Other objects
```

### Directory Structure

```
libdxfrw/
├── src/                      # Library source code
│   ├── libdxfrw.h/cpp       # Main API class
│   ├── drw_interface.h      # Abstract interface
│   ├── drw_entities.h/cpp   # Entity definitions
│   ├── drw_objects.h/cpp    # Object definitions
│   ├── drw_header.h/cpp     # Header variables
│   ├── drw_base.h           # Basic data structures
│   └── intern/              # Internal implementation
│       ├── dxfreader.h/cpp     # DXF reader
│       ├── dxfwriter.h/cpp     # DXF writer
│       ├── dwgreader*.h/cpp    # DWG readers
│       ├── dwgbuffer.h/cpp     # DWG buffer
│       ├── drw_textcodec.h/cpp # Character encoding
│       └── Other utilities
├── dwg2dxf/                  # DWG to DXF converter tool
├── dwg2text/                 # DWG to text extractor tool
├── bin/                      # Executable scripts
└── vs2013/                   # Visual Studio 2013 project
```

---

## API Specifications

### Main Class: dxfRW

#### Constructor

```cpp
dxfRW(const char* name);
```

- **Parameters**: `name` - File name to process
- **Description**: Creates an instance for processing DXF/DWG files

#### Main Methods

##### Reading

```cpp
bool read(DRW_Interface *interface_, bool ext);
```

- **Parameters**:
  - `interface_`: Callback interface implementation
  - `ext`: Apply extrusion to convert to 2D
- **Returns**: true on success, false on failure
- **Description**: Reads the file and calls interface methods for each entity

##### Writing

```cpp
bool write(DRW_Interface *interface_, DRW::Version ver, bool bin);
```

- **Parameters**:
  - `interface_`: Interface implementation providing data
  - `ver`: DXF version (R12, 2000, 2004, 2007, 2010)
  - `bin`: Output in binary format
- **Returns**: true on success, false on failure

##### Entity Writing Methods

```cpp
bool writePoint(DRW_Point *ent);
bool writeLine(DRW_Line *ent);
bool writeCircle(DRW_Circle *ent);
bool writeArc(DRW_Arc *ent);
bool writeEllipse(DRW_Ellipse *ent);
bool writeLWPolyline(DRW_LWPolyline *ent);
bool writeSpline(DRW_Spline *ent);
bool writeText(DRW_Text *ent);
bool writeMText(DRW_MText *ent);
bool writeHatch(DRW_Hatch *ent);
// ... many more
```

##### Table Object Writing Methods

```cpp
bool writeLayer(DRW_Layer *ent);
bool writeLineType(DRW_LType *ent);
bool writeTextstyle(DRW_Textstyle *ent);
bool writeDimstyle(DRW_Dimstyle *ent);
bool writeVport(DRW_Vport *ent);
bool writeAppId(DRW_AppId *ent);
```

### Interface: DRW_Interface

Applications must inherit from this interface and implement callback methods.

#### Required Methods

##### Header

```cpp
virtual void addHeader(const DRW_Header* data) = 0;
```

##### Table Objects

```cpp
virtual void addLayer(const DRW_Layer& data) = 0;
virtual void addLType(const DRW_LType& data) = 0;
virtual void addDimStyle(const DRW_Dimstyle& data) = 0;
virtual void addTextStyle(const DRW_Textstyle& data) = 0;
virtual void addVport(const DRW_Vport& data) = 0;
virtual void addAppId(const DRW_AppId& data) = 0;
```

##### Blocks

```cpp
virtual void addBlock(const DRW_Block& data) = 0;
virtual void setBlock(const int handle) = 0;
virtual void endBlock() = 0;
```

##### Entities

```cpp
virtual void addPoint(const DRW_Point& data) = 0;
virtual void addLine(const DRW_Line& data) = 0;
virtual void addRay(const DRW_Ray& data) = 0;
virtual void addXline(const DRW_Xline& data) = 0;
virtual void addCircle(const DRW_Circle& data) = 0;
virtual void addArc(const DRW_Arc& data) = 0;
virtual void addEllipse(const DRW_Ellipse& data) = 0;
virtual void addLWPolyline(const DRW_LWPolyline& data) = 0;
virtual void addPolyline(const DRW_Polyline& data) = 0;
virtual void addSpline(const DRW_Spline* data) = 0;
virtual void addInsert(const DRW_Insert& data) = 0;
virtual void addTrace(const DRW_Trace& data) = 0;
virtual void add3dFace(const DRW_3Dface& data) = 0;
virtual void addSolid(const DRW_Solid& data) = 0;
virtual void addMText(const DRW_MText& data) = 0;
virtual void addText(const DRW_Text& data) = 0;
virtual void addHatch(const DRW_Hatch *data) = 0;
virtual void addViewport(const DRW_Viewport& data) = 0;
virtual void addImage(const DRW_Image *data) = 0;
```

##### Dimensions

```cpp
virtual void addDimAlign(const DRW_DimAligned *data) = 0;
virtual void addDimLinear(const DRW_DimLinear *data) = 0;
virtual void addDimRadial(const DRW_DimRadial *data) = 0;
virtual void addDimDiametric(const DRW_DimDiametric *data) = 0;
virtual void addDimAngular(const DRW_DimAngular *data) = 0;
virtual void addDimAngular3P(const DRW_DimAngular3p *data) = 0;
virtual void addDimOrdinate(const DRW_DimOrdinate *data) = 0;
```

##### Others

```cpp
virtual void addLeader(const DRW_Leader *data) = 0;
virtual void linkImage(const DRW_ImageDef *data) = 0;
virtual void addComment(const char* comment) = 0;
```

##### Writing Methods

```cpp
virtual void writeHeader(DRW_Header& data) = 0;
virtual void writeBlocks() = 0;
virtual void writeBlockRecords() = 0;
virtual void writeEntities() = 0;
virtual void writeLTypes() = 0;
virtual void writeLayers() = 0;
virtual void writeTextstyles() = 0;
virtual void writeVports() = 0;
virtual void writeDimstyles() = 0;
virtual void writeAppId() = 0;
```

---

## Build Systems

### 1. Autotools (Recommended)

```bash
autoreconf -vfi
./configure
make
sudo make install
```

#### Configuration Files
- `configure.ac`: Autoconf configuration
- `Makefile.am`: Automake templates
- Library version: 0.6.3 (CURRENT=6, REVISION=3, AGE=0)

#### Dependencies
- **Required**: libiconv (character conversion)
- **Standard Library**: stdlib.h, string.h
- **Math Functions**: sqrt()

### 2. CMake

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
sudo cmake --build . --config Release --target install
```

#### Key CMake Settings
- Minimum CMake version: 3.10
- Build target: Static library `dxfrw`
- Installation paths:
  - Headers: `include/`
  - Library: `lib/` (Linux/macOS), `Debug/lib/` or `Release/lib/` (Windows)

### 3. Visual Studio 2013

- Solution file: `vs2013/libdxfrw.sln`
- Project file: `vs2013/libdxfrw.vcxproj`
- NuGet package: libiconv 1.14.0.11

### 4. Docker

```bash
# Build image
docker build --rm -t codelibs/libdxfrw .

# Build library
docker run -t --rm -v `pwd`:/work codelibs/libdxfrw:latest /work/build.sh
```

- Base image: CentOS 7
- Output: `dxfrw.tar.gz` (install location: `/opt`)

---

## Utility Tools

### 1. dwg2dxf

Command-line tool to convert DWG files to DXF format.

```bash
dwg2dxf [options] input.dwg [output.dxf]
```

**Files**:
- `dwg2dxf/main.cpp`: Entry point
- `dwg2dxf/dx_iface.h/cpp`: Interface implementation
- `dwg2dxf/dwg2dxf.1`: Man page

### 2. dwg2text

Tool to extract text information from DWG files.

```bash
dwg2text input.dwg
```

**Files**:
- `dwg2text/main.cpp`: Entry point
- `dwg2text/dx_iface.h/cpp`: Interface implementation
- `dwg2text/dwg2text.1`: Man page

### 3. dxf2txt.py

Python script to dump DXF files in text format.

```bash
python bin/dxf2txt.py input.dxf
```

---

## Data Structures

### Basic Types

```cpp
namespace DRW {
    enum Version {
        UNKNOWNV,    // Unknown
        R12,         // R12 DXF
        R14,         // R14 DXF
        R2000,       // R2000 DXF
        R2004,       // R2004 DXF
        R2007,       // R2007 DXF
        R2010,       // R2010 DXF
        R2013,       // R2013 DXF
        R2015        // R2015 DXF
    };
}
```

### Entity Base Class

```cpp
class DRW_Entity {
public:
    enum DRW_EntityType {
        POINT, LINE, CIRCLE, ARC, ELLIPSE,
        LWPOLYLINE, POLYLINE, SPLINE,
        INSERT, TEXT, MTEXT, HATCH,
        DIMENSION, LEADER, VIEWPORT, IMAGE,
        // ... others
    };

    DRW_Coord basePoint;      // Base point
    std::string layer;        // Layer name
    std::string lineType;     // Line type
    int color;                // Color number
    double thickness;         // Thickness
    double ltypeScale;        // Line type scale
    int handle;               // Handle
    // ... other properties
};
```

### Coordinate Type

```cpp
class DRW_Coord {
public:
    double x;
    double y;
    double z;
};
```

---

## Character Encoding Support

libdxfrw supports multiple character code pages:

- **CP932**: Japanese (Shift-JIS)
- **CP936**: Simplified Chinese (GB2312)
- **CP949**: Korean
- **CP950**: Traditional Chinese (Big5)

### Implementation Files

- `src/intern/drw_textcodec.h/cpp`: Character encoding conversion engine
- `src/intern/drw_cptable932.h`: Japanese code page
- `src/intern/drw_cptable936.h`: Simplified Chinese
- `src/intern/drw_cptable949.h`: Korean
- `src/intern/drw_cptable950.h`: Traditional Chinese
- `src/intern/drw_cptables.h`: Code page table integration

---

## Error Handling and Debugging

### Debug Level Settings

```cpp
void setDebug(DRW::DBG_LEVEL lvl);
```

**Debug Levels**:
- `DRW::NONE`: No debug output
- `DRW::DEBUG`: Debug information output

### Debug Utilities

- `src/intern/drw_dbg.h/cpp`: Debug output utilities

---

## Performance Considerations

### Buffering

- `dwgBuffer`: Efficient binary data processing when reading DWG files
- Supports bit-level read operations

### Memory Management

- Entities and objects are passed by pointer or reference
- Memory management is handled by the interface implementation side

### Ellipse to Polyline Conversion

```cpp
void setEllipseParts(int parts);
```

Set the number of segments when converting ellipses to polylines. Increasing the number of segments improves accuracy.

---

## Test Data

External test data repository:
- [fess-testdata/autocad](https://github.com/codelibs/fess-testdata/tree/master/autocad)

Contains sample files of various DXF/DWG file versions, useful for integration testing.

---

## Known Limitations

1. **DWG File Writing Not Supported**: Only DWG file reading is supported
2. **Limited Entity Support**: May not support all entities from the latest AutoCAD versions
3. **Reed-Solomon Decoding**: Error correction functionality for DWG files is limited

---

## Version History

See the `ChangeLog` file for details.

Major development milestones:
- **2011**: Project started
- **2013**: Added DWG R2004 support
- **2014**: Added DWG R2010/R2013 support
- **2015**: Added DWG R2015 support
- **2020s**: Ongoing community maintenance

---

## License

GNU General Public License v2.0 (or later)

See the `COPYING` file for details.

---

## Contributors

- **José F. Soriano (Rallaz)**: Original author
- **Nicu Tofan**: Contributor
- **Miguel E. Hernández Cuervo**: Contributor
- **codelibs**: Maintainer

---

## References

- Official website: http://sourceforge.net/projects/libdxfrw
- GitHub: https://github.com/codelibs/libdxfrw
- DXF Reference: AutoCAD DXF Specification (Autodesk official documentation)

---

## Support and Community

Report issues and submit pull requests through the GitHub repository:
https://github.com/codelibs/libdxfrw/issues

---

Last Updated: 2025-11-08
