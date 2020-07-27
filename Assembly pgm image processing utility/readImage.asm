#Student ID = 260913518
#########################Read Image#########################
.data
			.align 2
buffer:			.space 1024
			.align 2
bitBuffer:		.space 1
			.align 2
errorMessage:		.asciiz "There was an error opening file"
			.align 2
emptyspace:		.asciiz "\n"
			.align 2
finalprint:		.asciiz "\n The final memory looks like this:\n"
			.align 2
			
.text
		.globl read_image
read_image:
	
	
	
	# $a0 -> input file name
	################# return #####################
	# $v0 -> Image struct :
	# struct image {
	#	int width;
	#       int height;
	#	int max_value;
	#	char contents[width*height];
	#	}
	##############################################
	# Add code here
	
	move $s2, $a0	#save filename in s2
	li $v0, 13 	#open file command		#OPEN
	li $a1, 0 	#read only
	syscall
	
	move $s1, $v0 	#store file descriptor into s1	
	move $a0,$s1 	#a0 now has the file descriptor
	
	li $t0, 0 	#t0 is acting as a counter for whitespaces; initialize to 0
hasError:
	bge 	$v0, 0, read_bit
	li 	$v0, 4
	la 	$a0, errorMessage
	syscall
	jr	$ra

	
	
	move $a0,$s1 	#a0 now has the file descriptor
read_bit:
	
	li $v0, 14	#read command
	la $a1, bitBuffer	#read to bitBuffer
	li $a2, 1
	syscall
	
	lbu $t1, bitBuffer	#t1 is our designated bitbuffer holder
	
	#check if it's a whitespace
	#t2 holds our comparison in ASCII, 9 is tab, 10 is NL, 32 is space and 13 is CR
	li $t2, 9	#tab
	beq $t1, $t2, nextWhite
	li $t2 ,10 	#newline
	beq $t1, $t2, nextWhite
	li $t2, 13	#carriage return
	beq $t1, $t2, nextWhite
	li $t2, 32	#space
	beq $t1, $t2, nextWhite
	li $t2, 48
	blt $t1, $t2, nextWhite
	#bit has passed the vibe check
notWhite: 		#now let's determine which value we are dealing with, depending on the white value we are at
			#this part should also handle concatenating
	li $t2, 0 	#we're dealing with P2 or P5
	beq $t0, $t2, header1
	li $t2, 1 	
	beq $t0, $t2, header2
	li $t2, 2 	
	beq $t0, $t2, header3
	li $t2, 3 	
	beq $t0, $t2, header4
	
	li $t2, 4	#did we reach past 4th header element?
	bge $t0, $t2, parseArray #if we have, then we are done parsing the header, time to move on to the array
	
header1: #P2 or P5 (so we just have to skip the P and parse 2 or 5
	#if the current bit is not white anymore, we can increment the whitespace counter
	addi $t0, $t0, 1
	
#		#put $t1 into memory
#		#then load next byte
		li $t2, 79
		ble $t1, $t2, skipread
	loop1:
		li $v0, 14	#read command HEADER 1
		la $a1, bitBuffer	#read to bitBuffer
		li $a2, 1
		syscall
		lbu $t1, bitBuffer	#t1 is our designated bitbuffer holder
	skipread:
		
		li $t2, 50
		beq $t1, $t2, equals2
		li $t2, 53
		beq $t1, $t2, equals5
		beq $t2, 7, wrongequals
		equals2:
			li $t8, 0		#0 in $t8 means P2
			j wrongequals
		equals5:
			li $t8, 1		#1 in $t8 means p5
			
			
		wrongequals:
		
		li $v0, 14	#read command HEADER 1
		la $a1, bitBuffer	#read to bitBuffer
		li $a2, 1
		syscall
		lbu $t1, bitBuffer	#t1 is our designated bitbuffer holder
		
		#check if we're still dealing with header or with white
		li $t2, 9	#tab
		beq $t1, $t2, nextWhite
		li $t2 ,10 	#newline
		beq $t1, $t2, nextWhite
		li $t2, 13	#carriage return
		beq $t1, $t2, nextWhite
		li $t2, 32	#space
		beq $t1, $t2, nextWhite
		li $t2, 48
		blt $t1, $t2, nextWhite

		j loop1	#HEADER 1

			
header2:	#width
	addi $t0, $t0, 1
	li $t3, 0
	loop2:
