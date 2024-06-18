	jmp

write_in_hex:
	push
	dup
	push
	div
	push
	mul
	sub
	jmp

write:
	dup
	push
	cmp
	jg
	jmp

write_letter:
	push
	add
	push
	syscall
	jmp

write_number:
	push
	add
	push
	syscall
	jmp

update:
	push
	push
	div
	pop
	push
	push
	cmp
	je
	jmp

main:
	push
	pop
	jmp

exit:
	halt
