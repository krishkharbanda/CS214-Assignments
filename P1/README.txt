Project 1: My little malloc()

Krish Kharbanda (kk1297) and Riten Patel (rp1315)

This project includes a set of C test programs to verify the correctness, safety, and reliability of the custom memory allocator implemented in mymalloc.c. The tests are divided into three categories: unit tests, stress tests, and negative (death) tests.

Test Plan

    Test Organization

        The tests/unit/ directory contains small, focused programs that check specific allocator features such as alignment, splitting, and coalescing.

        The tests/stress/ directory contains tests that repeatedly allocate and free memory to evaluate performance and stability under load.

        The tests/negative/ directory contains programs that intentionally trigger allocator errors such as double frees to verify that error detection works correctly.

        The tests/bin/ directory is automatically created and holds the compiled test binaries after running make.

Test Descriptions

    Unit tests:

        alloc_basics.c tests pointer alignment, verifies that allocated blocks do not overlap, checks behavior when the heap is exhausted, and ensures consistent handling of malloc(0).

        coalesce.c tests that adjacent freed blocks merge correctly into a single larger free block and that such merged blocks can satisfy larger allocations.

        free_semantics.c verifies that freeing NULL is a no-op and that allocated memory contents remain unchanged after unrelated allocations and frees.

        split_and_fragmentation.c tests that large blocks can be split into smaller usable blocks and ensures the allocator does not create unusable fragments.

    Stress tests:

        memgrind.c runs a workload of five distinct tasks to test the allocator's performance and stability. The tasks include: (1) 120 immediate allocations and frees of 1-byte objects, (2) batch allocation of 120 1-byte objects followed by a batch free, (3) a randomized sequence of allocations and deallocations, (4) building and freeing a linked list, and (5) recursively building and freeing a binary tree.
        
    Negative tests:

        double_free_test.c calls free() twice on the same pointer. The allocator should detect the double free, print an error to standard error, and exit with status code 2.

    Running the Tests
        To build all test programs, run:
        make all
        This compiles the allocator and all test files. The resulting binaries are placed in tests/bin/.

        To run all functional tests (unit and stress tests), run:
        make run
        Each test prints its own progress and results to the terminal. The process stops if any test fails.

        To run all negative (death) tests, run:
        make run-death
        These tests are expected to terminate with exit code 2. If any program exits with a different code, it will be reported as a failure.

        To run the tests with AddressSanitizer and UndefinedBehaviorSanitizer enabled, use:
        make run SAN=1
        This can help detect memory corruption, buffer overflows, and undefined behavior.

        The default header size passed to the allocator is 24 bytes. This value can be changed at build time by running:
        make run HEADERSIZE=<value>

        To remove all compiled binaries and temporary build artifacts, run:
        make clean

    All unit and stress tests should complete successfully without crashes, memory leaks, or assertion failures.
    The allocator should remain stable under all stress test scenarios.
    Death tests should exit with status code 2 when invalid memory operations occur.

Design Notes for mymalloc.c

    Our implementation of the memory allocator is based on a singly-linked list of memory chunks managed within a static, fixed-size memory pool. The primary design goals were correctness, robustness through comprehensive error checking, and simplicity.

    Heap Organization and Data Structure

        Memory Pool: The heap is implemented as a single static char array of 4096 bytes. This approach encapsulates the allocator's memory and prevents it from interfering with the system's actual heap. A union with a double is used to guarantee the heap starts on an 8-byte aligned address.

        Chunk Structure: The heap is partitioned into a series of chunks. Each chunk consists of a MetaData header immediately followed by the client's usable memory (the payload).

        Explicit Singly-Linked List: We chose an explicit, singly-linked list to manage the chunks. The MetaData struct contains:

        size_t size: The size of the payload, not including the header.

        int is_free: A flag indicating whether the chunk is available for allocation.

        struct MetaData *next: A pointer to the header of the next chunk in memory.
        This explicit next pointer simplifies heap traversal. The trade-off is a slightly larger header size (24 bytes on a 64-bit system), but it makes iterating through the list clean and efficient.

    Allocation Strategy (mymalloc)

        First-Fit Algorithm: Our allocator uses a first-fit strategy. When mymalloc is called, it traverses the linked list from the beginning of the heap and selects the first free chunk that is large enough to satisfy the request. This approach is simple to implement and generally fast for many workloads.

        Block Splitting: To preserve larger free blocks for future allocations, we implement block splitting. If a selected free chunk is larger than the requested size, it is split into two parts:

        An allocated chunk of the required size.

        A new, smaller free chunk with the remaining space.
        A split only occurs if the remaining space is large enough to hold both a MetaData header and a minimal 8-byte payload. This prevents the creation of tiny, unusable fragments that would lead to heap fragmentation.

        Alignment: All allocation requests are rounded up to the nearest multiple of 8 bytes using the bitwise operation (size + 7) & ~7. This ensures that every pointer returned by mymalloc is 8-byte aligned, as required by the C standard for accommodating any data type.

    Deallocation and Coalescing (myfree)

        Error Handling: Robustness was a major priority. Before freeing, myfree performs several checks:

        It traverses the entire heap to ensure the provided pointer corresponds exactly to the start of a payload of a chunk we manage. This catches pointers to the middle of a chunk or addresses outside the heap.

        It checks the is_free flag of the corresponding chunk to detect double-frees.
        If any check fails, an error is printed and the program terminates.

        Coalescing Strategy: To combat fragmentation, our myfree function merges adjacent free blocks. After a block is marked as is_free, a separate pass is initiated from the start of the heap. This pass iterates through the linked list and merges any two consecutive free chunks into a single, larger free chunk. While this full-pass approach is less performant than immediate neighbor coalescing (which would require a doubly-linked list), it is simpler to implement correctly and effectively reduces fragmentation over time.

    Initialization and Leak Detection

        Lazy Initialization: The heap is initialized on the first call to either mymalloc or myfree. A static is_init flag prevents the initialization routine from running more than once.

        Leak Reporting: During initialization, the find_leaks function is registered using atexit(). This function automatically runs at program termination, scans the heap for any chunks that are still marked as "in-use," and reports the total number and size of leaked objects to standard error.