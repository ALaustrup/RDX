#!/bin/bash
# RDX Dependency Installation Script for macOS

echo "RDX Dependency Installation Script"
echo "==================================="
echo ""

# Check for Homebrew
if ! command -v brew &> /dev/null; then
    echo "Homebrew not found. Please install Homebrew first:"
    echo "/bin/bash -c \"\$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)\""
    exit 1
fi

echo "Installing dependencies via Homebrew..."
echo ""

# Install CMake if not present
if ! command -v cmake &> /dev/null; then
    echo "Installing CMake..."
    brew install cmake
fi

# Install Qt6
echo "Installing Qt6..."
brew install qt@6

# Install SQLite3
echo "Installing SQLite3..."
brew install sqlite

# Install zstd
echo "Installing zstd..."
brew install zstd

echo ""
echo "Dependencies installed successfully!"
echo ""
echo "Next steps:"
echo "1. mkdir build && cd build"
echo "2. cmake .. -DCMAKE_PREFIX_PATH=\$(brew --prefix qt@6)"
echo "3. cmake --build ."

