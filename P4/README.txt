Project IV: Nim Server
======================

Authors
-------
* Krish Kharbanda (NetID: kk1297)
* Ritenkumar Patel (NetID: rp1315)

Overview
--------
This project implements `nimd`, a concurrent TCP game server for the game of Nim. 

We implemented the extra credit version of the assignment that supports concurrent games.

Compilation
-----------
To compile the server, clients, and test suite, simply run:

    make

This produces the following executables:
* `nimd`: The game server.
* `testc`: An interactive client for playing manually.
* `rawc`: A raw debugging client for sending manual protocol strings.
* `test_suite`: An automated testing program.

Running the Server
------------------
Start the server by specifying a listening port (e.g., 5001):

    ./nimd 5001

Testing
-------
We have provided a comprehensive `test_suite` that verifies all project requirements, including concurrency and the extra credit features.

Automated Testing 
You can run all tests sequentially using the Makefile.
Note: You must start the server in a separate terminal before running the tests.

1.  **Terminal 1 (Server):**
    ```bash
    ./nimd 5001
    ```

2.  **Terminal 2 (Test Suite):**
    ```bash
    make test
    ```

This runs 5 test scenarios:
1.  Happy Path: Simulates a full valid game between two bots.
2.  Concurrency: Runs multiple games simultaneously to ensure threads do not block each other.
3.  Basic Errors: Checks for handling of long names, duplicate logins, and framing errors.
4.  Game Logic: Verifies detection of illegal moves (invalid pile/quantity) and forfeits on disconnect.
5.  Extra Credit: Verifies that the server sends `FAIL 31` (Impatient) immediately if a player moves out of turn.

Manual Testing
You can also play the game interactively using `testc` or `rawc`.

testc (Interactive Client)
Usage: `testc <host> <port> <name>`
Connects to the server and prompts you to move when it is your turn. Prevents you from making illegal local moves.

    ./testc localhost 5000 Alice

rawc (Debugging Client)**
Usage: `rawc <host> <port>`
Allows you to type raw NGP protocol strings directly to the server. Useful for debugging specific byte sequences or malformed messages.

    ./rawc localhost 5000
    # Type input like: 0|06|OPEN|Dave|