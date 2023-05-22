
//-------------------------------------------------------- includes --------------------------------------------------------

#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "include/graph.h"

//-------------------------------------------------------- tests get_file_infos --------------------------------------------------------

void test_get_file_infos()
{
    args_t *args = (args_t *)malloc(sizeof(args_t));
    args->input_file = fopen("graph/graph.bin", "r");
    get_file_infos(args);

    // Vérifie le nb_nodes et nb_edges
    CU_ASSERT_EQUAL(args->nb_nodes, 5);
    CU_ASSERT_EQUAL(args->nb_edges, 10);

    fclose(args->input_file);
    free_graph(args);
}

//-------------------------------------------------------- tests read_graph --------------------------------------------------------

void test_read_graph()
{
    args_t *args = (args_t *)malloc(sizeof(args_t));
    args->input_file = fopen("graph/graph.bin", "r");
    read_graph(args);

    // Vérifie les links
    CU_ASSERT_EQUAL(args->links[0].from, 0);
    CU_ASSERT_EQUAL(args->links[0].to, 2);
    CU_ASSERT_EQUAL(args->links[0].cost, 4);
    CU_ASSERT_EQUAL(args->links[1].from, 0);
    CU_ASSERT_EQUAL(args->links[1].to, 4);
    CU_ASSERT_EQUAL(args->links[1].cost, 7);
    CU_ASSERT_EQUAL(args->links[2].from, 0);
    CU_ASSERT_EQUAL(args->links[2].to, 1);
    CU_ASSERT_EQUAL(args->links[2].cost, 4);
    CU_ASSERT_EQUAL(args->links[3].from, 2);
    CU_ASSERT_EQUAL(args->links[3].to, 0);
    CU_ASSERT_EQUAL(args->links[3].cost, 3);
    CU_ASSERT_EQUAL(args->links[4].from, 1);
    CU_ASSERT_EQUAL(args->links[4].to, 0);
    CU_ASSERT_EQUAL(args->links[4].cost, 9);
    CU_ASSERT_EQUAL(args->links[5].from, 1);
    CU_ASSERT_EQUAL(args->links[5].to, 4);
    CU_ASSERT_EQUAL(args->links[5].cost, 7);
    CU_ASSERT_EQUAL(args->links[6].from, 1);
    CU_ASSERT_EQUAL(args->links[6].to, 3);
    CU_ASSERT_EQUAL(args->links[6].cost, 10);
    CU_ASSERT_EQUAL(args->links[7].from, 1);
    CU_ASSERT_EQUAL(args->links[7].to, 2);
    CU_ASSERT_EQUAL(args->links[7].cost, 2);
    CU_ASSERT_EQUAL(args->links[8].from, 4);
    CU_ASSERT_EQUAL(args->links[8].to, 0);
    CU_ASSERT_EQUAL(args->links[8].cost, 9);
    CU_ASSERT_EQUAL(args->links[9].from, 3);
    CU_ASSERT_EQUAL(args->links[9].to, 2);
    CU_ASSERT_EQUAL(args->links[9].cost, 5);

    fclose(args->input_file);
    free_graph(args);

    // Vérifie les links avec coût négatif
    args->input_file = fopen("graph/graphe1.bin", "r");
    read_graph(args);

    CU_ASSERT_EQUAL(args->links[0].from, 0);
    CU_ASSERT_EQUAL(args->links[0].to, 1);
    CU_ASSERT_EQUAL(args->links[0].cost, -1);
    CU_ASSERT_EQUAL(args->links[1].from, 0);
    CU_ASSERT_EQUAL(args->links[1].to, 2);
    CU_ASSERT_EQUAL(args->links[1].cost, -1);

    fclose(args->input_file);
    free_graph(args);
}

//-------------------------------------------------------- tests bellman_ford --------------------------------------------------------

