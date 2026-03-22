#!/usr/bin/sh
program_name="string_processor"

run_program() {
	./$program_name "$1" --regex-test
}

cd build

if [ ! -f "$program_name"]; then
	echo "$program_name not found. please run build.sh"
fi

run_program "[/?-/*]*"
run_program "[A-Z]?"
