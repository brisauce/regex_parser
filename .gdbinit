set print pretty on
set print array on

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


