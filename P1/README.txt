Project 1: My little malloc()

Authors:
Krish Kharbanda (kk1297)
Riten Patel (rp1315)

Design Notes:

1. Memory Layout:
   - Uses 8-byte headers with size stored in upper bits and allocation flag in LSB
   - Maintains 8-byte alignment for all allocations
   - Minimum chunk size is 16 bytes (8-byte header + 8-byte payload)

2. Allocation Strategy:
   - First-fit algorithm for finding free chunks
   - Splits large chunks when possible to reduce waste
   - Rounds up all requests to nearest multiple of 8 for alignment

3. Coalescing:
   - Automatic coalescing on every free() operation
   - Merges adjacent free chunks to reduce fragmentation
   - Additional coalescing attempted when allocation fails

4. Error Detection:
   - Validates all pointers against heap bounds
   - Checks for proper alignment (8-byte boundaries)
   - Detects double-free attempts
   - Detects attempts to free non-heap pointers
   - All errors print to stderr and exit with status 2

5. Leak Detection:
   - Registered with atexit() during initialization
   - Scans entire heap at program termination
   - Reports total leaked bytes and object count

Test Plan:

Test Programs:

1. memtest.c
   - Tests basic allocation of 64 objects
   - Verifies data integrity with unique byte patterns
   - Tests with/without leak detection (compile flags)

2. memgrind.c (stress testing)
   - Runs 5 workloads 50 times each, reports average time
   - Task 1: Immediate malloc/free cycles
   - Task 2: Bulk allocation then bulk free
   - Task 3: Random allocation/deallocation pattern  
   - Task 4: Linked list operations (custom)
   - Task 5: Dynamic array resizing (custom)

3. test1.c (basic functionality)
   - Basic allocation and data integrity verification
   - Tests allocation after free operations
   - Tests edge cases like malloc(0) and free(NULL)

4. test2.c (coalescing and fragmentation)
   - Tests adjacent chunk coalescing
   - Creates fragmented heap and tests defragmentation
   - Tests many small allocations and bulk operations

5. test3a.c, test3b.c, test3c.c (error detection)
   - test3a: Attempts to free stack variable
   - test3b: Attempts to free offset pointer (ptr + 1)
   - test3c: Attempts double-free
   - All should exit with error code 2

6. test4.c (boundary conditions)
   - Tests very large allocations (should fail gracefully)
   - Tests exact-size allocations
   - Tests many tiny allocations
   - Tests pointer alignment
   - Tests repeated allocation/free cycles

7. test5.c (leak detection)
   - Intentionally leaks memory to test leak detector
   - Should report leaks at program exit

How to Run Tests:

Compile all programs
make

Run basic functionality tests (safe tests)
make test

Run error detection tests (these will exit with errors)
make test-errors

Run individual tests
./test1        # Basic functionality
./test2        # Coalescing
./test3a       # Error: stack variable (exits with error)
./test3b       # Error: offset pointer (exits with error)  
./test3c       # Error: double free (exits with error)
./test4        # Boundary conditions
./test5        # Leak detection
./memtest      # Standard test
./memtest-leak # With intentional leaks
./memtest-real # Using real malloc for comparison

Run stress test
./memgrind

Expected Behavior:

- All "safe" tests (test1, test2, test4, test5) should complete successfully
- Error detection tests (test3a, test3b, test3c) should exit with error messages
- memtest should show "0 incorrect bytes" if implementation is correct
- memtest-leak should show leak report at exit
- memgrind should complete all 5 tasks without errors
- test5 should show leak report: approximately 350 bytes in 3 objects

Implementation Highlights:

- Efficient space utilization: supports 64+ simultaneous allocations in 4KB heap
- Robust error checking with descriptive error messages
- Automatic memory defragmentation to reduce fragmentation
- Comprehensive test suite covering normal use, edge cases, and error conditions
- Compatible with provided test cases