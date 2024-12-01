# collatz2


# Description:
	This program establishes a shared-memory object between the parent and 
	child processes. This technique allows the child to write the contents
       	of the sequence to the shared-memory object. The parent can then output
       	the sequence when the child completes. Because the memory is shared, 
	any changes the child makes will be reflected in the parent process as well.
	In this program, the parent and child processes must be coordinated so
       	that the parent does not output the sequence until the
	child finishes execution. These two processes will be synchronized using
	the wait() system call: the parent process will invoke wait(), which
	will suspend it until the child process exits

# To Compile: 

        gcc -Wall app.c -o collatz2 -lrt

#To Execute: 

        ./collatz2 35
