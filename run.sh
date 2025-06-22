#!/bin/bash

set -e # Exit if any command fails

# Step 1: Configure CMake if build/ doesn't exist
if [ ! -d build ]; then
	echo "🔧 Configuring project with CMake..."
	cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi

# Step 2: Build the project
echo "🔨 Building project..."
cmake --build build

# Step 3: Symlink compile_commands.json to project root
if [ ! -f compile_commands.json ]; then
	echo "🔗 Linking compile_commands.json..."
	ln -s build/compile_commands.json .
fi

# Step 4: Run the server
echo "🚀 Running server..."
./build/bin/http_server
