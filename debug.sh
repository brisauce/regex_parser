#!/usr/bin/sh
program_name="string_processor"
test_file_name="test_file.txt"

test_word="he*"
text_file_contents=$(
	cat <<EOF
testing testing one two three
testing testing you and me
hee hee
EOF
)

test_word_replacement="kalamazoo"

temp_file_name="temp_doc_wow"

run_gdbgui() {
	gdbgui -g "gdb --q" --args ./"$program_name" "$test_word" "$test_file_name" "$test_word_replacement"
}

if [ ! -d build ]; then
	echo "Build directory not found! Run configure.sh and then build.sh"
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
	if [ $answer == "y" ] || [ $answer == "Y"]; then
		printf "\nProceeding without user scripts...\n"
	else
		exit 1
	fi
fi

cd build

if [ -e $program_name ]; then
	printf "%s\n" "$text_file_contents" >"$test_file_name"
	run_gdbgui
else
	echo "$program_name not found. please run configure.sh then build.sh"
fi
