
define peek
set $target_fp = (FILE *)$arg0

if $argc == 0
help peek
else
set $count = 0
set $target = 10
printf "Peeking at next %d bytes",$target 

while $count < $target
  set $ch = (int)fgetc($target_fp)

  #  Implementation dependent equivalent to EOF, may not work
  if $ch == -1
    set $actual = $count + 1
    printf "\n Printed %d of %d bytes\n",$actual,$target
    loop_break
  end

  printf "%c", $ch

  set $count++

end

#  Hopefully equivalent to fseek(fp, -count, SEEK_CUR); 
call (int)fseek($target_fp,-$count,1)
printf "\n"
end

end

document peek
Doobee doobee doo ba doobee doobee doo ba
This is a script which prints 10 characters from the current position of the 
file position indicator, or as many as can be read before encountering an EOF
indicator.
end
