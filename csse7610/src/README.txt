CSSE7610 Assignment 2 - Ryan Ward 45813685

In order to solve the given program and implement in Java, each process (Reader, Writer, Incrementor) was modelled in
promela and checked that there would be no synchronisation issues. The program was closely modelled off the solution
to 1b) in the assignment, and some research was done to find the best way to implement the lock, signal and wait
functionality. The ReentrancyLock and Condition classes were used to provide monitor-like functionality as described
in part 1b). An auxilary variable was used to keep track of the number of writers waiting, to ensure that priority
was always given to  writers when necessary.

1. IWMonitor - A class that contains all synchronisation variables necessary for a monitor. Implements the startWrite,
endWrite, startIncrement, endIncrement functions to synchronise access between writer<->writer, incrementor<->incrementor
and incrementor<->writer threads.

2. SharedValues - A helper class to mimic global shared variables. Not used for synchronisation but rather for data
passing.

3. A2Reader - Implements the reader process functionality as described in the Assignment specification

4. A2Writer - Implements the writer process functionality, but allowing for multiple readers, as well as withholding
priority to itself, i.e. will always favour locking another writer process over an incrementor process.

5. A2Incrementor - Implements the incrementor process functionality, allowing for non-blocking reading of the shared
variables, and a low-priority lock to increment said variables

6. ReadWrite - The main function to execute and join all child threads.