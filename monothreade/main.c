#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/portable_endian.h"
#include "include/portable_semaphore.h"
#include "include/graph.h"

typedef struct {
    FILE *input_file;
    FILE *output_stream;
    uint8_t nb_threads;
    bool verbose;
} args_t;

void usage(char *prog_name) {
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

int8_t parse_args(args_t *args, int32_t argc, char *argv[]) {
    memset(args, 0, sizeof(args_t));

    // Valeurs par defaut pour les arguments.
    args->nb_threads = 4;
    args->verbose = false;
    args->output_stream = stdout;
    args->input_file = NULL;
    int32_t opt;
    while ((opt = getopt(argc, argv, "hn:vf:")) != -1) {
        switch (opt) {
            case 'n':
                args->nb_threads = atoi(optarg);
                if (args->nb_threads == 0) {
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
                if (args->output_stream == NULL) {
                    fprintf(stderr, "Impossible to open the output file %s: %s\n", optarg, strerror(errno));
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
    if (optind == argc) {
        fprintf(stderr,
                "Vous devez fournir un fichier d'instance en entree !\n");
        return -1;
    }

    if (NULL == (args->input_file = fopen(argv[optind], "r"))) {
        fprintf(stderr,
                "Impossible d'ouvrir le fichier qui contient l'instance de graphe %s: %s\n",
                argv[optind], strerror(errno));
        return -1;
    }
#endif  // LEPL1503_USE_HEADER_INSTANCE

    return 0;
}

int main(int32_t argc, char *argv[]) {
    args_t args;
    int32_t err = parse_args(&args, argc, argv);
    if (err == -1) {
        exit(EXIT_FAILURE);
    } else if (err == 1) {
        exit(EXIT_SUCCESS);
    }


    fileInfo * data = read_graph(args.input_file);

  
    if (args.verbose) {
        // Cela permet de gerer les differentes plateformes comme le raspberry.
        fprintf(stderr, "Nombre de noeuds: %" PRIu32 "\n", data -> nb_nodes);
        fprintf(stderr, "Nombre de liens: %" PRIu32 "\n", data -> nb_edges);
        for (uint32_t i = 0; i < data -> nb_edges ; ++i) {
            fprintf(stderr,
                    "Lien: %" PRIu32 " -> %" PRIu32 " (cout de %" PRIi32 ")\n",
                    (uint32_t)data -> links[i].from, (uint32_t)data->links[i].to,
                    (int32_t)data->links[i].cost);
        }
    }

    uint32_t buffer32;
    buffer32 = htobe32(data->nb_nodes);
    fwrite(&buffer32, sizeof(uint32_t), 1, args.output_stream);
    if (fwrite(&buffer32, sizeof(uint32_t), 1, args.output_stream) < 1){       //fwrite() should return nitems (here,1) on success and an error otherwise
        fprintf(stderr, "Error writing output file\n");
        exit(EXIT_FAILURE);
    }

    uint32_t count= 0;
    for (uint32_t source = 0; source < data->nb_nodes; ++source) {
        count++;
        int64_t * dist = malloc(sizeof(int64_t) * data->nb_nodes);
        uint32_t * path = malloc(sizeof(uint32_t) * data->nb_nodes);

        if (dist ==NULL){        //vérif des valeurs de retour
            fprintf(stderr, "Erreur d'allocation mémoire\n");
            exit(EXIT_FAILURE);
        }
        if (path ==NULL){
            fprintf(stderr, "Erreur d'allocation mémoire\n");
            exit(EXIT_FAILURE);
        }

        bellman_ford(data->links,data->nb_nodes, data->nb_edges, source, dist,path, args.verbose);
        Node* n;

        uint64_t buffer64;
        if(args.output_stream != stdout){
            buffer32 = htobe32(source);
            fwrite(&(buffer32), sizeof(uint32_t), 1, args.output_stream);
            n = get_max(dist, source,data->nb_nodes);

            //write with be32toh
            buffer32 = htobe32(n->max_node);
            fwrite(&buffer32, sizeof(uint32_t), 1, args.output_stream);
            buffer64 = htobe64(n->max_cost);
            fwrite(&buffer64, sizeof(uint64_t), 1, args.output_stream);
            uint32_t r_size = 0;
            uint32_t * r = get_path(n->max_node, path, source, &r_size);
            buffer32 = htobe32(r_size);
            fwrite(&buffer32, sizeof(uint32_t), 1, args.output_stream);
            for(uint32_t j = 0; j < r_size; j++){
                buffer32 = htobe32(r[j]);
                fwrite(&buffer32, sizeof(uint32_t), 1, args.output_stream);
            }

        }else{
            printf("Source : %" PRIu32 "\n", source);
            n = get_max(dist, source,data->nb_nodes);
            printf("Destination : %" PRIu32 "\n", n->max_node);
            printf("Coût : %" PRIi64 "\n", n->max_cost);
            uint32_t r_size = 0;
            uint32_t * r = get_path(n->max_node, path, source, &r_size);
            for(uint32_t j = 0; j < r_size; j++){
                printf("%" PRIu32 " ", r[j]);
            }printf("\n");
            printf("Nombre de noeuds : %" PRIu32 "\n", r_size);
            printf("\n");
            free(r);
            free(n);
        }
        free(dist);
        free(path);
    }printf("=> Nombre total de noeuds : %" PRIu32 "\n", count);

    // END

    if (args.input_file != NULL) {
        fclose(args.input_file);
    }
    if (args.input_file == NULL){                    //vérif de la valeur
        printf("Impossible to close the output file\n");
        return -1;
    }

    if (args.output_stream != stdout) {
        fclose(args.output_stream);
    }
    if (args.output_stream == NULL){                //vérif de la valeur
        printf("Impossible to close the output file\n");
        return -1;
    }

    free_graph(data);
    return 0;
}

