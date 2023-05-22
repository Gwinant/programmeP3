#include "../include/graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>     //to use INFINITY
#include "../include/portable_endian.h"
#include <inttypes.h>



void bellman_ford(link_t* links, uint32_t nb_nodes, uint32_t nb_edges, uint32_t s, int64_t *dist, uint32_t *prev, bool verbose) {
/*
    bellman_ford - Implémentation de l'algorithme de Bellman-Ford pour trouver le plus court chemin dans un graphe pondéré.
    @param links : tableau de pointeurs vers les liens (arrêtes) du graphe. Chaque lien est représenté par un tableau de 3 entiers : le noeud de départ, le noeud d'arrivée et le poids de l'arrête.
    @param nb_edges : nombre de liens dans le graphe.
    @param s : noeud de départ pour la recherche du plus court chemin.
    @param verbose : un booleen
    @param dist : tableau pour stocker les distances les plus courtes entre le noeud de départ et tous les autres noeuds du graphe.
    @param nb_nodes : nombre total de noeuds dans le graphe.

*/

    // Initialiser les tableaux de distance et de précédence
    for (uint32_t i = 0; i < nb_nodes; i++) {
        dist[i] = INT64_MAX;
        prev[i] = -1;
    }
    dist[s] = 0;

    // Exécuter l'algorithme Bellman-Ford
    for (uint32_t i = 0; i < nb_nodes-1; i++) {
        for (uint32_t j = 0; j < nb_edges; j++){
            uint32_t node_a = links[j].from;
            uint32_t node_b = links[j].to;
            int32_t cost_ab = links[j].cost;


            if (dist[node_a] != INT64_MAX && dist[node_a] + cost_ab < dist[node_b]) {
                dist[node_b] = dist[node_a] + cost_ab;
                prev[node_b] = node_a;
            }
        }
    }

    // Vérifier s'il y a un cycle négatif
    for (uint32_t j = 0; j < nb_edges; j++) {
        uint32_t node_a = links[j].from;
        uint32_t node_b = links[j].to;
        int32_t cost_ab = links[j].cost;


        if (dist[node_a] != INT64_MAX && dist[node_a] + cost_ab < dist[node_b]) {
            if (verbose) {
                printf("Cycle négatif détecté !\n");
            }

            // Réinitialiser les tableaux de distance et de précédence
            for (uint32_t k = 0; k < nb_nodes; k++) {
                dist[k] = INT64_MIN;
                prev[k] = -1;
            }
            dist[s] = 0;
            
        }
    }
    return;
}




fileInfo* get_file_infos(FILE* fd) {
    /*
    get_file_infos - Fonction pour extraire le nombre de noeuds et le nombre d'arêtes d'un fichier binaire contenant la description d'un graphe.
    @param binary_data : nom du fichier binaire contenant la description du graphe.
    @return fileInfo* : pointeur vers une structure contenant les informations extraites.
    */

    fileInfo* new_info = (fileInfo*)malloc(sizeof(fileInfo));
    if(new_info == NULL){
        fprintf(stderr, "erreur lors d'allocation de mémoire\n");
        exit(EXIT_FAILURE);
    }

    //initialisation de nos convertisseurs
    uint32_t conv_nodes;
    uint32_t conv_links;

    fread(&conv_nodes, sizeof(uint32_t), 1, fd); //lit un bloc de données binaires de taille sizeof(uint32_t) à partir du fichier pointé par fd
    fread(&conv_links, sizeof(uint32_t), 1, fd);

    //on convertit en big endian

    new_info->nb_nodes = (uint32_t)be32toh(conv_nodes);
    new_info->nb_edges = (uint32_t)be32toh(conv_links);

    if (new_info->nb_nodes <= 0 || new_info->nb_edges <= 0) {
        printf("Erreur : paramètres invalides !\n");
        exit(EXIT_FAILURE);
    }

    return new_info;
}


