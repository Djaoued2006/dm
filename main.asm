jmp main

main:
    push 2
    push 2
    cmp
    jle break
    push 1
    halt

break:
    push 0
    halt