void test_bellman_ford()
{
    uint32_t nb_nodes = 5;
    uint32_t nb_edges = 8;
    uint32_t source = 0;
    int64_t *dist = calloc(nb_nodes, sizeof(int64_t));
    uint32_t *prev = calloc(nb_nodes, sizeof(uint32_t));

    link_t *links = (link_t *)malloc(nb_edges * sizeof(link_t));
    links[0].from = 0;
    links[0].to = 1;
    links[0].cost = 6;
    links[1].from = 0;
    links[1].to = 2;
    links[1].cost = 7;
    links[2].from = 1;
    links[2].to = 3;
    links[2].cost = 5;
    links[3].from = 1;
    links[3].to = 2;
    links[3].cost = 8;
    links[4].from = 1;
    links[4].to = 4;
    links[4].cost = -4;
    links[5].from = 2;
    links[5].to = 3;
    links[5].cost = -3;
    links[6].from = 2;
    links[6].to = 4;
    links[6].cost = 9;
    links[7].from = 3;
    links[7].to = 1;
    links[7].cost = -2;

    // Test du fonctionnement de l'algorithme
    bellman_ford(links, nb_nodes, nb_edges, source, dist, prev, 1);
    CU_ASSERT_EQUAL(dist[0], 0);
    CU_ASSERT_EQUAL(dist[1], 2);
    CU_ASSERT_EQUAL(dist[2], 7);
    CU_ASSERT_EQUAL(dist[3], 4);
    CU_ASSERT_EQUAL(dist[4], -2);
    CU_ASSERT_EQUAL(prev[0], -1);
    CU_ASSERT_EQUAL(prev[1], 3);
    CU_ASSERT_EQUAL(prev[2], 0);
    CU_ASSERT_EQUAL(prev[3], 2);
    CU_ASSERT_EQUAL(prev[4], 1);

    // Test pour un cycle négatif
    links[2].cost = -10;
    bellman_ford(links, nb_nodes, nb_edges, source, dist, prev, 0);
    CU_ASSERT_EQUAL(dist[0], 0);
    CU_ASSERT_EQUAL(dist[1], INT64_MIN);
    CU_ASSERT_EQUAL(dist[2], INT64_MIN);
    CU_ASSERT_EQUAL(dist[3], INT64_MIN);
    CU_ASSERT_EQUAL(dist[4], INT64_MIN);
    CU_ASSERT_EQUAL(prev[0], -1);
    CU_ASSERT_EQUAL(prev[1], -1);
    CU_ASSERT_EQUAL(prev[2], -1);
    CU_ASSERT_EQUAL(prev[3], -1);
    CU_ASSERT_EQUAL(prev[4], -1);

    free(dist);
    free(prev);
    free(links);
}

void test_bellman_ford_noeud_isole()
{
    uint32_t nb_nodes = 4;
    uint32_t nb_edges = 3;
    uint32_t source = 0;
    int64_t *dist = calloc(nb_nodes, sizeof(int64_t));
    uint32_t *prev = calloc(nb_nodes, sizeof(uint32_t));

    link_t *links = (link_t *)malloc(nb_edges * sizeof(link_t));
    links[0].from = 0;
    links[0].to = 1;
    links[0].cost = 1;
    links[1].from = 0;
    links[1].to = 2;
    links[1].cost = 4;
    links[2].from = 1;
    links[2].to = 2;
    links[2].cost = 2;

    bellman_ford(links, nb_nodes, nb_edges, source, dist, prev, 0);
    // Vérifier que la dist vers le noeud isolés est infinie
    CU_ASSERT_EQUAL(dist[0], 0);
    CU_ASSERT_EQUAL(dist[1], 1);
    CU_ASSERT_EQUAL(dist[2], 3);
    CU_ASSERT_EQUAL(dist[3], INT64_MAX);
    CU_ASSERT_EQUAL(prev[0], -1);
    CU_ASSERT_EQUAL(prev[1], 0);
    CU_ASSERT_EQUAL(prev[2], 1);
    CU_ASSERT_EQUAL(prev[3], -1);

    source = 3;
    bellman_ford(links, nb_nodes, nb_edges, source, dist, prev, 0);
    // Vérifier que depuis un noeud isolé toutes les distances sont infinie sauf pour la source
    CU_ASSERT_EQUAL(dist[0], INT64_MAX);
    CU_ASSERT_EQUAL(dist[1], INT64_MAX);
    CU_ASSERT_EQUAL(dist[2], INT64_MAX);
    CU_ASSERT_EQUAL(dist[3], 0);
    CU_ASSERT_EQUAL(prev[0], -1);
    CU_ASSERT_EQUAL(prev[1], -1);
    CU_ASSERT_EQUAL(prev[2], -1);
    CU_ASSERT_EQUAL(prev[3], -1);

    free(dist);
    free(prev);
    free(links);
}

//-------------------------------------------------------- tests get_max --------------------------------------------------------

