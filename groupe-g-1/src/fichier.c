
//-------------------------------------------------------- includes --------------------------------------------------------

#include "../include/graph.h"

//-------------------------------------------------------- get_file_info --------------------------------------------------------

void get_file_infos(args_t *args)
{
    /**

        Fonction qui récupére le nombre de noeuds et le nombre d'arêtes dans un fichier binaire contenant la description d'un graphe et les stocke dans 
        les éléments nb_nodes et nb_edges de la structure args_t donnée en argument.

        Arguments :
        -----------
        args : pointeur vers une structure args_t ayant au minimum un élément input_file représentant l'adresse du fichier d'entée.

        Retourne :
        ----------
        empty

    **/

    // Initialisation de nos convertisseurs
    uint32_t conv_nodes;
    uint32_t conv_links;

    // Lecture des données du fichier d'entrée 
    size_t items_read = fread(&conv_nodes, sizeof(uint32_t), 1, args->input_file);
    if (items_read < 1)
    {
        fprintf(stderr, "Erreur de lecture du fichier d'entrée\n");
        exit(EXIT_FAILURE);
    }

    size_t items_read1 = fread(&conv_links, sizeof(uint32_t), 1, args->input_file);
    if (items_read1 < 1)
    {
        fprintf(stderr, "Erreur de lecture du fichier d'entrée\n");
        exit(EXIT_FAILURE);
    }

    // Convertion en big endian
    args->nb_nodes = (uint32_t)be32toh(conv_nodes);
    args->nb_edges = (uint32_t)be32toh(conv_links);

    // Vérification de la validité des paramètres
    if (args->nb_nodes <= 0 || args->nb_edges <= 0)
    {
        printf("Erreur : Paramètres invalides !\n");
        exit(EXIT_FAILURE);
    }

    return;
}

//-------------------------------------------------------- read_graph --------------------------------------------------------

void read_graph(args_t *args)
{
    /**
     
        Récupère les links (de structure link_t) du graphe représenté dans le fichier donné en argument et suivant le format défini dans l'énoncé.

        Arguments :
        -----------
        args : pointeur vers une structure args_t contenant les arguments passés en ligne de commande par les utilisateurs 
               (au minimum un élément input_file représentant l'adresse du fichier d'entée).

        Retourne :
        ----------
        empty

    **/

    // Récupération de nb_nodes et nb_edges
    get_file_infos(args);

    // Allocation de la mémoire
    args->links = (link_t *)malloc(args->nb_edges * sizeof(link_t));
    if (args->links == NULL)
    {
        free(args->links);
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    // Récupération des liens
    for (uint32_t i = 0; i < args->nb_edges; i++)
    { 
        uint32_t buffer;
        int32_t bufferC;

        // Noeud de départ
        size_t items_read = fread(&buffer, sizeof(uint32_t), 1, args->input_file);
        if (items_read < 1)
        {
            fprintf(stderr, "Erreur de lecture du fichier d'entrée\n");
            exit(EXIT_FAILURE);
        }
        args->links[i].from = htobe32(buffer);

        // Noeud d'arrivé
        size_t items_read2 = fread(&buffer, sizeof(uint32_t), 1, args->input_file);
        if (items_read2 < 1)
        {
            fprintf(stderr, "Erreur de lecture du fichier d'entrée\n");
            exit(EXIT_FAILURE);
        }
        args->links[i].to = htobe32(buffer);

        // Coût
        size_t items_read3 = fread(&bufferC, sizeof(int32_t), 1, args->input_file);
        if (items_read3 < 1)
        {
            fprintf(stderr, "Erreur de lecture du fichier d'entrée\n");
            exit(EXIT_FAILURE);
        }
        args->links[i].cost = htobe32(bufferC);

        // Vérification de la validité des paramètres
        if (args->links[i].from < 0 || args->links[i].from >= args->nb_nodes || args->links[i].to < 0 || args->links[i].to >= args->nb_nodes || args->links[i].from == args->links[i].to)
        {
            printf("Erreur : Indice de noeud invalide !\n");
            exit(EXIT_FAILURE);
        }
    }

    // Affichage des liens
    if(args->output_stream == stdout)
    {
        for (uint32_t i = 0; i < args->nb_edges; i++)
        {
            printf("%" PRIu32 ", %" PRIu32 ", %" PRIi32 "\n", args->links[i].from, args->links[i].to, args->links[i].cost);
        }

        printf("\n");
    }
    return;
}

//-------------------------------------------------------- free_graph --------------------------------------------------------

void free_graph(args_t *graph)
{
    /**
    
        Libère la mémoire allouée à une structure args_t.

        Arguments :
        -----------
        graph : structure args_t à supprimer

        Retourne :
        ----------
        empty

    **/

    if (graph != NULL)
    {
        if (graph->links != NULL)
        {
            free(graph->links);
        }
        if (graph->buffer != NULL)
        {
            free(graph->buffer);
        }

        free(graph);
    }
    return;
}