#Student ID = 260913518
##########################set pixel #######################
.data
oob:		.asciiz "out of bounds error!"
		.align 2
nlsetpixel:	.asciiz "\n"
		.align 2
.text
.globl set_pixel

set_pixel:
	
	
	# $a0 -> image struct
	# $a1 -> row number
	# $a2 -> column number
	# $a3 -> new value (clipped at 255)
	###############return################
	#void
	# Add code here
	lw $t1, ($a0) #width t1 is number of columns
	lw $t2, 4($a0)#height t2 is number of rows
	lw $t3, 8($a0)#max in t3
	#check if we have to clip max_val
	ble $a3, 255, skip_normalize #do we have to clip the new value?
	
	addi $a3, $0, 255	#if we do then the new val is 255
	
skip_normalize:
	#check if we have to update new max
	ble $a3, $t3, skip_set_new_max
	
	addi $t4, $a0, 8 #address of max in struct
	sb $a3, ($t4)
	
skip_set_new_max: #now we set the pixel with the value in $a3

	bge $a2, $t1, set_pixel_oob #is it out of bounds (oob)?
	bge $a1, $t2 set_pixel_oob
	mult $a1, $t1 #set pointer to correct place in struct, to replace the pixel value by whatever is in a3
	mflo $s1
	addi $s1, $s1, 12
	add $s1, $s1, $a0
	add $s1, $s1, $a2
	
	sb $a3, ($s1)
	
	jr $ra
	
set_pixel_oob:
	li $v0, 4
	la $a0, oob #"throw out of bounds warning"
	syscall
	la $a0, nlsetpixel
	syscall
	#quit to debug
	li $v0, 10
	syscall
	
	
	jr $ra
