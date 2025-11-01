#! /usr/bin/sh
if [ -d build ]
then
  cd build/
  make
else
  echo "Build folder not found. Please run configure.sh to generate build folder and necessary contents."
fi
