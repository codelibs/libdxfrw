#!/bin/bash
#
# Docker build script for libdxfrw
# Supports multiple OS/distributions
#
# Usage:
#   ./docker/build-docker.sh [options] [os]
#
# Options:
#   build     - Build Docker image only
#   run       - Build library using Docker
#   push      - Push image to Docker Hub
#   all       - Build image, run build, and push (default)
#
# OS options:
#   ubuntu       - Ubuntu 22.04 LTS
#   almalinux    - AlmaLinux 9 (CentOS successor)
#   amazonlinux  - Amazon Linux 2023
#   alpine       - Alpine Linux (musl-based)
#   all-os       - Build for all OS distributions
#
# Examples:
#   ./docker/build-docker.sh build ubuntu
#   ./docker/build-docker.sh run almalinux
#   ./docker/build-docker.sh all amazonlinux
#   ./docker/build-docker.sh run all-os
#

# Robust error handling
# -e: Exit on error
# -u: Exit on undefined variable
# -o pipefail: Fail on pipe errors
set -euo pipefail

# Configuration
IMAGE_NAME="codelibs/libdxfrw"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Available OS distributions
declare -A OS_LIST=(
    ["ubuntu"]="Dockerfile.ubuntu"
    ["almalinux"]="Dockerfile.almalinux"
    ["amazonlinux"]="Dockerfile.amazonlinux"
    ["alpine"]="Dockerfile.alpine"
)

# Function to print usage
usage() {
    cat << EOF
Usage: $0 [action] [os]

Actions:
  build        Build Docker image only
  run          Build library using Docker (creates tar.gz)
  push         Push image to Docker Hub
  all          Build image, run build, and push (default)

OS Distributions:
  ubuntu       Ubuntu 22.04 LTS (Debian-based)
  almalinux    AlmaLinux 9 (RHEL-based, CentOS successor)
  amazonlinux  Amazon Linux 2023 (AWS optimized)
  alpine       Alpine Linux (lightweight, musl-based)
  all-os       Build for all distributions

Examples:
  $0 build ubuntu           # Build Ubuntu image only
  $0 run almalinux          # Build library on AlmaLinux
  $0 all amazonlinux        # Build image, library, and push
  $0 run all-os             # Build library for all OS distributions

EOF
    exit 1
}

# Function to build Docker image
build_image() {
    local os=$1
    local dockerfile=$2
    local tag="${IMAGE_NAME}:${os}"

    echo "=========================================="
    echo "Building Docker image: ${tag}"
    echo "Dockerfile: ${dockerfile}"
    echo "=========================================="

    docker build --rm -t "${tag}" -f "${SCRIPT_DIR}/${dockerfile}" "${PROJECT_ROOT}"

    echo "✓ Image built successfully: ${tag}"
}

# Function to run build inside Docker
run_build() {
    local os=$1
    local tag="${IMAGE_NAME}:${os}"
    local output_file="dxfrw-${os}.tar.gz"

    echo "=========================================="
    echo "Building library using: ${tag}"
    echo "Output: ${output_file}"
    echo "=========================================="

    docker run -t --rm -v "${PROJECT_ROOT}:/work" "${tag}" /work/build.sh

    # Rename output file to include OS name
    if [ -f "${PROJECT_ROOT}/dxfrw.tar.gz" ]; then
        mv "${PROJECT_ROOT}/dxfrw.tar.gz" "${PROJECT_ROOT}/${output_file}"
        echo "✓ Build completed: ${output_file}"
    else
        echo "✗ Error: Build output not found"
        return 1
    fi
}

# Function to push Docker image
push_image() {
    local os=$1
    local tag="${IMAGE_NAME}:${os}"

    echo "=========================================="
    echo "Pushing Docker image: ${tag}"
    echo "=========================================="

    docker push "${tag}"

    echo "✓ Image pushed successfully: ${tag}"
}

# Function to process single OS
process_os() {
    local action=$1
    local os=$2
    local dockerfile=${OS_LIST[$os]}

    if [ -z "$dockerfile" ]; then
        echo "Error: Unknown OS '${os}'"
        echo "Available OS: ${!OS_LIST[@]}"
        exit 1
    fi

    case $action in
        build)
            build_image "$os" "$dockerfile"
            ;;
        run)
            build_image "$os" "$dockerfile"
            run_build "$os"
            ;;
        push)
            push_image "$os"
            ;;
        all)
            build_image "$os" "$dockerfile"
            run_build "$os"
            push_image "$os"
            ;;
        *)
            echo "Error: Unknown action '${action}'"
            usage
            ;;
    esac
}

# Main script
ACTION=${1:-all}
OS=${2:-almalinux}

# Handle special case: all-os
if [ "$OS" = "all-os" ]; then
    echo "=========================================="
    echo "Processing all OS distributions"
    echo "=========================================="

    for os in "${!OS_LIST[@]}"; do
        echo ""
        process_os "$ACTION" "$os"
    done

    echo ""
    echo "=========================================="
    echo "✓ All OS distributions processed"
    echo "=========================================="
    ls -lh "${PROJECT_ROOT}"/dxfrw-*.tar.gz 2>/dev/null || true
else
    process_os "$ACTION" "$OS"
fi