#		#put $t1 into memory
		li $t2, 1
		bne $t3, $t2, firstTime2
		
		#else multiply by 10
		li $t2, 10		#HEADER 2
		mult $s3, $t2
		mflo $s3
	firstTime2:
		li $t3, 1
		addi $t1, $t1, -48	#convert to int
		add $s3, $s3, $t1
#		#then load next byte
		li $v0, 14	#read command
		la $a1, bitBuffer	#read to bitBuffer
		li $a2, 1
		syscall

		lbu $t1, bitBuffer	#t1 is our designated bitbuffer holder
		
		
		#check if we're still dealing with header or with white
		
		#printtest
		#li $v0, 1
		#move $a0, $t1
		
		#li $t2, 9	#tab
		#beq $t1, $t2, nextWhite
		#li $t2 ,10 	#newline
		#beq $t1, $t2, nextWhite
		#li $t2, 13	#carriage return
		#beq $t1, $t2, nextWhite
		#li $t2, 32	#space
		#beq $t1, $t2, nextWhite
		li $t2, 48
		blt $t1, $t2, nextWhite
		
#		#if not white, then we can concatenate it 
#		## convert to int
#		##*10
		j loop2			#HEADER 2

header3: #height
	addi $t0, $t0, 1
	li $t3, 0
	loop3:
		li $t2, 1
		bne $t3, $t2, firstTime3
		
		#else multiply by 10
		li $t2, 10		#HEADER 3
		mult $s4, $t2
		mflo $s4
	firstTime3:
		li $t3, 1
		addi $t1, $t1, -48	#convert to int
		add $s4, $s4, $t1
#		#put $t1 into memory
#		#then load next byte
		li $v0, 14	#read command
		la $a1, bitBuffer	#read to bitBuffer
		li $a2, 1				#HEADER 3
		syscall
	
		lbu $t1, bitBuffer	#t1 is our designated bitbuffer holder
		#check if we're still dealing with header or with white
		#li $t2, 9	#tab
		#beq $t1, $t2, nextWhite
		#li $t2 ,10 	#newline
		#beq $t1, $t2, nextWhite
		#li $t2, 13	#carriage return
		#beq $t1, $t2, nextWhite
		#li $t2, 32	#space
		#beq $t1, $t2, nextWhite
		li $t2, 48
		blt $t1, $t2, nextWhite
		
#		#if not white, then we can concatenate it 
#		## convert to int
#		##*10
		j loop3			#HEADER 3
		
header4: #max gray val
	addi $t0, $t0, 1
	li $t3, 0
	loop4:
	li $t2, 1
		bne $t3, $t2, firstTime4		#HEADER 4
		
		#else multiply by 10
		li $t2, 10
		mult $s5, $t2
		mflo $s5
	firstTime4:
		li $t3, 1
		addi $t1, $t1, -48	#convert to int	#HEADER 4
		add $s5, $s5, $t1
#		#put $t1 into memory
#		#then load next byte
		li $v0, 14	#read command
		la $a1, bitBuffer	#read to bitBuffer
		li $a2, 1
		syscall
	
		lbu $t1, bitBuffer	#t1 is our designated bitbuffer holder
		#check if we're still dealing with header or with white
		#li $t2, 9	#tab
		#beq $t1, $t2, nextWhite
		#li $t2 ,10 	#newline
		#beq $t1, $t2, nextWhite
		#li $t2, 13	#carriage return
		#beq $t1, $t2, nextWhite
		#li $t2, 32	#space
		#beq $t1, $t2, nextWhite
		li $t2, 48
		blt $t1, $t2, nextWhite
		
#		#if not white, then we can concatenate it 
#		## convert to int
#		##*10
		j loop4			#HEADER 4

	
#notWhiteLoop:
#	li $v0, 14	#read command
#	la $a1, bitBuffer	#read to bitBuffer
#	li $a2, 1
#	syscall
	
#	lbu $t1, bitBuffer	#t1 is our designated bitbuffer holder

#	li	$t2, 1
#	beq	$t0, $t7, width
#	li	$t2, 2
#	beq	$t0, $t7, height
#	li	$t2, 3
#	beq	$t0, $t7, grayVal
	
