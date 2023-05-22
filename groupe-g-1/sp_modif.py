import math
import argparse
import sys

"""
    La partie du code Main ci-dessous a été légèrement modifiée par rapport au fichier sp.py original. 
    L'objectif est de pouvoir enregistrer les résultats dans un fichier sp_output.bin, 
    ce qui permettra de les comparer avec les résultats de notre fonction Bellman-Ford en C 
    lors du test unitaire test_global de notre suite de tests.
"""


def get_file_infos(data):
    """
    Récupère les informations basiques du graphe : le nombre de noeuds et de liens.
    """
    nb_nodes = int.from_bytes(data[0:4], "big", signed=True)
    nb_edges = int.from_bytes(data[4:], "big", signed=True)
    return nb_nodes, nb_edges


def bellman_ford(links, s, verbose):
    """
    Exécute l'algorithme de Bellman-Ford avec comme noeud source `s`.
    Retourne un tableau de distances pour atteindre chaque noeud depuis `s`,
    ainsi que le chemin pour atteindre ces noeuds (tableau de précédence).
    Si le noeud est isolé ou qu'un cycle négatif est trouvé, retourne une distance
    infinie pour chaque autre noeud, et une distance de 0 pour `s`.
    """

    # Initialisation.
    dist = [math.inf] * nb_nodes
    dist[s] = 0  # Le noeud source est à distance 0 de lui-meme.
    path = [-1] * nb_nodes
    
    # Iteration de Bellman-Ford.
    for _ in range(nb_nodes-1):
        for j in range(len(links)):
            node_from, node_to, cost = links[j][0], links[j][1], links[j][2]
            if (dist[node_from] != math.inf and dist[node_to] > dist[node_from] + cost):
                dist[node_to] = dist[node_from] + cost
                path[node_to] = node_from
    
    # Detection de cycle negatif.
    for j in range(len(links)):
        node_from, node_to, cost = links[j][0], links[j][1], links[j][2]
        if (dist[node_from] != math.inf and dist[node_to] > dist[node_from] + cost):
            if verbose:
                print("Cycle négatif détecté")
            dist = [math.inf] * nb_nodes
            dist[s] = 0
            path = [-1] * nb_nodes
    return dist, path


def get_path(dest, path, source):
    """
    Retourne une liste contenant le chemin de `source` vers `dest`
    en utilisant le tableau de précédence `path`.
    """
    r = [dest]
    i = dest
    while i != source:
        r.insert(0, path[i])
        i = path[i]
    return r


def get_max(dist, s):
    """
    Retourne l'indice du noeud dont il existe un chemin de `s` vers ce noeud
    et le cout de ce chemin est le plus élevé parmis tous les noeuds ayant un chemin
    depuis `s`.
    """
    max_cost = -math.inf
    max_node = s
    for node_idx in range(len(dist)):
        if node_idx != s and dist[node_idx] != math.inf and dist[node_idx] >= max_cost:
            max_cost = dist[node_idx]
            max_node = node_idx
    if max_cost == -math.inf:
        if dist[s] != math.inf and dist[s] >= max_cost:
            max_cost = dist[s]

    return max_cost, max_node


def read_graph(filename):
    """
    Récupère le graphe représenté dans le fichier donné en argument.
    Suit le format défini dans l'énoncé.
    """
    with open(filename, "rb") as fd:
        binary_data = fd.read()
        nb_nodes, nb_edges = get_file_infos(binary_data[:8])

        if verbose:
            print("Nombre de noeuds :", nb_nodes,
                  ", nombre de liens :", nb_edges)

        binary_data = binary_data[8:]
        links = []

        for i in range(nb_edges):
            from_node = int.from_bytes(
                binary_data[i * 12:i * 12 + 4], "big", signed=True)
            to_node = int.from_bytes(
                binary_data[i * 12 + 4:i * 12 + 8], "big", signed=True)
            cost = int.from_bytes(
                binary_data[i * 12 + 8:i * 12 + 12], "big", signed=True)
            l1 = [from_node, to_node, cost]
            links.append(l1)

    return links, nb_nodes

# Partie du code modifié
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="LEPL1503 - Algorithme de plus court chemin")
    parser.add_argument("graph_file", help="chemin vers le fichier du graphe à traiter.")
    parser.add_argument("-v", help="autorise les messages de debug. Si ce n'est pas activé, aucun message de ce type ne peut être affiché, excepté les messages d'erreur en cas d'échec. Par défaut : False.", action="store_true")
    args = parser.parse_args()

    verbose = args.v

    graph_file = args.graph_file
    graph, nb_nodes = read_graph(graph_file)

    with open("sp_output.bin", "wb") as output_file:
        # Écriture du nombre de nœuds dans le fichier de sortie.
        output_file.write(nb_nodes.to_bytes(4, "big"))

        for source in range(nb_nodes):
            # Calcul des distances et du chemin le plus court depuis chaque source.
            dist, path = bellman_ford(graph, source, verbose)

            if verbose:
                # Affichage des informations si le mode verbose est activé.
                print("source : " + str(source))
                print("Distances: ", dist)
                d, n = get_max(dist, source)
                print("\tdestination : " + str(n))
                print("\tcoût : " + str(d))
                p = get_path(n, path, source)
                print("\tnombre de noeuds : " + str(len(p)))
                print("\tchemin : " + " ".join(str(x) for x in p))

            # Écriture des résultats dans le fichier de sortie.
            output_file.write(source.to_bytes(4, "big"))
            d, n = get_max(dist, source)
            output_file.write(n.to_bytes(4, "big"))
            output_file.write(d.to_bytes(8, "big", signed=True))
            r = get_path(n, path, source)
            output_file.write(len(r).to_bytes(4, "big", signed=True))
            for j in range(len(r)):
                output_file.write(r[j].to_bytes(4, "big"))
