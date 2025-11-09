#!/bin/bash
#
# Build script for libdxfrw
# This script is designed to run inside Docker containers
# and creates a distributable tar.gz archive
#

# Robust error handling
# -e: Exit on error
# -u: Exit on undefined variable
# -o pipefail: Fail on pipe errors
set -euo pipefail

echo "=========================================="
echo "libdxfrw Build Script"
echo "=========================================="

# Configuration
PREFIX="/opt/dxfrw"
OUTPUT_DIR="/work"
OUTPUT_FILE="dxfrw.tar.gz"

# Start timer
START_TIME=$(date +%s)

# Step 1: Generate configure script
echo ""
echo "[1/6] Generating configure script..."
autoreconf -vfi

# Step 2: Configure build
echo ""
echo "[2/6] Configuring build..."
./configure --prefix="${PREFIX}"

# Step 3: Build library
echo ""
echo "[3/6] Building library..."
make -j$(nproc)

# Step 4: Install to prefix
echo ""
echo "[4/6] Installing to ${PREFIX}..."
make install

# Step 5: Copy additional binaries if they exist
echo ""
echo "[5/6] Copying additional binaries..."
if [ -d "bin" ] && [ "$(ls -A bin 2>/dev/null)" ]; then
    mkdir -p "${PREFIX}/bin"
    cp -v bin/* "${PREFIX}/bin/" || true
else
    echo "No additional binaries found in bin/ directory"
fi

# Step 6: Create distributable archive
echo ""
echo "[6/6] Creating distributable archive..."
cd /opt
tar czf "${OUTPUT_DIR}/${OUTPUT_FILE}" dxfrw

# Set permissions to allow host user access when running in Docker
# 666 (rw-rw-rw-) ensures the file can be accessed even if Docker runs as different UID
chmod 666 "${OUTPUT_DIR}/${OUTPUT_FILE}"

# Calculate elapsed time
END_TIME=$(date +%s)
ELAPSED=$((END_TIME - START_TIME))

# Show summary
echo ""
echo "=========================================="
echo "✓ Build completed successfully!"
echo "=========================================="
echo "Output file: ${OUTPUT_DIR}/${OUTPUT_FILE}"
echo "File size:   $(du -h "${OUTPUT_DIR}/${OUTPUT_FILE}" | cut -f1)"
echo "Build time:  ${ELAPSED} seconds"
echo "=========================================="
echo ""
echo "To extract the archive:"
echo "  tar xzf ${OUTPUT_FILE} -C /opt"
echo ""

# Clean up build artifacts
echo "Cleaning up build artifacts..."
cd /work
make clean 2>/dev/null || true
