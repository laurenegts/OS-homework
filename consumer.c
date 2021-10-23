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

	int shm_fd = shm_open("table", O_RDWR, 0666);
//	ftruncate(tbl, sizeof(int));
	int* table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	 
	sem_t* fill = sem_open("fillSemaphore", O_CREAT, 0666, 0);
	sem_t* avail = sem_open("availableSemaphore", O_CREAT, 0666, 2);
	sem_t* mutex = sem_open("mutexSemaphore", O_CREAT, 0666, 1);
	
//	sem_t* fill = sem_open("fillSemaphore", O_RDWR);
//	sem_t* avail = sem_open("availableSemaphore", O_RDWR);
//	sem_t* mutex = sem_open("mutexSemaphore", O_RDWR);

	int loop = 20;
	printf("\nConsumer ready to receive %d items.\n", loop);

	for(int i = 1; i < loop; ++i) {
		printf("in consumer for loop\n");
		sem_wait(fill);
		printf("consumer after fill\n");

		int ms = rand() % 2 + 1;
		sleep(ms);
		printf("consumer out of sleep\n");

		sem_wait(mutex);
		printf("consumer in mutex\n");
		--(*table);
		sem_post(mutex);
	
		printf("Item consumed, %d remaining.\n", *table);
		sem_post(avail);
	}

	printf("consumer out of for loop\n");
	sem_close(fill);
	sem_close(avail);
	sem_close(mutex);

//	sem_unlink("fillSemaphore");
//	sem_unlink("availableSemaphore");
//	sem_unlink("mutexSemaphore");

	munmap(table, sizeof(int));
	close(shm_fd); //blah
//	shm_unlink("table");

	printf("Consumer cleaned up!\n");
	return 0;
}
