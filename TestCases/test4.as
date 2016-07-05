	add 1 1 1
	lw 1 2 1
	nand 1 2 3
	add 1 3 4
	lw 0 5 two
	lw 0 6 neg1
back	add 5 6 5
	beq 0 5 done
	beq 0 0 back
done	halt
two	.fill 2
neg1	.fill -1
