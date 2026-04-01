#! /usr/bin/sh

. ./data.sh
program_name=$STRING_PROCESSOR_PROGRAM_NAME 
test_file_name=$STRING_PROCESSOR_FILE_NAME 
test_word=$STRING_PROCESSOR_TEST_WORD

text_file_contents=$STRING_PROCESSOR_TEXT_FILE_CONTENTS 

test_word_replacement=$STRING_PROCESSOR_TEST_WORD_REPLACEMENT

green_text=$(tput setaf 2)
reset_text_color=$(tput sgr0)
run_program() {
	./"$program_name" "$test_word" "$test_file_name" "$test_word_replacement"
}

if [ ! -d build ]; then
	echo "Build directory not found. Please run configure.sh to generate build files."
	exit 1
fi

cd build

if [ -e "$program_name" ]; then
	printf "%s\n" "$text_file_contents" >"$test_file_name"

	echo "${green_text}Contents of $test_file_name before query-replace:${reset_text_color}"
	cat "$test_file_name"
	run_program
	echo "${green_text}Contents of $test_file_name after query-replace:${reset_text_color}"
	cat "$test_file_name"
else
	echo "$program_name not found. Please run build.sh."
fi
