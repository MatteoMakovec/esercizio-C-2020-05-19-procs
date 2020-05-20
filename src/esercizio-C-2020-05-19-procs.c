//N = 10
//
//un processo padre crea N processi figli ( https://repl.it/@MarcoTessarotto/crea-n-processi-figli )
//
//shared variables: countdown, process_counter[N], shutdown
//
//usare mutex per regolare l'accesso concorrente a countdown
//
//dopo avere avviato i processi figli, il processo padre dorme 1 secondo e poi imposta il valore di countdown al valore 100000.
//
//quando countdown == 0, il processo padre imposta shutdown a 1.
//
//aspetta che terminino tutti i processi figli e poi stampa su stdout process_counter[].
//
//i processi figli "monitorano" continuamente countdown:
//- processo i-mo: se countdown > 0, allora decrementa countdown ed incrementa process_counter[i]
//- se shutdown != 0, processo i-mo termina

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#define N 10
#define COUNTDOWN_VALUE 100

#define CHECK_ERR(a,msg) {if ((a) == -1) { perror((msg)); exit(EXIT_FAILURE); } }

int countdown, process_counter[N], shutdown;
int process_count;
sem_t * semaphore;
char * addr;

void child_process(int index) {
	while(1){
		if(addr[countdown] == 0){
			addr[shutdown] = 1;
		  }
		if(addr[shutdown] != 0){
			exit(EXIT_SUCCESS);
		}

		if (sem_wait(semaphore) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}


		if(addr[countdown] > 0){
			addr[countdown] -= 1;
			addr[process_count+index] += 1;
		}


		if (sem_post(semaphore) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
	}
}

int main() {
	int s;

	semaphore = malloc(sizeof(sem_t));
	s = sem_init(semaphore,1, 1);
	CHECK_ERR(s,"sem_init")

	addr = mmap(NULL, sizeof(int)+sizeof(process_counter)+sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	countdown = 0;
	process_count = sizeof(int);
	shutdown = sizeof(int)+sizeof(process_counter);

	addr[countdown] = COUNTDOWN_VALUE;

	for (int i = 0; i < N; i++) {
		switch (fork()) {
		  case 0:
			  child_process(i);
			  break;

		  case -1:
			  perror("fork() error");
			  exit(EXIT_FAILURE);

		  default:
			  ;
		}
	  }

	for(int i = 0; i < N; i++){
		if(wait(NULL) == -1){
			perror("wait() error");
		}
	}

	for(int i = 0; i < N; i++){
		printf("processo figlio %d: %d\n", i, addr[process_count+i]);
	}

	return 0;
}
