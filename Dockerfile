# Default Dockerfile for libdxfrw (AlmaLinux 9)
# For other OS options, see docker/ directory:
# - docker/Dockerfile.ubuntu      (Ubuntu 22.04 LTS)
# - docker/Dockerfile.almalinux   (AlmaLinux 9)
# - docker/Dockerfile.amazonlinux (Amazon Linux 2023)
# - docker/Dockerfile.alpine      (Alpine Linux)

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

