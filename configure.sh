#! /usr/bin/sh

if [ -d build ]; then
	rm -rf ./build/
fi

cmake -S . -B ./build

if [ -f .gdbinit ]; then
	cp .gdbinit ./build
else
	echo "WARNING: .gdbinit file not found!"
fi

cd build/

if [ -e compile_commands.json ]; then
	mv compile_commands.json ..
else
	echo "ERROR: compile_commands.json not found in directory:"
	pwd
fi
