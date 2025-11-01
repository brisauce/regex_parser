#!/usr/bin/sh
PROGRAM_NAME="string_processor"


run_gdbgui() {
  gdbgui -g "gdb --q" --args ./$PROGRAM_NAME
}


cd build

if [ -e $PROGRAM_NAME ]; then
  run_gdbgui
else
  echo "$PROGRAM_NAME not found. please run configure.sh then build.sh"fi