Node * get_max(int64_t *dist, uint32_t s, uint32_t n){     //n = FILEINFO -> nb_nodes

    /*
    Retourne l'indice du noeud dont il existe un chemin de s vers ce noeud
    et le cout de ce chemin est le plus élevé parmis tous les noeuds ayant un chemin
    depuis s.
    */

    int64_t max_cost = INT64_MIN ;
    uint32_t max_node = s ;

    for (uint32_t node_idx = 0; node_idx <n; node_idx++){         //si dist[node_idx] != -1, on a parcouru tout l'array dist
        if ( dist[node_idx] != INT64_MAX && dist[node_idx] > max_cost){
            max_cost = dist[node_idx];
            max_node = node_idx ;
        }
    }

    if (max_cost == INT64_MIN){  // je ne pense pas qu'on puisse écrire une constante en négatif directement
        max_cost = dist[s];

    }


    Node *destination = (Node *) malloc(sizeof(Node));   //création d'un pointeur vers la structure Node

    if (destination == NULL){           //vérification de la valeur de retour du malloc()
        free(destination);
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }
    destination->max_cost = max_cost ;
    destination->max_node = max_node ;

    return destination ;


}


uint32_t* get_path(uint32_t dest,uint32_t path[],uint32_t source,uint32_t *r_size ){

    //Retourne une liste contenant le chemin de source vers dest en utilisant le tableau de précédence path

    uint32_t i = dest;
    uint32_t size = 1;

    uint32_t* r = (uint32_t *)malloc(sizeof(uint32_t)*size);
    if (r == NULL){                           //vérif de la valeur de retour
        free(r);
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }
    r[0]=dest;
    while (i != source){                             //boucle jusqu'à la source
        size+=1;
        r = (uint32_t *)realloc(r, sizeof(uint32_t)*size);   //agrandit l'array
        if (r == NULL){                       //vérification de la valeur de retour
            free(r);
            fprintf(stderr, "Erreur d'allocation mémoire\n");
            exit(EXIT_FAILURE);
        }
        for (uint32_t j = size -1; j >0; j--){       //on commence par la droite pour décaler les éléments
            r[j] = r[j-1];                           //décale tout les éléments vers la fin
        }
        r[0]=path[i];                                //rajoute le noeud au débout de l'array
        i=path[i];                                   //prochain noeud
    }

    *r_size = size;

    return r;

}

fileInfo* read_graph(FILE *input_file) {
    /**
    Récupère le graphe représenté dans le fichier donné en argument.
            Suit le format défini dans l'énoncé.
    **/
    fileInfo* val = get_file_infos(input_file); // -> get_file_infos(input_file)
    //fseek(input_file, 8, SEEK_SET); // -> fseek(input_file, 0, SEEK_SET

    //initialisation de la variable links (allocation de la mémoire pour tous les edges)
    val -> links =  (link_t *) malloc(val->nb_edges * sizeof(link_t));


    if (val -> links == NULL){       //vérification de la valeur de retour du malloc()
        free(val -> links) ;
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    for(uint32_t i = 0; i < val->nb_edges; i++){    //on lit le fichier
        uint32_t buffer ;
        int32_t bufferC ;

        //for(int j = 0; j < 3; j++){        //On parcoure les 3 valeurs
        fread(&buffer, sizeof(uint32_t), 1, input_file);    //on stocke la valeur lue dans buffer

        /*if (fread(&buffer, sizeof(uint32_t), 1, input_file) < 1){   //fread() should return nitems (here,1) on success and an error otherwise
            fprintf(stderr, "Error reading input file\n");
            exit(EXIT_FAILURE);
        }*/
        val -> links[i].from = htobe32(buffer);    //htobe32 = équivalent de int.from_bytes
        fread(&buffer, sizeof(uint32_t), 1, input_file);    //on stocke la valeur lue dans buffer
        val -> links[i].to = htobe32(buffer);    //htobe32 = Ã©quivalent de int.from_bytes
        fread(&bufferC, sizeof(int32_t), 1, input_file);    //on stocke la valeur lue dans buffer
        val -> links[i].cost = htobe32(bufferC);    //htobe32 = Ã©quivalent de int.from_bytes
        if ( val ->links[i].from< 0 || val ->links[i].from >= val ->nb_nodes || val ->links[i].to < 0 || val ->links[i].to >= val ->nb_nodes || val ->links[i].from == val ->links[i].to) {
                printf("Erreur : indice de noeud invalide !\n");
                exit(EXIT_FAILURE);
        }
    }


    //affichage des edges
    for (uint32_t i = 0; i < val->nb_edges; i++) {
        printf("%" PRIu32 ", %" PRIu32 ", %" PRIi32 "\n", val->links[i].from, val->links[i].to, val->links[i].cost);
    }

    printf("\n");

    return val ;


}


void free_graph(fileInfo* graph){

    //Libère la mémoire allouée pour un graphe

    if (graph != NULL){

        if (graph->links != NULL){
            free(graph->links);
        }

        free(graph);
    }
}
