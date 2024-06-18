	jmp main

print_in_binary:
	dup 1
	push 0
	cmp
	je print_zero
	jmp print_number

print_number:
	dup 1
	push 0
	cmp
	je exit
	dup 1
	dup 1
	push 2
	div
	push 2
	mul
	sub
	push 48
	add
	push 0
	syscall
	push 2
	div
	jmp print_number

print_zero:
	push 0
	push 48
	add
	push 0
	syscall
	jmp exit

main:
	push 65535
	jmp print_in_binary

exit:
	halt
