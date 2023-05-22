
//-------------------------------------------------------- includes --------------------------------------------------------

#define VAR_GLOBALES
#include "include/main.h"

//-------------------------------------------------------- main --------------------------------------------------------

int main(int32_t argc, char *argv[])
{
    // Allocation de mémoire pour les données fournies en entrée
    args_t *args = (args_t *)malloc(sizeof(args_t));
    if (args == NULL)
    {
        free(args);
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    // Récupération des données fournies en entrée
    int8_t err = parse_args(args, argc, argv);
    if (err == -1)
    {
        exit(EXIT_FAILURE);
    }
    else if (err == 1)
    {
        exit(EXIT_SUCCESS);
    }
    read_graph(args);

    // Allocation de mémoire pour le buffer producteur/consommateur
    args->buffer = (output_t *)malloc(sizeof(output_t) * args->nb_threads * 2);
    if (args->buffer == NULL)
    {
        free(args->buffer);
        fprintf(stderr, "Erreur d'allocation mémoire\n");
        exit(EXIT_FAILURE);
    }

    // Argument du producteur et du consommateur
    void *info = args;

    // Aide au débugage
    if (args->verbose)
    {
        fprintf(stderr, "Nombre de noeuds: %" PRIu32 "\n", args->nb_nodes);
        fprintf(stderr, "Nombre de liens: %" PRIu32 "\n", args->nb_edges);
        for (uint32_t i = 0; i < args->nb_edges; ++i)
        {
            fprintf(stderr,
                    "Lien: %" PRIu32 " -> %" PRIu32 " (cout de %" PRIi32 ")\n",
                    (uint32_t)args->links[i].from, (uint32_t)args->links[i].to,
                    (int32_t)args->links[i].cost);
        }
    }

    // Ecriture de nb_nodes et nb_edges
    uint32_t buffer32;
    buffer32 = htobe32(args->nb_nodes);
    size_t items_write = fwrite(&buffer32, sizeof(uint32_t), 1, args->output_stream);
    if (items_write < 1)
    {
        fprintf(stderr, "Erreur d'écriture dans le fichier de sortie\n");
        exit(EXIT_FAILURE);
    }

    // Initialisation
    uint8_t THREAD_NUM = args->nb_threads;
    pthread_t th[THREAD_NUM];
    pthread_mutex_init(&mutexSource, NULL);
    pthread_mutex_init(&mutexBuffer, NULL);
    semEmpty = my_sem_init(2 * args->nb_threads);
    semFull = my_sem_init(0);

    // Création des threads
    for (uint32_t i = 0; i <= THREAD_NUM; i++)
    {
        if (i > 0)
        {
            if (pthread_create(&th[i], NULL, producer, info) != 0)
            {
                perror("Impossible de créer un thread\n");
            }
        }
        else
        {
            if (pthread_create(&th[i], NULL, consumer, info) != 0)
            {
                perror("Impossible de créer un thread/n");
            }
        }
    }

    // Threads
    for (uint32_t i = 0; i <= THREAD_NUM; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Echec du join thread/n");
        }
    }

    // Destruction des sémaphores et mutex
    my_sem_destroy(semEmpty);
    my_sem_destroy(semFull);
    pthread_mutex_destroy(&mutexSource);
    pthread_mutex_destroy(&mutexBuffer);

    // Fermeture le fichier d'entrée
    int32_t inputClose = fclose(args->input_file);
    if (inputClose == EOF) 
    {
        fprintf(stderr, "Erreur durant la fermeture du fichier d'entrée");
        exit(EXIT_FAILURE);
    }
    
    // Fermeture du fichier de sortie 
    if (args->output_stream != stdout)
    {
        int32_t outputClose = fclose(args->output_stream);
        if (outputClose == EOF)
        {
            fprintf(stderr, "Erreur durant la fermeture du fichier de sortie");
            exit(EXIT_FAILURE);
        }
    }

    // Libère la mémoire 
    free_graph(args);

    return 0;
}