
//-------------------------------------------------------- includes --------------------------------------------------------

#include "../include/main.h"

//-------------------------------------------------------- producer --------------------------------------------------------

void *producer(void *args)
{
    /**
    
        Stocke dans le buffer la structure output_t contenant toutes les données du plus long plus court chemin pour chaque source.

        Arguments :
        -----------
        args : pointeur void vers une structure args_t contenant les arguments passés en ligne de commande par les utilisateurs. 

        Retourne :
        ----------
        empty

    **/

    args_t *data = (args_t *)args;

    while (Counter < data->nb_nodes)
    {
        // Prend une source
        pthread_mutex_lock(&mutexSource);
        int32_t source = Counter;
        Counter++;
        pthread_mutex_unlock(&mutexSource);

        // Allocation de mémoire 
        int64_t *dist = malloc(sizeof(int64_t) * data->nb_nodes);
        uint32_t *prev = malloc(sizeof(uint32_t) * data->nb_nodes);
        if (dist == NULL || prev == NULL)
        {
            free(dist);
            free(prev);
            fprintf(stderr, "Erreur d'allocation mémoire\n");
            exit(EXIT_FAILURE);
        }

        // Calcul des valeurs 
        output_t output;
        bellman_ford(data->links, data->nb_nodes, data->nb_edges, source, dist, prev, data->verbose);
        output = get_max(dist, source, data->nb_nodes);
        uint32_t path_size = 0;
        uint32_t *path = get_path(output.max_node, prev, source, &path_size);
        output.s = source;
        output.path_size = path_size;
        output.path = path;

        // Stockage dans le buffer
        sem_wait(semEmpty);
        pthread_mutex_lock(&mutexBuffer);
        data->buffer[in] = output;
        in = (in + 1) % (2 * data->nb_threads);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(semFull);

        // Libère la mémoire
        free(dist);
        free(prev);
    }

    pthread_exit(NULL);
}

//-------------------------------------------------------- consumer --------------------------------------------------------

void *consumer(void *args)
{
    /**
    
        Récupère la structure output_t contenant toutes les données du plus long plus court chemin pour chaque source pour les écrire dans le fichier binaire de sortie.
        Si aucun fichier de sortie n'est donné, les résultats sont affiché dans le terminal.

        Arguments :
        -----------
        args : pointeur void vers une structure args_t contenant les arguments passés en ligne de commande par les utilisateurs.

        Retourne :
        ----------
        empty

    **/

    args_t *data = (args_t *)args;
    while (compte < data->nb_nodes)
    {
        // Récupération des valeurs dans le buffer
        output_t consumed;
        sem_wait(semFull);
        pthread_mutex_lock(&mutexBuffer);
        consumed = data->buffer[out];
        out = (out + 1) % (2 * data->nb_threads);
        pthread_mutex_unlock(&mutexBuffer);
        sem_post(semEmpty);

        // Ecriture des résulats dans un fichier binaire (si l'utilisateur précise le fichier de sortie [-f "output-file-name"])
        if (data->output_stream != stdout)
        {
            int64_t buffer64;
            uint32_t buffer32;

            buffer32 = htobe32(consumed.s);
            size_t items_write = fwrite(&(buffer32), sizeof(uint32_t), 1, data->output_stream);
            if (items_write < 1)
            {
                fprintf(stderr, "Erreur d'écriture dans le fichier de sortie\n");
                exit(EXIT_FAILURE);
            }

            buffer32 = htobe32(consumed.max_node);
            size_t items_write1 = fwrite(&buffer32, sizeof(uint32_t), 1, data->output_stream);
            if (items_write1 < 1)
            {
                fprintf(stderr, "Erreur d'écriture dans le fichier de sortie\n");
                exit(EXIT_FAILURE);
            }

            buffer64 = htobe64(consumed.max_cost);
            size_t items_write2 = fwrite(&buffer64, sizeof(int64_t), 1, data->output_stream);
            if (items_write2 < 1)
            {
                fprintf(stderr, "Erreur d'écriture dans le fichier de sortie\n");
                exit(EXIT_FAILURE);
            }

            buffer32 = htobe32(consumed.path_size);
            size_t items_write3 = fwrite(&buffer32, sizeof(uint32_t), 1, data->output_stream);
            if (items_write3 < 1)
            {
                fprintf(stderr, "Erreur d'écriture dans le fichier de sortie\n");
                exit(EXIT_FAILURE);
            }

            for (uint32_t j = 0; j < consumed.path_size; j++)
            {
                buffer32 = htobe32(consumed.path[j]);
                size_t items_write4 = fwrite(&buffer32, sizeof(uint32_t), 1, data->output_stream);
                 if (items_write4 < 1)
                {
                    fprintf(stderr, "Erreur d'écriture dans le fichier de sortie\n");
                    exit(EXIT_FAILURE);
                }
            }
        }

        // Affichage des résultats dans le terminal (par défaut)
        else
        {
            printf("Source : %" PRIu32 "\n", consumed.s);
            printf("Destination : %" PRIu32 "\n", consumed.max_node);
            printf("Coût : %" PRIi64 "\n", consumed.max_cost);
            printf("chemin : ");

            for (uint32_t j = 0; j < consumed.path_size; j++)
            {
                printf("%" PRIu32 " ", consumed.path[j]);
            }

            printf("\n");
            printf("Nombre de noeuds : %" PRIu32 "\n", consumed.path_size);
            printf("\n");
        }

        compte++;
        free(consumed.path);
    }

    pthread_exit(NULL);
}

