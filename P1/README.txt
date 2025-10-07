Project 1: My little malloc()

Authors:
Krish Kharbanda (kk1297)
Riten Patel (rp1315)

Design Notes:

- Our implementation uses 8-byte headers with the size in upper bits and allocation flag in LSB. All allocations are 8-byte aligned with minimum chunk size of 16 bytes.
- We used a first-fit algorithm to find free chunks. Large chunks get split when possible to reduce waste. All requests are rounded up to multiples of 8.
- For coalescing, we merged adjacent free chunks on every free() call. We also tried coalescing when malloc fails to find space.
- Error detection checks all pointers against heap bounds and alignment. We detected double-free and invalid free attempts. All errors print to stderr and exit with code 2.
- Leak detection runs at program exit using atexit(). It scans the heap and reports total leaked bytes and object count.

Test Plan:

Test Programs:

memtest.c
- Tests 64 object allocation with data integrity
- Compiles with -DLEAK flag for leak testing  

memgrind.c (stress testing)
- 5 workloads run 50 times each
- Task 1: malloc/free cycles
- Task 2: bulk operations
- Task 3: random allocation patterns
- Task 4: linked list simulation
- Task 5: dynamic array resizing

test1.c (basic functionality)
- Basic allocation and data integrity
- malloc(0) and free(NULL) handling
- Allocation after free

test2.c (coalescing)
- Adjacent chunk merging
- Fragmentation handling
- Many small allocations

test3a.c, test3b.c, test3c.c (error detection)  
- test3a: free stack variable
- test3b: free offset pointer
- test3c: double free
- All exit with error code 2

test4.c (edge cases)
- Large allocation failures
- Exact size allocations
- Tiny allocations
- Pointer alignment
- Repeated cycles

test5.c (leak detection)
- Leaks memory on purpose
- Should report leaks at exit

Running Tests:

make                # compile everything
./memtest          # should show "0 incorrect bytes" 
./test1            # basic functionality
./test2            # coalescing
./test4            # edge cases
./test5            # should show leak report
./memgrind         # stress testing

Error tests (exit with errors):
./test3a           # stack variable free
./test3b           # offset pointer free  
./test3c           # double free

Expected Results:
memtest: "0 incorrect bytes"
test1, test2, test4: all tests pass
test5: leak report with ~350 bytes in 3 objects
test3a, test3b, test3c: error messages and exit
memgrind: all 5 tasks complete successfully