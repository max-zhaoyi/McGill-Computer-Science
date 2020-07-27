#Student ID = 260913518
#################################invert Image######################
.data
.text
.globl invert_image 
invert_image:
	# $a0 -> image struct
	#############return###############
	# $v0 -> new inverted image
	############################
	# Add Code
	
	lw $t5, ($a0)	#t5 holds width
	lw $t9, 4($a0)	#t9 holds height
	lw $t8, 8($a0)	#$t8 holds max
	
	li $s3, 0	#$s3 holds our row counter
	li $s2, 0	#t6 holds our column counter
	#li $t5, 0
	move $s6, $a0	#save a0 in s6 for the future
	move $s5, $ra
	
traverse_loop:
	
	blt $s2, $t5, skip_augment_row #if we have finished this column, row++
	li $s2, 0
	addi $s3, $s3 1
	bge $s3, $t9, done_invert
	
skip_augment_row:
	
	move $a0, $s6
	move $a1, $s3
	move $a2, $s2
	jal get_pixel
	move $s7, $v0 #s7 holds pixel value
	
	move $a0, $s6
	
	lw $t5, ($a0)	#t5 holds width RESET
	lw $t9, 4($a0)	#t1 holds height RESET
	lw $t8, 8($a0)	#$t8 holds max RESET
	sub $s7, $t8, $s7 #pixel value=max-pixelvalue
	
	ble $s7, $t8, skip_update_max
	sw $s7, 8($a0) #update max val in struct
	
skip_update_max:

	move $a3, $s7
	move $a0, $s6
	move $a1, $s3
	move $a2, $s2
	jal set_pixel
	
	move $a0, $s6	#being safe and restoring temp registers
	lw $t5, ($a0)	#t5 holds width RESET
	lw $t9, 4($a0)	#t1 holds height RESET
	lw $t8, 8($a0)	#$t8 holds max RESET
	addi $s2, $s2, 1
	j traverse_loop
	
done_invert:

	move $v0,$s6
	move $ra, $s5	
	jr $ra


