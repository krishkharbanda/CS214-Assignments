## Project 1: My little malloc()

Krish Kharbanda (kk1297) and Riten Patel (rp1315)

**Test Plan**

This project includes a set of standalone C test programs to verify the correctness, safety, and reliability of the custom memory allocator implemented in `mymalloc.c`. The tests are divided into three categories: unit tests, stress tests, and negative (death) tests.

---

**1. Test Organization**

* The `tests/unit/` directory contains small, focused programs that check specific allocator features such as alignment, splitting, and coalescing.
* The `tests/stress/` directory contains tests that repeatedly allocate and free memory to evaluate performance and stability under load.
* The `tests/negative/` directory contains programs that intentionally trigger allocator errors such as double frees to verify that error detection works correctly.
* The `tests/bin/` directory is automatically created and holds the compiled test binaries after running `make`.

---

**2. Test Descriptions**

Unit tests:

* `alloc_basics.c` tests pointer alignment, verifies that allocated blocks do not overlap, checks behavior when the heap is exhausted, and ensures consistent handling of `malloc(0)`.
* `coalesce.c` tests that adjacent freed blocks merge correctly into a single larger free block and that such merged blocks can satisfy larger allocations.
* `free_semantics.c` verifies that freeing `NULL` is a no-op and that allocated memory contents remain unchanged after unrelated allocations and frees.
* `split_and_fragmentation.c` tests that large blocks can be split into smaller usable blocks and ensures the allocator does not create unusable fragments.

Stress tests:

* `memgrind.c` performs 120 iterations of repeated allocation and freeing to simulate real-world usage. It includes immediate-free, batch-free, and random allocation patterns to test stability and fragmentation resistance.

Negative tests:

* `double_free_test.c` calls `free()` twice on the same pointer. The allocator should detect the double free, print an error to standard error, and exit with status code 2.

---

**3. Running the Tests**

To build all test programs, run:

```
make all
```

This compiles the allocator and all test files. The resulting binaries are placed in `tests/bin/`.

To run all functional tests (unit and stress tests), run:

```
make run
```

Each test prints its own progress and results to the terminal. The process stops if any test fails.

To run all negative (death) tests, run:

```
make run-death
```

These tests are expected to terminate with exit code 2. If any program exits with a different code, it will be reported as a failure.

To run the tests with AddressSanitizer and UndefinedBehaviorSanitizer enabled, use:

```
make run SAN=1
```

This can help detect memory corruption, buffer overflows, and undefined behavior.

To remove all compiled binaries and temporary build artifacts, run:

```
make clean
```

---

**4. Expected Results**

All unit and stress tests should complete successfully without crashes, memory leaks, or assertion failures.
The allocator should remain stable under all stress test scenarios.
Death tests should exit with status code 2 when invalid memory operations occur.

---

**5. Notes**

The default header size passed to the allocator is 24 bytes. This value can be changed at build time by running:

```
make run HEADERSIZE=<value>
```

