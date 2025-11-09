# Docker Build Configuration

This directory contains Docker build configurations for libdxfrw supporting multiple OS distributions.

## Files

- **`Dockerfile.ubuntu`** - Ubuntu 22.04 LTS (Debian-based)
- **`Dockerfile.almalinux`** - AlmaLinux 9 (RHEL/CentOS compatible, CentOS successor)
- **`Dockerfile.amazonlinux`** - Amazon Linux 2023 (AWS optimized)
- **`Dockerfile.alpine`** - Alpine Linux (lightweight, musl-based)
- **`build-docker.sh`** - Automated build script for all distributions

## Quick Usage

```bash
# Build for single OS
./build-docker.sh run almalinux

# Build for all OS
./build-docker.sh run all-os

# See all options
./build-docker.sh
```

## Output

Each build creates a tar.gz archive in the project root:
- `dxfrw-ubuntu.tar.gz`
- `dxfrw-almalinux.tar.gz`
- `dxfrw-amazonlinux.tar.gz`
- `dxfrw-alpine.tar.gz`

## OS Selection Guide

| Choose | When you need |
|--------|---------------|
| **AlmaLinux** | RHEL/CentOS/Fedora compatible binaries |
| **Ubuntu** | Debian/Ubuntu compatible binaries |
| **Amazon Linux** | AWS EC2 or AWS environment deployment |
| **Alpine** | Minimal footprint, container-optimized builds |

## Manual Build

If you need to customize the build:

```bash
# Build image
docker build -t codelibs/libdxfrw:ubuntu -f Dockerfile.ubuntu ..

# Run build
docker run -t --rm -v $(pwd)/..:/work codelibs/libdxfrw:ubuntu /work/build.sh
```

Note: Manual builds should be run from the project root, not from this directory.