void test_get_max()
{

    // coût max à partir du noeud source 0
    int64_t dist1[5] = {0, 4, 4, 14, 7};
    int32_t nb_nodes1 = sizeof(dist1) / sizeof(int64_t);
    output_t res1 = get_max(dist1, 0, nb_nodes1);
    CU_ASSERT_EQUAL(res1.max_cost, 14);
    CU_ASSERT_EQUAL(res1.max_node, 3);

    // coût max à partir du noeud source 1
    int64_t dist2[5] = {5, 0, 2, 10, 7};
    uint32_t nb_nodes2 = sizeof(dist2) / sizeof(int64_t);
    output_t res2 = get_max(dist2, 1, nb_nodes2);
    CU_ASSERT_EQUAL(res2.max_cost, 10);
    CU_ASSERT_EQUAL(res2.max_node, 3);

    // coût max à partir du noeud source 2
    int64_t dist3[5] = {3, 7, 0, 17, 10};
    uint32_t nb_nodes3 = sizeof(dist3) / sizeof(int64_t);
    output_t res3 = get_max(dist3, 2, nb_nodes3);
    CU_ASSERT_EQUAL(res3.max_cost, 17);
    CU_ASSERT_EQUAL(res3.max_node, 3);

    // coût max à partir du noeud 0 dont les coûts valent -1
    int64_t dist4[5] = {0, -1, -1, -1, -1};
    uint32_t nb_nodes4 = sizeof(dist4) / sizeof(int64_t);
    output_t res4 = get_max(dist4, 0, nb_nodes4);
    CU_ASSERT_EQUAL(res4.max_cost, -1);
    CU_ASSERT_EQUAL(res4.max_node, 4);

    // coût max à partir du noeud 0 dont les coûts valent 1
    int64_t dist5[5] = {0, 1, 1, 1, 1};
    uint32_t nb_nodes5 = sizeof(dist5) / sizeof(int64_t);
    output_t res5 = get_max(dist5, 0, nb_nodes5);
    CU_ASSERT_EQUAL(res5.max_cost, 1);
    CU_ASSERT_EQUAL(res5.max_node, 4);
}

void test_get_max_cycle_negatif()
{

    // coût max à partir d'un cycle négatif
    int64_t dist1[4] = {INT64_MIN, INT64_MIN, 0, INT64_MIN};
    uint32_t nb_nodes1 = sizeof(dist1) / sizeof(int64_t);
    output_t res1 = get_max(dist1, 2, nb_nodes1);
    CU_ASSERT_EQUAL(res1.max_cost, 0);
    CU_ASSERT_EQUAL(res1.max_node, 2);
}

void test_get_max_noeud_isole()
{

    // coût max à partir du noeud source 0 avec noeud 3 isolé
    int64_t dist1[4] = {0, 1, 3, INT64_MAX};
    uint32_t nb_nodes1 = sizeof(dist1) / sizeof(int64_t);
    output_t res1 = get_max(dist1, 0, nb_nodes1);
    CU_ASSERT_EQUAL(res1.max_cost, 3);
    CU_ASSERT_EQUAL(res1.max_node, 2);

    // coût max à partir du noeud source 3 isolé
    int64_t dist2[4] = {INT64_MAX, INT64_MAX, INT64_MAX, 0};
    uint32_t nb_nodes2 = sizeof(dist2) / sizeof(int64_t);
    output_t res2 = get_max(dist2, 3, nb_nodes2);
    CU_ASSERT_EQUAL(res2.max_cost, 0);
    CU_ASSERT_EQUAL(res2.max_node, 3);
}

//-------------------------------------------------------- tests get_path --------------------------------------------------------

void test_get_path()
{
    uint32_t dest = 2;
    uint32_t source = 0;
    uint32_t prev[] = {-1, 0, 1, 2, 0};
    uint32_t path_size = 0;
    uint32_t *path = get_path(dest, prev, source, &path_size);

    CU_ASSERT_EQUAL(path_size, 3); // vérifie la taille du tableau retourné
    CU_ASSERT_EQUAL(path[0], source);
    CU_ASSERT_EQUAL(path[1], 1);
    CU_ASSERT_EQUAL(path[2], dest);

    free(path);
}

void test_get_path_noeud_isole()
{
    uint32_t dest = 3;
    uint32_t source = 3;
    uint32_t prev[] = {-1, -1, -1, -1}; // n'arrive que si le noeud source est isolé ou si il y a un cycle négatif
    uint32_t path_size = 0;
    uint32_t *path = get_path(dest, prev, source, &path_size);

    CU_ASSERT_EQUAL(path_size, 1); // vérifie la taille du tableau retourné
    CU_ASSERT_EQUAL(path[0], source);

    free(path);
}


