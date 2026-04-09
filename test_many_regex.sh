#!/usr/bin/sh

. ./data.sh
program_name=$STRING_PROCESSOR_PROGRAM_NAME 

test_file_name=$STRING_PROCESSOR_FILE_NAME 

text_file_contents=$STRING_PROCESSOR_TEXT_FILE_CONTENTS 

test_word_replacement=$STRING_PROCESSOR_TEST_WORD_REPLACEMENT
test_word_replacement='friends'

red_text=$(tput setaf 1)
green_text=$(tput setaf 2)
reset_text_color=$(tput sgr0)
proj_dir=$(pwd)

script_name=$(basename "$0")

alias cat="batcat"

reset_file_contents() {
	printf "%s\n" "$text_file_contents" > "$test_file_name"
}

print_next_test() {
  echo "${red_text}===========================================================${reset_text_color}"
  echo "${red_text}=========================NEXT TEST=========================${reset_text_color}"
  echo "${red_text}===========================================================${reset_text_color}"
}

run_program() {
  if [ "$#" -ne 1 ];
  then
    echo "$script_name run_program - This function takes one argument."
    exit 1
  fi


	echo "${green_text}Contents of $test_file_name before query-replace:${reset_text_color}"
  printf "${green_text}Testing file ${reset_text_color}\"$test_file_name\" "
  printf "${green_text}Against regex ${reset_text_color}\"$1\"\n"
	cat "$test_file_name"

	./"$program_name" "$1" "$test_file_name" "$test_word_replacement"

	echo "${green_text}Contents of $test_file_name after query-replace:${reset_text_color}"
	cat "$test_file_name"
  reset_file_contents
}

if [ ! -d build ]; then
  echo "Please run configure.sh and then build.sh before running this script."
  exit 1
fi

cd build

if [ ! -f "$program_name" ]; then
  echo "$program_name not found, please run build.sh before running this script."
  exit 1
fi
reset_file_contents
run_program "colour"
print_next_test
run_program "colou?r"
print_next_test
run_program "col[m-p]ur"
print_next_test
run_program "col*our"
