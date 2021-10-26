#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

int main() {
	// Open a POSIX shared memory object that can be read from and written to.
	// Owner, group, and other all have read and write permissions
	int sharedMemoryObject = shm_open("table", O_CREAT | O_RDWR, 0666);
	
	// Set the size of the shared memory space to the number of bits in the int data type
	ftruncate(sharedMemoryObject, sizeof(int));

	// Creates a new mapping in the virtual address space.  Kernel chooses where in memory
	// to start the mapping.  Size is limited to the number of bits in the int data type.
	// The mapping can be read from and written to.  It is visible to other processes
	// (namely the consumer process) that also map this region of memory.  Its contents
	// begin at the memory location specified by sharedMemoryObject, and has no offset.
	int* table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryObject, 0);
	
	// Create the necessary semaphores.  itemProduced is for the producer to indicate
	// that it has produced an item, so the consumer now has an item to consume.  Its
	// original value is 0 because initally there are no items available.  spaceAvailable
	// is to indicate that there is space available in the shared memory for the producer
	// to put an item into.  Its initial value is 2 because per the project instructions,
	// the shared memory buffer is to hold at most two items.  mutex is to act as a mutex
	// lock for the critical section, the table.  Its initial value is 1 because nothing 
	// should be able to access the critical section until necessary. 
	sem_t* itemProduced = sem_open("itemProducedSemaphore", O_CREAT, 0666, 0);
	sem_t* spaceAvailable = sem_open("spaceAvailableSemaphore", O_CREAT, 0666, 2);
	sem_t* mutex = sem_open("mutexSemaphore", O_CREAT, 0666, 1);

	int numberOfItems = 10;
	printf("\nProducer ready to create %d items.\n", numberOfItems);

	for(int i = 0; i < numberOfItems; ++i) {
		// Producer must wait until there is space available in the table for another item.
		sem_wait(spaceAvailable);

		// Producer waits a random amount of time before creating an item.
		int ms = rand() % 2 + 1;
		sleep(ms);

		// Acquire lock on critical section.
		sem_wait(mutex);
		// Add item to table.
		++(*table);
		// Release lock on critical section.
		sem_post(mutex);

		// Inform user that item has been produced.
		printf("Item produced, there are now %d item(s) in the table.\n", *table);
		// Alert consumer that item has been produced.
		sem_post(itemProduced);
	}
	
	// Close the semaphores to release their resources back to the system.
	sem_close(itemProduced);
	sem_close(spaceAvailable);
	sem_close(mutex);

	// Destroy the semaphores.
	sem_unlink("itemProducedSemaphore");
	sem_unlink("spaceAvailableSemaphore");
	sem_unlink("mutexSemaphore");
	
	// Remove any mappings to the memory allocated when mmap was called.
	munmap(table, sizeof(int));
	// Close the shared memory object.
	close(sharedMemoryObject);

	// Remove the shared memory object that table points to.
	shm_unlink("table");

	printf("Producer cleaned up!\n");
	return 0;
}
