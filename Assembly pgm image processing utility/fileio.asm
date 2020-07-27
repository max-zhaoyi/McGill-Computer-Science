#Student ID = 260913518
############################ Q1: file-io########################
.data
			.align 2
inputTest1:		.asciiz "test1.txt"
			.align 2
inputTest2:		.asciiz "test2.txt"
			.align 2
outputFile:		.asciiz "copy.pgm"
			.align 2
buffer:			.space 1024
errorMessage:		.asciiz "There was an error opening file"
			.align 2
P2Header:		.asciiz "P2\n24 7\n15\n"
			.align 2
						
.text
.globl fileio

fileio:
	
	la $a0,inputTest1
	#la $a0,inputTest2
	jal read_file
	
	la $a0,outputFile
	jal write_file
	
	li $v0,10		# exit...
	syscall	
		

	
read_file:	
	move $s0, $a0	#save filename in s2
	li $v0, 13 	#open file command		#OPEN
	li $a1, 0 	#read only
	syscall
	
	move $s1, $v0 	#store file descriptor into s1	
	move $a0,$s1 	#a0 now has the file descriptor
	
hasError:
	bge $v0, 0, read
	li $v0, 4
	la $a0, errorMessage
	syscall
	jr $ra
	
read:	
	move $a0,$s1 	#a0 now has the file descriptor
	li $v0, 14 	#read file command
	la $a1, buffer	#read to "buffer"
	li $a2, 1024	#read 1024 characters
	syscall
	
	li $v0, 4	#print command	
	la $a0, buffer	#print the buffer
	syscall
		
	move $a0,$s0 	#a0 now has the filename
	li $v0, 16	#close file pointed to by a0
	syscall
	jr $ra
	
write_file:
	move $s0, $a0 	#file name in s2
	li $v0, 13 	#open file command		
	li $a1, 1 	#write mode
	syscall
	
	move $s1, $v0 	#store input file descriptor into s1
	
	li $v0, 15       	#System call for write file 
	move $a0, $s1    	#file descriptor in a0
	la $a1, P2Header 	#address of file in a1
	la $a2, 11       	#length in a2
	syscall 	
	
	li $v0, 15      	#same thing but with buffer this time
	move $a0, $s1     
	la $a1, buffer 
	la $a2, 1024    
	syscall
	
	move $a0, $s0	#load file name in a0
	li $v0, 16 	#close file
	syscall
	jr $ra
		  	  
