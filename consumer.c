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

	// See producer.c for comments on the following lines.
	int sharedMemoryObject = shm_open("table", O_RDWR, 0666);
	int* table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, sharedMemoryObject, 0);
	 
	sem_t* itemProduced = sem_open("itemProducedSemaphore", O_CREAT, 0666, 0);
	sem_t* spaceAvailable = sem_open("spaceAvailableSemaphore", O_CREAT, 0666, 2);
	sem_t* mutex = sem_open("mutexSemaphore", O_CREAT, 0666, 1);
	
	int numberOfItems = 10;
	printf("\nConsumer ready to receive %d items.\n", numberOfItems);

	for(int i = 0; i < numberOfItems; ++i) {
		// Consumer must wait until an item is available before it can act.
		sem_wait(itemProduced);

		// Consumer waits a random amount of time before it consumes an item.
		int ms = rand() % 2 + 1.5;
		sleep(ms);

		// Acquire lock on critical section.
		sem_wait(mutex);
		// Consume item from table.
		--(*table);
		// Release lock on critical section.
		sem_post(mutex);
	
		// Inform user that item has been consumed.
		printf("Item consumed, %d remaining.\n", *table);
		// Alert producer that there is now space available for another item.
		sem_post(spaceAvailable);
	}

	// See producer.c for comments on the following lines.
	sem_close(itemProduced);
	sem_close(spaceAvailable);
	sem_close(mutex);

	sem_unlink("itemProducedSemaphore");
	sem_unlink("spaceAvailableSemaphore");
	sem_unlink("mutexSemaphore");

	munmap(table, sizeof(int));
	close(sharedMemoryObject);
	shm_unlink("table");

	printf("Consumer cleaned up!\n");
	return 0;
}
