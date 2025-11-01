#! /usr/bin/sh

if [ -d build ]
then
  rm -rf ./build/
fi

cmake -S . -B ./build

cd build/

if [ -e compile_commands.json ]
then
  mv compile_commands.json ..
else
  echo "ERROR: compile_commands.json not found in directory:"
  pwd
fi
