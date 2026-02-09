#!/bin/bash

# Build script for FullBrainer with SFML visualization

# Check if SFML is installed
if ! pkg-config --exists sfml-all 2>/dev/null; then
    echo "SFML not found. Installing SFML..."
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y libsfml-dev sqlite3 libsqlite3-dev cmake
    elif command -v pacman &> /dev/null; then
        sudo pacman -Sy sfml sqlite cmake
    else
        echo "Please install SFML manually for your system"
        exit 1
    fi
fi

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir -p build
fi

cd build

# Run cmake and build
cmake ..
cmake --build . --config Release

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful!"
    echo "Run with: ./fullBrainer"
else
    echo "Build failed!"
    exit 1
fi
