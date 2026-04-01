#!/usr/bin/sh
. ./data.sh
program_name=$STRING_PROCESSOR_PROGRAM_NAME 
test_file_name=$STRING_PROCESSOR_FILE_NAME 
test_word=$STRING_PROCESSOR_TEST_WORD

text_file_contents=$STRING_PROCESSOR_TEXT_FILE_CONTENTS 

test_word_replacement=$STRING_PROCESSOR_TEST_WORD_REPLACEMENT

temp_file_name="temp_doc_wow"

#  The browser gdbgui runs on for its gui. Important that this 
#  is the name that you can use from the terminal to start it.
browser_name="google-chrome"

run_gdbgui() {
  local dir="$(whereis -b $browser_name)"
  local browser_dir="${dir#"$browser_name: "}"
	gdbgui -g "gdb --q" --browser "$browser_dir" --args ./"$program_name" "$test_word" "$test_file_name" "$test_word_replacement"
}

if [ ! -d build ]; then
	echo "Build directory not found! Run configure.sh and then build.sh"
  exit 1
fi

if [ -f .gdbinit ]; then
	#  Copy .gdbinit script into the build folder, where gdb will be run
	cp .gdbinit ./build
	cd build
	touch $temp_file_name
	cat <<EOF >>$temp_file_name
# ================WARNING================
#
# ANY CHANGES MADE TO THIS FILE WILL NOT 
#                BE SAVED!
#
# To make changes which WILL be saved, 
# go to the main project directory and 
# make changes to the .gdbinit file 
# there.
EOF
	cat .gdbinit >>$temp_file_name
	rm -f .gdbinit
	mv $temp_file_name .gdbinit
	cd ..
else
	read -n 1 -rp "Warning! .gdbinit not found in project directory.\nContinue? [y/n]" answer
	if [ "$answer" = "y" ] || [ "$answer" = "Y" ]; then
		printf "\nProceeding without user scripts...\n"
	else
		exit 1
	fi
fi

cd build

if [ -e $program_name ]; then
	printf "%s\n" "$text_file_contents" > "$test_file_name"
	run_gdbgui
else
	echo "$program_name not found. please run configure.sh then build.sh"
fi
