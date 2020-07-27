# Student ID = 1234567
####################################write Image#####################
.data
write_white:	.asciiz " "
		.align 2
write_nl:	.asciiz "\n"
		.align 2
P5Header2:	.asciiz "P5\n"
		.align 2
P2Header2:	.asciiz "P2\n"
		.align 2
.text
.globl write_image
write_image:
	# $a0 -> image struct
	# $a1 -> output filename
	# $a2 -> type (0 -> P5, 1->P2)
	################# returns #################
	# void
	# Add code here.
	li $s3, 0
	move $s0, $a0
	move $a0, $a1
	move $s2, $a1
	li $v0, 13 	#open file command		
	li $a1, 1 	#write mode
	syscall
	move $s1, $v0 	#store input file descriptor into s1
	
	li $v0, 9
	li $a0, 5
	syscall
	move $t9, $v0	#our custom 5 byte buffer in t9
	
	beq $a2, 0, isP5
	
isP2:
	li $v0, 15       	#System call for write file 
	move $a0, $s1    	#file descriptor in a0
	la $a1, P2Header2 	#address of file in a1
	la $a2, 3       	#length in a2
	syscall
	#now that we're done with the header, we have to determine how big width height and max val are
	lw $t1, ($s0) #width t1 is number of columns 
	j digitGate
isP5:
	
	#lw $t2, 4($a0)#height t2 is number of rows
	#lw $t3, 8($a0)#max in t3	
	li $v0, 15       	#System call for write file 
	move $a0, $s1    	#file descriptor in a0
	la $a1, P5Header2 	#address of file in a1
	la $a2, 3       	#length in a2
	syscall
	#now that we're done with the header, we have to determine how big width height and max val are
	lw $t1, ($s0) #width t1 is number of columns 
	
digitGate:
	blt $t1, 10, oneDigit
	blt $t1, 100, twoDigit
	blt $t1, 1000, threeDigit
	#blt $t1, 10000, fourDigit
	#$t0
oneDigit:
	addi $s3, $s3, 1
	
	li $v0, 9
	li $a0, 5
	syscall
	move $t9, $v0	#our custom 5 byte buffer in t9
	
	
	addi $t1, $t1, 48
	sb $t1, ($t9)
	
	li $v0, 15       	#System call for write file 
	move $a0, $s1    	#file descriptor in a0
	la $a1, ($t9) 	#address of file in a1
	li $a2, 1       	#length in a2
	syscall
	li $v0, 15       	#System call for write file 
	move $a0, $s1    	#file descriptor in a0
	la $a1, write_white 	#address of file in a1
	li $a2, 1       	#length in a2
	syscall
	j postDigitParse
twoDigit:
	addi $s3, $s3, 1
	li $v0, 9
	li $a0, 5
	syscall
	move $t9, $v0	#our custom 5 byte buffer in t9
	
	div $t2, $t1, 10
	mul $t2, $t2, 10
	sub $t1, $t1, $t2
	div $t2, $t2, 10 #t2 is left digit, $t1 is right digit
	
	addi $t3, $t2, 48
	sb $t3, ($t9)
	addi $t3, $t1, 48
	sb $t3, 1($t9)
	addi $t3, $0, 32 #space
	sb $t3, 2($t9)
	li $v0, 15       	#System call for write file 
	move $a0, $s1    	#file descriptor in a0
	la $a1, ($t9) 	#address to be printed in $t9
	li $a2, 3       	#length in a2
	syscall
	
	j postDigitParse
threeDigit:
	addi $s3, $s3, 1
	li $v0, 9
	li $a0, 5
	syscall
	move $t9, $v0	#our custom 5 byte buffer in t9
	
	#leftmost digit: t1/100, center: t1/100-left*10, right: t1(mod10)
	div $t3, $t1,100
	addi $t3,$t3,48
	
	sb $t3, ($t9)
	div $t3, $t1,100
	
	mul $t3,$t3,10
	div $t2, $t1, 10
	sub $t4, $t2,$t3
	
	addi $t4,$t4,48
	sb $t4, 1($t9)
	
	div $t3, $t1, 10
	mul $t3, $t3, 10
	
	
	sub $t3, $t1, $t3
	
	#PRINT TEST
	#li $v0, 1
	#move $a0, $t3
	#syscall
	
	addi $t3, $t3, 48
	sb $t3, 2($t9)
	
	
	addi $t3, $0, 32 #space
	sb $t3, 3($t9)
	li $v0, 15       	#System call for write file 
	move $a0, $s1    	#file descriptor in a0
	la $a1, ($t9) 	#address of file in a1
	li $a2, 4       	#length in a2
	syscall
	j postDigitParse

postDigitParse:
	beq $s3, 1, hParse
	beq $s3, 2, mParse
	
	
	##TODO
	
	
	
	j end_writeimage
hParse:
	lw $t1, 4($s0) #width t1 is number of row
	j digitGate

mParse:
	lw $t1, 8($s0)
	j digitGate
	
	

end_writeimage:	
	move $a1, $s2	#load file name in a0
	li $v0, 16 	#close file
	syscall
	jr $ra
