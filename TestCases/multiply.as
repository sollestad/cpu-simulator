	lw 0 2 one
	lw 0 3 neg1
	lw 0 4 num1
	lw 0 5 num2
	add 0 0 1
start	nand 2 4 6
	nand 3 6 6
	beq 0 6 skip
	add 1 5 1
skip	add 5 5 5
	add 2 2 2
	beq 0 2 done
	beq 0 0 start
done	halt
one .fill 1
neg1 .fill -1
num1 .fill 10
num2 .fill 16
