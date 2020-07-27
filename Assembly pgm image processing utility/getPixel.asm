#Student ID = 260913518
##########################get pixel #######################
.data
oob:		.asciiz "out of bounds error!"
		.align 2
nlgetpixel:	.asciiz "\n"
		.align 2
.text
.globl get_pixel

get_pixel:
	
	
	# $a0 -> image struct
	# $a1 -> row number
	# $a2 -> column number
	################return##################
	# $v0 -> value of image at (row,column)
	#######################################
	# Add Code
	
	lw $t2, 0($a0) #width t2 is number of columns
	bge $a2, $t2, get_pixel_oob #check if it's out of bounds
	
	lw $t1, 4($a0)#height t1 is number of rows	
	bge $a1, $t1, get_pixel_oob #check if it's out of bounds
	
	
	mult $t2, $a1 	#mult width times row number (i*width)+j formula
	mflo $s1
	add $s1, $s1, $a2 #(i*width)+j formula
	#addi $a0, $a0, 12	#offset in struct for the w, h, max variables
	add $s1, $s1, $a0 #point at the correct byte in the struct (start of array+(i*width)+j
	
	lbu $v0, 12($s1)	#load value in our return register

	jr $ra
	
get_pixel_oob:

	li $v0, 4
	la $a0, oob
	syscall
	la $a0, nlgetpixel
	syscall
	#quit to debug
	li $v0, 10
	syscall
	
	add $v0, $v0, $0
	

	jr $ra
