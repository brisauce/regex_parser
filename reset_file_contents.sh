#!/usr/bin/sh

if [ ! -f data.sh ]; then 
  cd ..
fi
. ./data.sh
program_name=$STRING_PROCESSOR_PROGRAM_NAME 

test_file_name=$STRING_PROCESSOR_FILE_NAME 

text_file_contents=$STRING_PROCESSOR_TEXT_FILE_CONTENTS 

test_word_replacement=$STRING_PROCESSOR_TEST_WORD_REPLACEMENT


if [ ! -d ./build ]; then 
  echo "$0: cannot find build directory from $(pwd)"
  exit 1
fi

cd build

if [ ! -f $test_file_name ]; then
  echo "$0: unable to locate $test_file_name in $(pwd)"
  exit 1
fi

printf "%s\n" "$text_file_contents" > "$test_file_name"

