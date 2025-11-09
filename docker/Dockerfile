# Default Dockerfile for libdxfrw (AlmaLinux 9)
# This is a symbolic/default build configuration
# For OS-specific builds, use the corresponding Dockerfile:
#   - Dockerfile.ubuntu      (Ubuntu 22.04 LTS)
#   - Dockerfile.almalinux   (AlmaLinux 9 - RHEL/CentOS compatible)
#   - Dockerfile.amazonlinux (Amazon Linux 2023)
#   - Dockerfile.alpine      (Alpine Linux - lightweight)

FROM almalinux:9

LABEL maintainer="CodeLibs Project"
LABEL description="Build environment for libdxfrw on AlmaLinux 9 (CentOS successor)"

# Install build dependencies
RUN dnf -y groupinstall "Development Tools" && \
    dnf -y install \
    autoconf \
    automake \
    libtool \
    pkg-config \
    && dnf clean all

WORKDIR /work

# Default command
CMD ["sh", "/work/build.sh"]
