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
sem_t * semaphore;

void child_process(int index) {
	int s;
	int counter = 0;

	printf("[processo figlio %d] entry point\n", index);

	while(1){
		if(countdown == 0){
			  printf("shutdown invocato\n\n");
			  shutdown = 1;
		  }
		if(shutdown != 0){
			exit(counter);
		}
		if (sem_wait(semaphore) == -1) {
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}

		if(countdown > 0){
			printf("[processo figlio %d] countdown diminuito\n", index);
			countdown--;
			counter++;
		}

		if (sem_post(semaphore) == -1) {
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
	}

	exit(EXIT_SUCCESS);
}

int main() {
	int s;


	semaphore = malloc(sizeof(sem_t));
	s = sem_init(semaphore,1, 1);
	CHECK_ERR(s,"sem_init")

	countdown = COUNTDOWN_VALUE;
	printf("countdown impostato\n");

	for (int i = 0; i < N; i++) {
		switch (fork()) {
		  case 0:
			  child_process(i);
			  break;

		  case -1:
			  perror("fork");
			  exit(EXIT_FAILURE);

		  default:
			  process_counter[i];
		}
	  }

/*
	while(1){
	  if(countdown == 0){
		  printf("shutdown invocato\n\n");
		  shutdown = 1;
	  }
	}*/

	for(int i = 0; i < N; i++){
		/*
		 *
		 * Creare un sistema di wait che prenda tutti i valori di uscita dei vari child e lo salvi
		 * nella cella del process_counter corrispondente al processo child terminato
		 *
		 */
	}

	for(int i = 0; i < N; i++){
		printf("processo figlio %d: %d\n", i, process_counter[i]);
	}

	return 0;
}
