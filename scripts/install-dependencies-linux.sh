#!/bin/bash
# RDX Dependency Installation Script for Linux

echo "RDX Dependency Installation Script"
echo "==================================="
echo ""

# Detect Linux distribution
if [ -f /etc/debian_version ]; then
    echo "Detected Debian/Ubuntu system"
    echo ""
    echo "Installing dependencies via apt..."
    sudo apt-get update
    sudo apt-get install -y \
        build-essential \
        cmake \
        qt6-base-dev \
        qt6-declarative-dev \
        libsqlite3-dev \
        libzstd-dev \
        pkg-config
elif [ -f /etc/redhat-release ]; then
    echo "Detected RHEL/CentOS system"
    echo ""
    echo "Installing dependencies via yum/dnf..."
    if command -v dnf &> /dev/null; then
        sudo dnf install -y \
            gcc-c++ \
            cmake \
            qt6-qtbase-devel \
            qt6-qtdeclarative-devel \
            sqlite-devel \
            libzstd-devel \
            pkgconfig
    else
        sudo yum install -y \
            gcc-c++ \
            cmake \
            qt6-qtbase-devel \
            qt6-qtdeclarative-devel \
            sqlite-devel \
            libzstd-devel \
            pkgconfig
    fi
elif [ -f /etc/arch-release ]; then
    echo "Detected Arch Linux system"
    echo ""
    echo "Installing dependencies via pacman..."
    sudo pacman -S --needed \
        base-devel \
        cmake \
        qt6-base \
        qt6-declarative \
        sqlite \
        zstd
else
    echo "Unknown Linux distribution. Please install manually:"
    echo "- CMake 3.20+"
    echo "- Qt6 (Core, Quick, QML)"
    echo "- SQLite3 development libraries"
    echo "- zstd development libraries"
    exit 1
fi

echo ""
echo "Dependencies installed successfully!"
echo ""
echo "Next steps:"
echo "1. mkdir build && cd build"
echo "2. cmake .."
echo "3. cmake --build ."

