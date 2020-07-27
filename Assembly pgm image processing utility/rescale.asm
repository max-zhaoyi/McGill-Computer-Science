# Student ID = 260913518
###############################rescale image######################
.data
.text
.globl rescale_image
rescale_image:
	# $a0 -> image struct
	############return###########
	# $v0 -> rescaled image
	######################
	# Add Code
	
	
	lw $t0, ($a0)	#t0 holds width (nb of cols)
	lw $t1, 4($a0)	#t1 holds height (nb of rows)
	lw $t2, 8($a0)	#$t2 holds max
	li $s6, 0	#col counter
	li $s2, 0	#row counter
	li $s3, 255	#min val
	
	move $s0, $a0 #store pointer to struct in s0
	move $s5, $ra
	
min_update_loop:	
	blt $s6, $t0, skip_augment_row_rescale_min #If col counter is smaller than width keep counting
	li $s6, 0	#else reset col counter and simply augment row counter
	addi $s2, $s2, 1
	bge $s2, $t1, done_min	#when row equals height we're done
	
skip_augment_row_rescale_min:	
	move $a1, $s2 #row in a1, col in a2 for get pixel
	move $a2, $s6
	jal get_pixel
	#pixel value at v0
	move $a0, $s0 #store pointer to struct in a0
	lw $t0, ($a0)	#t0 holds width (nb of cols)
	lw $t1, 4($a0)	#t1 holds height (nb of rows)
	lw $t2, 8($a0)	#$t2 holds max
	
	blt $s3, $v0, skip_update_min
	move $s3, $v0 #UPDATING MINIMUM
	
	
skip_update_min:
	addi $s6, $s6, 1
	j min_update_loop


done_min: 
	move $a0, $s0 #store pointer to struct in s0
	lw $t0, ($a0)	#t0 holds width (nb of cols)
	lw $t1, 4($a0)	#t1 holds height (nb of rows)
	lw $t2, 8($a0)	#$t2 holds max
#PRINT TEST
	#li $v0, 1
	#li $a0, 4556
	#move $a0, $s3
	#syscall
	#move $a0, $s0 #being EXTRA safe (debugging mips is ://)
	#j done_rescale

#we now procede to rescale the image
	#is min=max? if so we just return the same image
	beq $s3, $t2, done_rescale
	sub $s4, $t2,$s3 #s4=max-min
	#reset counters
	li $s6, 0	#col counter
	li $s2, 0	#row counter
	
	
	
	#print test
	#li $v0, 1
	#move $a0, $s3
	#syscall
	#move $a0, $s0 #being EXTRA safe (debugging mips is ://)
	
	
	
rescale_loop:	
	blt $s6, $t0, skip_augment_row_rescale
	li $s6, 0
	addi $s2, $s2, 1
	bge $s2, $t1, done_rescale
	
skip_augment_row_rescale:	
	move $a0, $s0 #store pointer to struct in a0
	move $a1, $s2
	move $a2, $s6
	jal get_pixel
	
	move $t5, $v0
	
	#print test
	#li $v0, 1
	#move $a0, $t5
	#syscall
	#move $a0, $s0 #being EXTRA safe (debugging mips is ://)
	
	
	move $a0, $s0 #store pointer to struct in a0, being EXTRA safe you know
	lw $t0, ($a0)	#t0 holds width (nb of cols)
	lw $t1, 4($a0)	#t1 holds height (nb of rows)
	lw $t2, 8($a0)	#$t2 holds max
	#formula: $t5=($v0-$s3)255/$s4
	beq $t5, 255, pixel_zero
	
	sub $t5, $t5, $s3
	
	beq $t5, 0, pixel_zero
	
	
	mul $t5, $t5, 255
	mflo $t5
	#FLOAT DIVISION
	#ROUND TO NEAREST INTEGER
	#STORE FINAL ANSWER IN $t5
	#
	#
	mtc1 $t5, $f1
	mtc1 $s4, $f3 #using odd registers to respect the single precision conventions
	
	cvt.s.w $f1, $f1
	cvt.s.w $f3, $f3
	
	div.s $f0, $f1, $f3
	round.w.s $f1, $f0
	
	mfc1 $t5, $f1
pixel_zero:
	move $a0, $s0 #store pointer to struct in a0
	move $a3, $t5
	move $a1, $s2
	move $a2, $s6
	
	jal set_pixel
	#PRINT TEST
	#li $v0, 1
	#move $a0, $t5
	#syscall
	move $a0, $s0 #being EXTRA safe (debugging mips is ://)
	lw $t0, ($a0)	#t0 holds width (nb of cols)
	lw $t1, 4($a0)	#t1 holds height (nb of rows)
	lw $t2, 8($a0)	#$t2 holds max
	addi $s6, $s6, 1
	j rescale_loop
	
done_rescale:
	move $a0, $s0
	addi $t4, $a0, 8 #address of max in struct
	addi $t5, $0, 255
	#sb $t5, ($t4) #set max val to 255
	
	move $ra, $s5
	jr $ra
