#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>






#ifndef LEPL1503__GRAPH__
#define LEPL1503__GRAPH__


//structure équivalente à graph_t

typedef struct link{
    uint32_t to;
    uint32_t from;
    int32_t cost;
} link_t;

typedef struct fileInfo {
    uint32_t nb_nodes;
    uint32_t nb_edges;
    link_t* links ;   //normalement, int_64
} fileInfo;

typedef struct{
    int64_t max_cost ;
    uint32_t max_node ;
} Node ;

typedef struct{
    uint32_t s;
    int64_t max_cost;
    uint32_t max_node;
    uint32_t r_size;
    uint32_t * r;
} output_t;



fileInfo* get_file_infos(FILE* fd);

void bellman_ford(link_t* links, uint32_t nb_nodes, uint32_t nb_edges, uint32_t s, int64_t *dist, uint32_t *prev, bool verbose);

Node * get_max(int64_t *dist, uint32_t s, uint32_t n);

uint32_t* get_path(uint32_t dest,uint32_t path[],uint32_t source,uint32_t *r_size );

fileInfo* read_graph(FILE *input_file);

void free_graph(fileInfo* graph);



#endif // LEPL1503__GRAPH__
