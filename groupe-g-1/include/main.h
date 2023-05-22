
//-------------------------------------------------------- includes --------------------------------------------------------

#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>
#include "graph.h"

//-------------------------------------------------------- variables --------------------------------------------------------

#ifdef VAR_GLOBALES

    sem_t *semEmpty;
    sem_t *semFull;
    pthread_mutex_t mutexSource;
    pthread_mutex_t mutexBuffer;
    uint32_t Counter = 0;
    uint32_t compte = 0;
    uint32_t in = 0;
    uint32_t out = 0;

#else

    extern sem_t *semEmpty;
    extern sem_t *semFull;
    extern pthread_mutex_t mutexSource;
    extern pthread_mutex_t mutexBuffer;
    extern uint32_t Counter;
    extern uint32_t compte;
    extern uint32_t in;
    extern uint32_t out;

#endif

//-------------------------------------------------------- fonctions --------------------------------------------------------

void usage(char *prog_name);

int8_t parse_args(args_t *args, int32_t argc, char *argv[]);

void *producer(void *args);

void *consumer(void *args);