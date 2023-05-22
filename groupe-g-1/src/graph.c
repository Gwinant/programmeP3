
//-------------------------------------------------------- includes --------------------------------------------------------

#include "../include/graph.h"

//-------------------------------------------------------- bellman_ford --------------------------------------------------------

void bellman_ford(link_t *links, uint32_t nb_nodes, uint32_t nb_edges, uint32_t source, int64_t *dist, uint32_t *prev, bool verbose)
{
    /**

        Implémentation de l'algorithme de Bellman-Ford pour trouver le plus court chemin dans un graphe pondéré.
        Calcule et stocke les distances les plus courtes entre le noeud de départ et tous les autres noeuds du graphe dans l'array dist
        ainsi que le noeud précédent chaque destination dans l'array prev.
        Si un noeud est isolé, sa distance vaut INT64_MAX et son précédant est -1.
        Si le noeud source est isolé, toute les distanes valent INT64_MAX sauf sa distance par rapport à lui-même qui vaut 0 et tout les précédants valent -1.
        Si un cycle négatif est détecté, la distance pour ce noeud vaut INT64_MIN et son précédant est -1.


        Arguments :
        -----------
        links : pointeur vers un tableau qui contient les liens (arêtes) du graphe.
                Chaque lien est représenté par une structure link_t composée de 3 entiers : le noeud de départ, le noeud d'arrivée et le poids de l'arrête.
        nb_nodes : entier non signé sur 32 bytes représentant le  nombre total de noeuds dans le graphe.
        nb_edges : entier non signé sur 32 bytes qui représente le nombre de liens dans le graphe.
        source : entier non signé sur 32 bytes qui représente le noeud de départ (source) pour la recherche du plus court chemin.
        dist : pointeur vers un tableau d'entiers signés sur 64 bytes pour stocker les distances les plus courtes pour attendre chaque noeud depuis la source.
        prev : pointeur vers un tableau d'entiers non signés sur 32 bytes pour stocker le noeud précédent chaque destination.
        verbose : booléen indiquant si le programme doit afficher les aides au débugage.

        Retourne :
        ----------
        empty

    **/

    // Initialisation  des tableaux de distance et de précédence
    for (uint32_t i = 0; i < nb_nodes; i++)
    {
        dist[i] = INT64_MAX;
        prev[i] = -1;
    }
    dist[source] = 0; // distance depuis noeud source vers lui même est nulle

    // Exécution de l'algorithme de Bellman-Ford
    for (uint32_t i = 0; i < nb_nodes - 1; i++)
    {
        for (uint32_t j = 0; j < nb_edges; j++)
        {
            uint32_t node_a = links[j].from;
            uint32_t node_b = links[j].to;
            int32_t cost_ab = links[j].cost;

            if (dist[node_a] != INT64_MAX && dist[node_a] + cost_ab < dist[node_b])
            {
                dist[node_b] = dist[node_a] + cost_ab;
                prev[node_b] = node_a;
            }
        }
    }

    // Vérification de cycle négatif
    for (uint32_t j = 0; j < nb_edges; j++)
    {
        uint32_t node_a = links[j].from;
        uint32_t node_b = links[j].to;
        int32_t cost_ab = links[j].cost;

        if (dist[node_a] != INT64_MAX && dist[node_a] + cost_ab < dist[node_b])
        {
            // Aide au débugage
            if (verbose)
            {
                printf("Cycle négatif détecté !\n");
            }

            // Applications des valeurs de retour d'un cycle négatif
            for (uint32_t k = 0; k < nb_nodes; k++)
            {
                dist[k] = INT64_MIN;
                prev[k] = -1;
            }
            dist[source] = 0;
        }
    }
    return;
}

//-------------------------------------------------------- get_max --------------------------------------------------------

output_t get_max(int64_t *dist, uint32_t source, uint32_t nb_nodes)
{
    /**

        Retourne l'indice du noeud dont il existe un chemin de source vers ce noeud et le coût de ce chemin est le plus élevé parmis tous les noeuds ayant 
        un chemin depuis la source. Si il y a un cycle négatif ou que le noeud source est isolé, retourne la source et un coût de 0.

        Arguments :
        -----------
        dist : pointeur vers un tableau d'entiers signés sur 64 bytes pour stocker les distances les plus courtes pour attendre chaque noeud depuis la source.
        source : entier non signé sur 32 bytes qui représente le noeud de départ (source).
        nb_nodes : entier non signé sur 32 bytes représentant le  nombre total de noeuds dans le graphe.

        Retourne :
        ----------
        destination : structure output_t ayant 2 éléments : max_cost et max_node

    **/

    // Initialisation    
    int64_t max_cost = INT64_MIN;
    uint32_t max_node = source;
    
    // Comparaison
    for (uint32_t node_idx = 0; node_idx < nb_nodes; node_idx++)
    {
        if (node_idx != source && dist[node_idx] != INT64_MAX && dist[node_idx] >= max_cost)
        {
            max_cost = dist[node_idx];
            max_node = node_idx;
        }
    }

    // Si pas de chemin
    if (max_cost == INT64_MIN)
    {
        max_node = source;
        max_cost = dist[source];
    }

    // Structure de sortie
    output_t destination;
    destination.max_cost = max_cost;
    destination.max_node = max_node;

    return destination;
}

//-------------------------------------------------------- get_path --------------------------------------------------------

uint32_t *get_path(uint32_t dest, uint32_t *prev, uint32_t source, uint32_t *path_size)
{
    /**

        Retourne une liste contenant le chemin de source vers dest en utilisant le tableau de précédence path.
        Stocke la taille de cette liste dans la variable path_size.
        Si le chemin vers ce noeud n'existe pas, path = [source], path_size = 1

        Arguments :
        -----------
        dest : entier non signé sur 32 bytes représentant le noeud destination.
        prev : pointeur vers un tableau d'entiers non signés sur 32 bytes où sont stocké les noeuds précédent chaque destination.
        source : entier non signé sur 32 bytes représentant le noeud source.
        path_size : pointeur vers entier non signé sur 32 bytes qui contiendra la taille du chemin.

        Retourne :
        ----------
        path : tableau d'entiers non signés sur 32 bytes contenant le chemin pour atteindre la destination dest depuis source.
        
    **/

    // Allocation de mémoire
    uint32_t *path = (uint32_t *)malloc(sizeof(uint32_t));
    if (path == NULL)
    {
        free(path);
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    // Initialisation
    uint32_t node = dest;
    uint32_t size = 1;
    path[0] = dest;

    // Ajout du précédent au début
    while (node != source)
    {
        size++;
        path = (uint32_t *)realloc(path, sizeof(uint32_t) * size);
        if (path == NULL)
        {
            free(path);
            fprintf(stderr, "Erreur d'allocation mémoire\n");
            exit(EXIT_FAILURE);
        }
        for (uint32_t j = size - 1; j > 0; j--)
        {
            path[j] = path[j - 1];
        }
        path[0] = prev[node];
        node = prev[node];
    }

    // Stockage de la taille du chemin
    *path_size = size;

    return path;
}