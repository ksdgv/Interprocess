# Interprocess
This program implements a simple exchange of information between two processes using the `pipe()` system call in C.  It allows the user to specify a generator command and a consumer command, separated by a pipe symbol (`|`).  The standard output of the generator is connected to the standard input of the consumer.
