	.intel_syntax noprefix
	.data
# Exit codes
SUCCESS = 0	 			# success return code
# Used constants
NULL = 0				# null byte

	.text
	.global fibonacci
	.type fibonacci, @function

fibonacci:
# ----------------------------
# Parameters
# rdi = int fibonacci step
# rsi = char * output destination
# rdx = size_t length of output buffer

# local used registers
# ----------------------------
	push rbx	 		# save callee-saved registers
	push rbp	 		# save callee-saved registers
	push r12	 		# save callee-saved registers
	push r13	 		# save callee-saved registers
	push r14	 		# save callee-saved registers
	push r15	 		# save callee-saved registers
	cmp rdi, 1	 		# compare against first index value
	jbe .skip_loop			# print output if index <= 1
	push rdx			# save output length
	push rsi			# save output destination

	# r14:r12:r10:r8  = prev - 2
	# r15:r13:r11:r9  = prev - 1
	# rdx:rbp:rbx:rax = curr num
	mov rax, rdi			# mov step into accumulator
	sub rax, 1			# skip first 2 loops
	mov rcx, rax			# set loop counter

	xor r14, r14			# zero out upper bits of num1
	xor r12, r12			# zero out upper middle bits of num1
	xor r10, r10			# zero out lower middle bits of num1
	xor r8, r8			# zero out lower bits of num1

	xor r15, r15			# zero out upper bits of num2
	xor r13, r13			# zero out upper middle bits of num2
	xor r11, r11			# zero out lower middle bits of num2
	mov r9, 1			# set lower bits to 1 of num2
.fib_loop:
	mov rax, r8			# set lower bits of prev number - 2
	mov rbx, r10			# set lower middle bits of prev number - 2
	mov rbp, r12			# set upper middle bits of prev number - 2
	mov rdx, r14			# set upper bits of prev number - 2

	add rax, r9			# add lower bits of prev number - 1 to prev - 2
	adc rbx, r11			# add lower middle bits of prev number - 1 to prev - 2
	adc rbp, r13			# add upper middle bits of prev number - 1 to prev - 2
	adc rdx, r15			# add upper bits of prev number - 1 to prev - 2

	mov r8, r9			# update prev - 2 lower bits
	mov r10, r11			# update prev - 2 lower middle bits
	mov r12, r13			# update prev - 2 upper middle bits
	mov r14, r15			# update prev - 2 upper bits

	mov r9, rax			# update prev - 1 lower bits
	mov r11, rbx			# update prev - 1 lower middle bits
	mov r13, rbp			# update prev - 1 upper middle bits
	mov r15, rdx			# update prev - 1 upper bits
	loop .fib_loop			# jump to loop start

	# set up output
	pop rdi				# snprintf arg1: output string
	pop rsi				# snprintf arg2: output length
	mov rcx, rdx			# snprintf arg4: calculated upper bits
	mov rdx, [rip + .out_str]	# snprintf arg3: load string format
	mov r8, rbp			# snprintf arg5: calculated upper middle bits
	mov r9, rbx			# snprintf arg6: calculated lower middle bits
	push rax			# snprintf arg7: calculated lower bits
	jmp .print_output
.skip_loop:
	# skip loop if first 2 indices
	push rdi			# snprintf arg7: index
	mov rdi, rsi			# snprintf arg1: output string
	mov rsi, rdx			# snprintf arg2: output length
	mov rdx, [rip + .out_str]	# snprintf arg3: load string format
	mov rcx, NULL			# snprintf arg4: NULL
	mov r8, NULL			# snprintf arg5: NULL
	mov r9, NULL			# snprintf arg6: NULL
.print_output:
	xor eax, eax			# no XMM0 registers in use for floats
	call snprintf@plt		# store output
	pop rcx				# balance stack from arg7 push
	mov rax, SUCCESS		# return success
	pop r15				# save callee-saved registers
	pop r14				# save callee-saved registers
	pop r13				# save callee-saved registers
	pop r12				# save callee-saved registers
	pop rbp				# save callee-saved registers
	pop rbx				# save callee-saved registers
	ret

# TODO: should these go in the .data section?
.out_str:
	.quad .str0
.str0:
	.string "%.16llX%.16llX%.16llX%.16llX\n"
