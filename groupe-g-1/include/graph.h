
//-------------------------------------------------------- includes --------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <inttypes.h>
#include <limits.h>
#include "portable_endian.h"
#include "portable_semaphore.h"

#ifndef LEPL1503__GRAPH__
#define LEPL1503__GRAPH__

//-------------------------------------------------------- structures --------------------------------------------------------

typedef struct link
{
    uint32_t to;
    uint32_t from;
    int32_t cost;
} link_t;

typedef struct
{
    uint32_t s;
    int64_t max_cost;
    uint32_t max_node;
    uint32_t path_size;
    uint32_t *path;
} output_t;

typedef struct
{
    link_t *links;
    uint32_t nb_nodes;
    uint32_t nb_edges;
    FILE *input_file;
    FILE *output_stream;
    uint8_t nb_threads;
    bool verbose;
    output_t *buffer;
} args_t;

//-------------------------------------------------------- fonctions --------------------------------------------------------

void get_file_infos(args_t *args);

void bellman_ford(link_t *links, uint32_t nb_nodes, uint32_t nb_edges, uint32_t source, int64_t *dist, uint32_t *prev, bool verbose);

output_t get_max(int64_t *dist, uint32_t source, uint32_t nb_nodes);

uint32_t *get_path(uint32_t dest, uint32_t *prev, uint32_t source, uint32_t *path_size);

void read_graph(args_t *args);

void free_graph(args_t *graph);

#endif // LEPL1503__GRAPH__
