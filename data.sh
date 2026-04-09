#!/usr/bin/sh 

export STRING_PROCESSOR_PROGRAM_NAME="string_processor"
export STRING_PROCESSOR_FILE_NAME="test_file.txt" 
export STRING_PROCESSOR_TEST_WORD="col[m-p]ur"
STRING_PROCESSOR_TEXT_FILE_CONTENTS="$(
	cat <<'EOF'
colour colour colour colour
EOF
)"
export STRING_PROCESSOR_TEXT_FILE_CONTENTS
export STRING_PROCESSOR_TEST_WORD_REPLACEMENT="kalamazoo"

