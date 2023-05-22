#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>

int substract_timeval(struct timeval* result, struct timeval *x, struct timeval* y);
double execute(int i);
void execute_fils(int i);
void save_csv(char* filename, double *temps, int th);

char* filename_csv = "test_perf/timer.csv";

char* graph = "./graph/graph1000.bin";
int threads = 1;

int main(int argc, char** argv){
    int th = threads, i;
    double temps[th];

    //Lancement de l'exécution de mise en cache
    execute(0);
    for(i=1; i<=th; i++){
        double ti = execute(i);
        temps[i-1] = ti;
        sleep(0.5);
    }

    save_csv(filename_csv, temps, th);

    printf("\n");
    return EXIT_SUCCESS;
}

double execute(int i){
    struct timeval start, stop, temps;
    int status = 0;
    gettimeofday(&start, NULL);
    pid_t pid = fork();
    if(pid == 0){ //fils
        execute_fils(i);
    } else if (pid < 0) { // Erreur lors de la création du processus fils
       printf("Erreur lors de l'exécution avec %d threads (fork).", i);
       return 0.0;
    } else { // pere
        int fils = waitpid(pid, &status, 0);
        gettimeofday(&stop, NULL);
        substract_timeval(&temps, &stop, &start);
        if(fils == -1){ exit(EXIT_FAILURE);}
        double s = (double) temps.tv_sec;
        double us = ((double) temps.tv_usec) / 1000000;
        return  (s + us);
    }
    return -1.0;
}

// Si i > 0, exécution normale avec i threads de calcul
// Si i == 0, exécution de mise en cache avec 8 threads
void execute_fils(int i) {
    char* env[] = {"LANG=fr", NULL};
    char *arg[5];
    arg[0] = "./main";
    arg[1] = "-n";
    char tab[3];
    arg[2] = tab;
    arg[3] = graph;
    arg[4] = NULL;

    //Calcul de la longueur du nombre i lors
    //de sa conversion en string
    if (i > 0) {
        size_t size = ((int) log10(i) + 2) * sizeof(char);
        snprintf(arg[2], size, "%d", i);
        tab[3] = '\0';
        //snprintf(arg[4], 2*sizeof(char), "%d", i);
    } else {
        snprintf(arg[2], 2*sizeof(char), "%d", 8);
        tab[3] = '\0';
    }

    //Lancement de l'exécutable
    execve(arg[0], arg, env);
}


int substract_timeval(struct timeval* result, struct timeval *x, struct timeval* y) {
    if (x->tv_usec < y->tv_usec) {
        int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
        y->tv_usec -= 1000000 * nsec;
        y->tv_sec += nsec;
    }
    if (x->tv_usec - y->tv_usec > 1000000) {
        int nsec = (x->tv_usec - y->tv_usec) / 1000000;
        y->tv_usec += 1000000 * nsec;
        y->tv_sec -= nsec;
    }
    result->tv_sec = x->tv_sec - y->tv_sec;
    result->tv_usec = x->tv_usec - y->tv_usec;
    
    return x->tv_sec < y->tv_sec;
}

void save_csv(char* filename, double *temps, int th) {
    FILE *fd = fopen(filename, "w");
    if (fd == NULL) {
        printf("Erreur lors de l'ouvberture du fichier csv (fopen)");
    }
    
    fprintf(fd, "%s,%s\n", "Nombre de threads", "temps [s]");

    int i;
    for(i = 0; i<th; i++) {
        fprintf(fd, "%d,%f\n", i+1, temps[i]);
    }
    fclose(fd);
}