//-------------------------------------------------------- test global --------------------------------------------------------

// Initialisation des fichiers nécessaires
const char* python_file = "sp_modif.py"; //sp.py modifié afin de renvoyer les résultats du programme Python dans un fichier sp_output
const char* graph_directory = "graph";
const char* graph_file = "graph.bin";

// Compare les résultats du programme Python avec le programme C
// Retourne true si les résultats sont identiques, sinon false
bool compare_results(int32_t source, const int64_t dist_python[], const uint32_t prev_python[], const int64_t dist_c[], const uint32_t prev_c[], uint32_t nb_nodes) {
    for (uint32_t i = 0; i < nb_nodes; i++) {
        if (dist_python[i] != dist_c[i] || prev_python[i] != prev_c[i]) {
            printf("Erreur : les résultats diffèrent pour le noeud %d\n", i);
            return false;
        }
    }
    return true;
}

// Exécute le programme Python et le programme C et stocke les résultats

void run_program(const char* python_file, const char* graph_directory, const char* graph_file, int32_t source, int64_t dist_python[], uint32_t prev_python[], int64_t dist_c[], uint32_t prev_c[], uint32_t nb_nodes) {

    // Exécute le programme Python
    char python_command[100];
    snprintf(python_command, sizeof(python_command), "python3 %s %s/%s > /dev/null", python_file, graph_directory, graph_file);
    int python_status = system(python_command);
    if (python_status != 0) {
        fprintf(stderr, "Erreur lors de l'exécution du programme Python\n");
        exit(EXIT_FAILURE);
    }

    // Lire les résultats à partir du fichier de sortie
    FILE* python_output = fopen("sp_output.bin", "rb");
    if (python_output == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie Python\n");
        exit(EXIT_FAILURE);
    }
    fread(dist_python, sizeof(int64_t), nb_nodes, python_output);
    fread(prev_python, sizeof(uint32_t), nb_nodes, python_output);
    fclose(python_output);

    // Exécute le programme C
    char c_command[100];
    snprintf(c_command, sizeof(c_command), "./main %s/%s > /dev/null", graph_directory, graph_file);
    int c_status = system(c_command);
    if (c_status != 0) {
        fprintf(stderr, "Erreur lors de l'exécution du programme C\n");
        exit(EXIT_FAILURE);
    }

    // Lire les résultats à partir du fichier de sortie
    FILE* c_output = fopen("sp_output.bin", "rb");
    if (c_output == NULL) {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier de sortie du programme C\n");
        exit(EXIT_FAILURE);
    }
    fread(dist_c, sizeof(int64_t), nb_nodes, c_output);
    fread(prev_c, sizeof(uint32_t), nb_nodes, c_output);
    fclose(c_output);
}

void test_global() {
    int32_t source = 5;
    uint32_t nb_nodes = 4;
    int64_t dist_python[nb_nodes];
    uint32_t prev_python[nb_nodes];
    int64_t dist_c[nb_nodes];
    uint32_t prev_c[nb_nodes];
    run_program(python_file, graph_directory, graph_file, source, dist_python, prev_python, dist_c, prev_c, nb_nodes);
    bool results_match = compare_results(source, dist_python, prev_python, dist_c, prev_c, nb_nodes);
    CU_ASSERT_TRUE(results_match);
}

//-------------------------------------------------------- exécution des tests --------------------------------------------------------

int main()
{
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Suite", NULL, NULL);
    CU_add_test(suite, "test_get_file_infos", test_get_file_infos);
    CU_add_test(suite, "test_read_graph", test_read_graph);
    CU_add_test(suite, "test_bellman_ford", test_bellman_ford);
    CU_add_test(suite, "test_bellman_ford_noeud_isolé", test_bellman_ford_noeud_isole);
    CU_add_test(suite, "test_get_max", test_get_max);
    CU_add_test(suite, "test_get_max_cycle_négatif", test_get_max_cycle_negatif);
    CU_add_test(suite, "test_get_max_noeud_isolé", test_get_max_noeud_isole);
    CU_add_test(suite, "test_get_path", test_get_path);
    CU_add_test(suite, "test_get_path_noeud_isolé_et_cycle_négatif", test_get_path_noeud_isole);
    CU_add_test(suite, "test_global", test_global);
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}
