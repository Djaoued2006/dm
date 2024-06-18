jmp main

write_in_hex:
    push rcx
    dup 1
    push 16
    div
    push 16
    mul
    sub
    jmp write 

write:
    dup 1
    push 9
    cmp
    jg write_letter
    jmp write_number

write_letter:
    push 55
    add
    push 0
    syscall
    jmp update

write_number:
    push 48
    add
    push 0
    syscall
    jmp update

update:
    push rcx
    push 16
    div
    pop rcx
    push rcx
    push 0
    cmp 
    je exit
    jmp write_in_hex

main:
    push 1001
    pop rcx
    jmp write_in_hex
    

exit:
    halt