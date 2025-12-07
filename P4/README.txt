CS 214 Project IV: Nim Server
Authors:
---------
Name: Krish Kharbanda
NetID: kk1297

Name: Ritenkumar Patel
NetID: rp1315

Implementation Details:
- We implemented the Concurrent Game server using pthreads.
- We attempted and implemented the Extra Credit (handling impatient moves via poll()).
- We handle all protocol error codes (10, 21-24, 31-33).

Testing:
- Tested using the provided test_suite.c which covers:
  1. Happy Path (Standard game flow)
  2. Error Cases (Framing, state violations)
  3. Concurrency (Multiple games running simultaneously)
  4. Extra Credit (Out-of-turn moves)