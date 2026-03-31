#! /usr/bin/sh

program_name="string_processor"

test_file_name="test_file.txt"

test_word="color"

text_file_contents=$(
	cat <<EOF
We say color,
and the brits say color too.
Isn't that interesting?
EOF
)

test_word_replacement="kalamazoo"

run_program() {
	./$program_name "$test_word" "$test_file_name" "$test_word_replacement"
}

if [ ! -d build ]; then
	echo "Build directory not found. Please run configure.sh to generate build files."
	exit 1
fi

cd build

if [ -e $program_name ]; then
	printf "%s\n" "$text_file_contents" >"$test_file_name"

	echo "Contents of $test_file_name before query-replace:"
	cat $test_file_name
	run_program
	echo "Contents of $test_file_name after query-replace:"
	cat $test_file_name
else
	echo "$program_name not found. Please run build.sh."
fi
