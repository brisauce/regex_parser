#! /usr/bin/sh

PROGRAM_NAME="string_processor"


run_program () {
  ./$PROGRAM_NAME "Run, run, as fast as you can! You can't catch me, I'm the gingerbread man!" f[a-d]st 
}

if [ ! -d build ]; then
  echo "Build directory not found. Please run configure.sh to generate build files."
  exit 1
fi

cd build

if [ -e $PROGRAM_NAME ]; then
  run_program
else
  echo "$PROGRAM_NAME not found. Please run build.sh."
fi
