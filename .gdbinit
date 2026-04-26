set print pretty on
set print array on

define hook-run
  shell ../reset_file_contents.sh
end

define peek
  if $argc == 0
    help peek
  else
    set $target_fp = (FILE *)$arg0
    set $count = 0
    set $target = 10
    printf "Peeking at next %d bytes:\n",$target

    while $count < $target
      set $ch = (int)fgetc($target_fp)
      if $ch == -1
        set $actual = $count + 1
        printf "\nPrinted %d of %d bytes\n",$actual,$target
        loop_break
      end
      printf "%c",$ch
      set $count++
    end

# Parentheses and lack of spaces help GDB parse math
    call (int)fseek($target_fp, (0 - $count), 1)
    printf "\n"
  end
end

document peek
  syntax:
  peek file_pointer


  This is a script made by google gemini.

  Attempts to print 10 one-byte characters from where the current file position indicator is,
  then returns to where the script was called.
end

define printFile
  if $argc == 0 
    help peek 
  end

  if $argc > 1
    help peek 
  end 

  set $target_fp = (FILE *)$arg0
  set $return_here = (long)ftell($target_fp)

  #  Move to the beginning of the file
  call fseek($target_fp, 0, 0)

  #  Print all chars to stdout
  while 1
    set $ch = (int)fgetc($target_fp)

    if ($ch == -1)
      loop_break
    end

    printf "%c",$ch
  end 
  #  Return the position indicator to its original place
  call fseek($target_fp, $return_here, 0)
end

document printFile
  syntax:
  printFile file_pointer

  Prints the contents of the file pointed to by file_pointer from the beginning.
  Sets the position indicator back to what it was before the function was called
  after printing out the file.
end