#	j	read_bit

		
nextWhite:
	li $v0, 14	#read command
	la $a1, bitBuffer	#read to bitBuffer
	li $a2, 1
	syscall
	
	lbu $t1, bitBuffer	#t1 is our designated bitbuffer holder

	li $t2, 9	#tab
	beq $t1, $t2, nextWhite
	li $t2 ,10 	#newline
	beq $t1, $t2, nextWhite
	li $t2, 13	#carriage return
	beq $t1, $t2, nextWhite
	li $t2, 32	#space
	beq $t1, $t2, nextWhite
	#li $t2, 48
	#blt $t1, $t2, nextWhite
	
	#PRINT TEST
	#li $v0, 1
	#move $a0, $t0
	#syscall
	#move $a0,$s1 
	#we are dealing with a non-white bit rn
	j notWhite
	
parseArray:	#start with memory allocation, REMEMBER we are fed with first whitespace 
	mult $s3, $s4		
	mflo $t3
	addi $t3, $t3, 12	
	#add $t3, $t3, $t3 	
	#add $t3, $t3, $t3 	#(3 words or 12 bytes (width, height,max) + width*hight bytes)
	#move $s1, $a0		#####
	move $a0, $t3
	li $v0, 9		#sbrk (allocate heap memory)
	syscall 			#mALLOC
	move $a0,$s1 
	move $s6, $v0		#s6 holds our address for the array
	#move $a0, $s1
	
	sw $s3, 0($s6)		#load first 12 bytes of the array with w,h,max
	sw $s4, 4($s6) 	
	sw $s5, 8($s6) 	
	
	
	li $t4, 12		#$t4 holds the immediate 12
	
	#offset value in t4, pointer in t5, t3 holds size of mem
	add $t5, $t4, $s6 	#pointer in t5, which is s6 moved by 12
	li $t2, 1
	
	beq $t8, $t2, p5loop
	
p2loop:	
	bge $t4, $t3, end_readImage	
	
	
	li $t2, 5
	bgt $t2, $t1, end_readImage
	li $t2, 48
	bgt $t2, $t1, readArrayNext
	
	addi $t1, $t1, -48
	
	move $t6, $t1 #t6 holds our number

#read next and determine if we have to concatenate		
	li $v0, 14	#read command
	la $a1, bitBuffer	#read to bitBuffer
	li $a2, 1
	syscall
	lbu $t1, bitBuffer
	
		
	bge $t4, $t3, end_readImage
				
	li $t2, 5
	bgt $t2, $t1, end_readImage
	li $t2, 48
	bgt $t2, $t1, storeArrayNext
	
p2concatenate:		
	#passes vibe check
	addi $t1, $t1, -48

	li $t2, 10
	mult $t6, $t2
	mflo $t6
	add $t6, $t6, $t1
	#concatenated
	
	#read next and determine if we have to concatenate		
	li $v0, 14	#read command
	la $a1, bitBuffer	#read to bitBuffer
	li $a2, 1
	syscall
	lbu $t1, bitBuffer	
		
	bge $t4, $t3, end_readImage			
	li $t2, 5
	bgt $t2, $t1, end_readImage
	li $t2, 48
	bgt $t2, $t1, storeArrayNext
	j p2concatenate
	
storeArrayNext:
	sb $t6, ($t5)
	
	addi $t5,$t5,1
	addi $t4,$t4,1
	
	
	#PRINT TEST
	#li $v0, 1
	#move $a0, $t6
	#syscall
	#li $v0, 4
	#la $a0, emptyspace
	#syscall
	move $a0,$s1 
readArrayNext:	
	li $v0, 14	#read command
	la $a1, bitBuffer	#read to bitBuffer
	li $a2, 1
	syscall
	lbu $t1, bitBuffer
	
	j p2loop
	
		
isP5:	
	li $v0, 14	#read command
	la $a1, bitBuffer	#read to bitBuffer
	li $a2, 1
	syscall
	lbu $t1, bitBuffer
p5loop:
	bge $t4, $t3, end_readImage	
	#addi $t1, $t1, -48
	
	sb $t1, ($t5)
	
	addi $t4,$t4,1
	addi $t5,$t5,1
	
	#PRINT TEST
	#li $v0, 1
	#move $a0, $t1
	#syscall
	#li $v0, 4
	#la $a0, emptyspace
	#syscall
	move $a0,$s1 
	
	
	j isP5

end_readImage:
	
	
	#li $v0, 4
	#la $a0, finalprint
	#syscall
	#move $a0,$s1 

	#li $v0, 4
	#la $a0, 0($s6)
	#syscall
	#move $a0,$s1 
	
	li $v0, 16
	move $a0, $s2
	syscall
		
	move $v0, $s6
	jr $ra
	