//-------------------------------------------------------- usage --------------------------------------------------------

void usage(char *prog_name)
{
    fprintf(stderr, "UTILISATION:\n");
    fprintf(stderr, "    %s [OPTIONS] input_file\n", prog_name);
    fprintf(stderr,
            "    input_file: chemin vers le fichier d'instance representant le "
            "graphe a traiter.\n");
    fprintf(
        stderr,
        "    -f output_file: chemin vers le fichier qui contiendra le resultat "
        "de programme, au format specifie dans l'enonce. Defaut : stdout.\n");
    fprintf(stderr,
            "    -n n_threads: nombre de threads de calcul executant "
            "l'algorithme de plus court chemin. Defaut : 4.\n");
    fprintf(stderr,
            "    -v: autorise les messages de debug. Si ce n'est pas active, "
            "aucun message de ce type ne peut etre affiche, excepte les "
            "messages d'erreur en cas d'echec. Defaut : false.\n");
}

//-------------------------------------------------------- parse_args --------------------------------------------------------

int8_t parse_args(args_t *args, int32_t argc, char *argv[])
{
    memset(args, 0, sizeof(args_t));

    // Valeurs pas défaut pour les arguments
    args->nb_threads = 4;
    args->verbose = false;
    args->output_stream = stdout;
    args->input_file = NULL;
    int32_t opt;

    while ((opt = getopt(argc, argv, "hn:vf:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            args->nb_threads = atoi(optarg);
            if (args->nb_threads == 0)
            {
                fprintf(stderr,
                        "Le nombre de threads de calcul doit etre superieur a 0 ! Recu : %s\n",
                        optarg);
                return -1;
            }
            break;
        case 'v':
            args->verbose = true;
            break;
        case 'f':
#ifdef LEPL1503_USE_HEADER_INSTANCE
            fprintf(stderr,
                    "Vous utilisez un fichier header comme instance de "
                    "graphe. Cela annule l'utilisation de l'argument donne "
                    "par -f. Veuillez retirer l'include au header si ce "
                    "n'est pas le comportement desire.\n");
            break;
#endif // LEPL1503_USE_HEADER_INSTANCE
            args->output_stream = fopen(optarg, "w+");
            if (args->output_stream == NULL)
            {
                fprintf(stderr, "Impossible d'ouvrir le fichier qui contient l'instance de graphe %s: %s\n", optarg, strerror(errno));
                return -1;
            }

            break;
        case '?':
        case 'h':
            usage(argv[0]);
            return 1;
        default:
            usage(argv[0]);
            return -1;
        }
    }

#ifdef LEPL1503_USE_HEADER_INSTANCE
    fprintf(stderr,
            "Vous utilisez un fichier header comme instance de "
            "graphe. Cela annule l'utilisation du fichier d'instance en entree "
            "donne en argument. Veuillez retirer l'include au header si ce "
            "n'est pas le comportement desire.\n");
#else
    if (optind == argc)
    {
        fprintf(stderr,
                "Vous devez fournir un fichier d'instance en entree !\n");
        return -1;
    }

    if (NULL == (args->input_file = fopen(argv[optind], "r")))
    {
        fprintf(stderr,
                "Impossible d'ouvrir le fichier qui contient l'instance de graphe %s: %s\n",
                argv[optind], strerror(errno));
        return -1;
    }
#endif // LEPL1503_USE_HEADER_INSTANCE

    return 0;
}