compile using make

if fails use:
gcc -g a3_test.c sma.c

a3_test.c has following modifications:
	test2 has been moved after test5, for reasons discussed below:
	possible calls of printFreeList() that enumerates free blocks and their addresses FOR DEBUG

note that many of the returns are of the form "block+FREE_BLOCK_HEADER_SIZE" because i am returning the usable space, and ignoring the header. By corollary, block pointers point to its struct header by default.


testing output (lab2-3):

/home/2019/zpi/Projects/COMP 310 A3$ ./a.out
Test 1: Excess Memory Allocation...
				 PASSED

Test 3: Check for Worst Fit algorithm...
				 PASSED

Test 4: Check for Next Fit algorithm...
				 PASSED

Test 5: Check for Reallocation with Next Fit...
				 PASSED

Test 2: Program break expansion test...
				 PASSED

Test 6: Print SMA Statistics...
===============================
Total number of bytes allocated: 25998336
Total free space: 2738048
Size of largest contigious free space (in bytes): 65472
