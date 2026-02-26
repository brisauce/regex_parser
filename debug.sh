#!/usr/bin/sh
program_name="string_processor"
test_file_name="test_file.txt"

test_word="test"
text_file_contents=$(cat << EOF
testing testing one two three
testing testing you and me
hee hee
EOF
)
test_word_replacement="kalamazoo"


run_gdbgui() {
  gdbgui -g "gdb --q" --args ./"$program_name" "$test_word" "$test_file_name" "$test_word_replacement"
}


cd build

if [ -e $program_name ]; then
  printf "%s\n" "$text_file_contents" > "$test_file_name"
  run_gdbgui
else
  echo "$program_name not found. please run configure.sh then build.sh"
fi
