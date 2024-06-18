jmp main

new_line:
    push 10
    push 0
    syscall 
    jmp loop

loop:   
    dup 1
    push 26
    cmp
    je done
    dup 2
    dup 2
    add
    push 0
    syscall
    inc
    jmp new_line

done:
    halt

main:
    push 65
    push 0
    jmp loop