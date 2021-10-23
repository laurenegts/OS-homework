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
	int tbl = shm_open("table", O_CREAT | O_RDWR, 0666);
	ftruncate(tbl, sizeof(int));
	int* table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, tbl, 0);
	
	sem_t* fill = sem_open("fillSemaphore", O_CREAT, 0666, 0);
	sem_t* avail = sem_open("availableSemaphore", O_CREAT, 0666, 2);
	sem_t* mutex = sem_open("mutexSemaphore", O_CREAT, 0666, 1);

	sem_init(fill, 1, 0);
	sem_init(avail, 1, 2);
	sem_init(mutex, 1, 1);

	int loop = 20;
	printf("\nProducer ready to create %d items.\n", loop);

//	sem_post(avail);

	for(int i = 0; i < loop; ++i) {
	//	printf("in producer for loop\n");
		//printf("producer after sem_post(avail)\n");
		sem_wait(avail);
	//	printf("producer after sem_wait(avail)\n");

		int ms = rand() % 2 + 1;
		sleep(ms);
	//	printf("producer done sleeping\n");

		sem_wait(mutex);
	//	printf("producer in mutex\n");
		++(*table);
		sem_post(mutex);

		printf("Item produced, there are now %d item(s) in the table.\n", *table);
		sem_post(fill);
	}

//	printf("producer out of for loop\n");
	sem_close(fill);
	sem_close(avail);
	sem_close(mutex);

	sem_unlink("fillSemaphore");
	sem_unlink("availableSemaphore");
	sem_unlink("mutexSemaphore");

	munmap(table, sizeof(int));
	close(tbl);
	shm_unlink("table");

	printf("Producer cleaned up!\n");
	return 0;
}
