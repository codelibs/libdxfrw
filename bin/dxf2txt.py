#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
dxf2txt.py - Extract text content from DXF files

This script reads a DXF file and extracts all text entities (TEXT and MTEXT)
to a text file. It handles different DXF versions and character encodings.

Usage:
    python3 dxf2txt.py <input.dxf> <output.txt>

Requirements:
    - Python 3.6+
    - ezdxf package (pip install ezdxf)

Author: CodeLibs Project
"""

import re
import sys
import ezdxf
from pathlib import Path


def show_usage():
    """Display usage information"""
    print(__doc__)
    sys.exit(1)


def extract_text_from_dxf(input_file, output_file):
    """
    Extract text entities from DXF file and save to text file

    Args:
        input_file: Path to input DXF file
        output_file: Path to output text file
    """
    # Check if input file exists
    if not Path(input_file).exists():
        print(f"Error: Input file not found: {input_file}", file=sys.stderr)
        sys.exit(1)

    try:
        # Try reading with UTF-8 encoding first
        dwg = ezdxf.readfile(input_file, encoding='utf-8')

        # For older DXF versions, try CP932 encoding (Japanese Shift-JIS)
        old_enc_versions = ['AC1009', 'AC1015', 'AC1018']
        if dwg.dxfversion in old_enc_versions:
            try:
                dwg = ezdxf.readfile(input_file, encoding='cp932')
            except:
                pass  # Keep UTF-8 version if CP932 fails

        # Extract text entities
        texts = []
        modelspace = dwg.modelspace()

        for entity in modelspace:
            if entity.dxftype() == 'TEXT':
                # Simple text entity
                texts.append(entity.dxf.text)
            elif entity.dxftype() == 'MTEXT':
                # Multi-line text entity
                # Remove formatting codes and convert \P to newlines
                text = re.sub(r'\{\\f[^;]+;([^}]+)\}', r'\1', entity.plain_text())
                text = text.replace('\\P', '\n')
                texts.append(text)

        # Write extracted text to output file
        with open(output_file, 'w', encoding='utf-8') as f:
            for text in texts:
                f.write(text + '\n')

        print(f"Extracted {len(texts)} text entities from {dwg.dxfversion} format DXF")

    except ezdxf.DXFError as e:
        print(f"Error: Failed to read DXF file: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


def main():
    """Main entry point"""
    # Check arguments
    if len(sys.argv) != 3:
        print(f"Error: Invalid arguments", file=sys.stderr)
        show_usage()

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    extract_text_from_dxf(input_file, output_file)


if __name__ == '__main__':
    main()

