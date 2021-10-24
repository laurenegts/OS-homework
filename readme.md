Prerequisites:
Git, gcc, and the pthread library.
Written in Vim on RHEL 8.

Installation: 
1. Install git, gcc, and the pthread library on your system. 

2. Clone the repository: github.com/laurenegts/OS-homework/tree/master

Usage:
1. Compile the program with the following commands:
gcc producer.c -pthread -lrt -o producer
gcc consumer.c -pthread -lrt -o consumer

2. Run the program with the following command:
./producer & ./consumer &

3. Use control + c to exit the program after it has finished running.
