.equ BUF_SIZE, 64

.data

buf: .fill BUF_SIZE, 1, 0
delm: .asciz " \r\n"
msg_buffer_overflow: .asciz "Buffer overflow\n"
msg_zero_args: .asciz "Invalid number of args: two numbers expected, but none received\n"
msg_one_arg: .asciz "Invalid number of args: two numbers expected, but only one received\n"
msg_many_args: .asciz "Invalid number of args: two numbers expected, but more received\n"
msg_lhs_invalid: .asciz "Invalid args: unexpected symbols in the first argument\n"
msg_rhs_invalid: .asciz "Invalid args: unexpected symbols in the second argument\n"
msg_addition_overflow: .asciz "Addition overflow\n"
msg_fmt: .asciz "%d\n"

.text
.global main


main:
        # Read from stdin into the buffer
        la a0, buf
        li a1, BUF_SIZE
        call gets

        # Check for buffer overflow by checking the symbol before the last
        la t0, buf
        lb t0, [BUF_SIZE - 2](t0)
        bne t0, zero, failure_buffer_overflow

        # Find the first token and store it in s1
        la a0, buf
        la a1, delm
        call strtok
        mv s1, a0
        beq s1, zero, failure_zero_args

        # Find the second token and store it in s2
        li a0, 0
        la a1, delm
        call strtok
        mv s2, a0
        beq s2, zero, failure_one_arg

        # Check that there are no more tokens
        li a0, 0
        la a1, delm
        call strtok
        bne a0, zero, failure_many_args

        # Check that the first token contains only digits
        mv a0, s1
        jal ra, all_digits
        beq a0, zero, failure_lhs_invalid

        # Check that the second token contains only digits
        mv a0, s2
        jal ra, all_digits
        beq a0, zero, failure_rhs_invalid

        # Parse the first token
        # s1 now contains its value
        mv a0, s1
        call atoi
        mv s1, a0

        # Parse the second token
        # s2 now contains its value
        mv a0, s2
        call atoi
        mv s2, a0

        # Add both operands and check for overflow
        # s1 now contains their sum
        add s1, s1, s2
        blt s1, zero, failure_addition_overflow

        # Print the sum
        la a0, msg_fmt
        mv a1, s1
        call printf

        # Exit gracefully
        li a7, 2                  # Exit system call
        li a0, 0                  # Exit status (success)
        ecall

failure_buffer_overflow:
        la a0, msg_buffer_overflow
        j failure

failure_zero_args:
        la a0, msg_zero_args
        j failure

failure_one_arg:
        la a0, msg_one_arg
        j failure

failure_many_args:
        la a0, msg_many_args
        j failure

failure_lhs_invalid:
        la a0, msg_lhs_invalid
        j failure

failure_rhs_invalid:
        la a0, msg_rhs_invalid
        j failure

failure_addition_overflow:
        la a0, msg_addition_overflow
        j failure

# failure message is in a0
failure:
        call printf
        li a7, 2                  # Exit system call
        li a0, 1                  # Exit status (failure)
        ecall


# Checks that a string consists of digits only
# This subroutine follows the calling convention
all_digits:
        mv t0, a0

all_digits_loop:
        lb t1, 0(t0)
        beq t1, zero, all_digits_success

        li t2, '0'
        blt t1, t2, all_digits_fail
        li t2, '9'
        bgt t1, t2, all_digits_fail

        addi t0, t0, 1
        j all_digits_loop

all_digits_success:
        li a0, 1
        ret

all_digits_fail:
        li a0, 0
        ret
