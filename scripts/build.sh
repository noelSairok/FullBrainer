#!/bin/bash

# Build script for FullBrainer in Rust

# Check if Rust is installed
if ! command -v cargo &> /dev/null; then
    echo "Rust not found. Please install Rust using rustup."
    exit 1
fi

# Build the project
cargo build --release

if [ $? -eq 0 ]; then
    echo ""
    echo "Build successful!"
    echo "Run with: cargo run"
else
    echo "Build failed!"
    exit 1
fi