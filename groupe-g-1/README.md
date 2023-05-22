# Projet 3 : groupe G1


## Introduction 

Le but de notre  programme `main` est d'implémenter en C l'algorithme de bellman-ford afin de trouver le plus long plus court chemin depuis chaque noeud dans un graphe orienté et pondéré pouvant contenir des poids négatifs.


## Prérequis

La libraire CUNIT doit être installée pour pouvoir lancer les tests de notre programme. 

Système d'exploitation : Le projet a été développé et testé sur Linux et MacOS.



## Exécution du programme `main`

 Le `MakeFile` contient les commandes suivantes : 
- `make` ou `make main` : crée l'exécutable main (compilation des fonctions du dossier src et de main.c )
- `make tests` : crée le fichier exécutable, **lance les tests unitaires** implémentés dans Tests.c et lance la commande make clean 
- `make clean` : supprime les fichiers temporaires créés après compilation de main.c 
- `make timer`: compile les fichiers main.c et timer.c 

### Exécution du programme principal (main.c)
Pour lancer notre programme, assurez vous de l'avoir compilé via la commande `make`, puis entrez la commande :
`./main [-v] [-n nb-threads] [-f "output-file-name"] "input-file.bin"`

- Argument obligatoire :
     - "input-file.bin" : fichier binaire d'entrée comprenant une description du graphe sur lequel on souhaite utiliser l'algorithme de Bellman-Ford

- Arguments optionnels :
     -  -v : active le mode verbose, affichage de message de debug sur la console de sortie standard (stdout) ou d’erreur (stderr)

     -  -n nb-threads : nombre de threads de calcul 

     - -f "output-file-name" : fichier binaire de sortie qui comportera les résultats de notre programme (si aucun fichier de sortie n’est spécifié, les résultats sont affichés au format texte sur la console de sortie standard (stdout))

### Exécution des tests de performance 
Pour récupérer les données de temps et de consommation de mémoire sur votre ordinateur ou Raspberry, entrez la commande : `./script.sh`. Les données seront mises à jour dans les fichiers du dossier test_perf. 
Si vous souhaitez uniquement obtenir les temps d'exécution, entrez la commande `.\timer`qui exécutera `timer.c`.




## Spécifications

### Fichier d'entrée (input) :
Notre programme prend en entrée un fichier binaire contenant une description d'un graphe pondéré. Le fichier est écrit en big endian et contient les valeurs suivantes :
- Le nombre de noeuds (nombre entier non signé réprésenté sur 32 bits)
- Le nombre de liens (nombre entier non signé réprésenté sur 32 bits)
- Les liens du graphes comprenant chacun 3 valeurs
    - L’identifiant du nœud de départ (nombre entier non signé réprésenté sur 32 bits)
    - L’identifiant du nœud de destination (nombre entier non signé réprésenté sur 32 bits)
    - Le cout du lien  (nombre entier signé réprésenté sur 32 bits)


### Fichier de sortie (output) :

Le programme renvoie un fichier de sortie binaire qui contient, pour chaque nœud du graphe, les informations sur le
plus court chemin le plus loin depuis ce nœud, s’il existe. L'ensemble est encodé en big endian et a le format suivant :

- le nombre de noeuds (nombre entier non-signé représenté sur 32 bits)
- Pour chaque noeud :
    - L’identifiant du nœud source (entier non-signé représenté sur 32 bits)
    - L’identifiant du nœud destination (entier non-signé représenté sur 32 bits)
    - Le poids/cout du plus court chemin le plus long (entier signé représenté sur 64 bits)
    - Le nombre de nœuds que traverse ce plus court chemin (entier non-signé encodé sur 32 bits)
    - Les nœuds que ce plus court chemin emprunte (entiers non-signés encodés sur 32 bits)


## Structure du projet

Le projet contient les dossiers et fichiers suivants :

**Dossier src**
: Contient les fichiers sources 

  - `graph.c`: reprend toutes le fonctions permettant de résoudre le problème du plus court chemin.
     - la fonction `bellman-ford()` qui implémente l'algorithme de Bellman-Ford pour résoudre le problème du plus court chemin
     - `get_max()` qui trouve le noeud le plus loin de la source et le coût pour arriver à cette destination
     - `get_path()` qui permet de récupérer le chemin emprunté grâce à bellman-Ford


  - `fichier.c` : reprend toutes les fonctions permettant de lire le fichier d'entrée
    - `get_file_infos()` qui récupère les données du fichier binaire contenant le graphe (nombre de noeud et nombre d'arêtes)
    - `read_graph()` qui récupère les données du fichier d'entrée (links) 
    - `free_graph()` qui permet de libérer la mémoire allouée à un graphe pour éviter les fuites de mémoire


  - `execution.c` : reprend toutes les fonctions permettant d'exécuter le programme en multithread 
    - `producer()` qui stocke les résulats de l'algorithme calculés par plusieurs threads en parallèle dans un buffer 
    - `consumer()` qui récupère les données stockées dans le buffer pour les écrire dans un fichier binaire de sortie 
    - `usage()` et `parse_args()` qui traitent les arguments passés en ligne de commande 

**Dossier include**
: Contient les fichiers headers 

  * `graph.h` : 
    - Déclaration des structures utilisées, dont args_t qui contient les données du graphe et arguments passés en ligne de commande
    - Déclaration des fonctions de `graph.c` et `fichier.c` 
  - `main.h` :
    - Initialisation des variables globales
    - Déclaration des fonctions de `execution.c`

  * **portable_endian.h** et **portable_semaphore.h** : fournissent une compatibilté macOS

**Dossier graph**
: Contient des fichiers binaires de différentes tailles pouvant être utilisés en input pour tester l'efficaité du programme  

**Dossier test_perf**
: Met à jour les résultats des tests de performance 

- dans `timer.csv` pour les tests de temps 
- dans `MaxRAMoccupied.txt` pour les tests de consommation de mémoire 
  




`main.c`
  - Récupère les données fournies en entrée 
  - Crée et lance n threads producteurs qui vont travailler en parallèle et stocker les résulats dans un buffer 
  - Crée et lance le consommateur qui récupère les données du buffer 
  - Après avoir importé graph.h,  les producteurs utilisent les fonctions codées dans graph.c et fichier.c et les structures définies dans graph.h pour exécuter l'algorithme de Bellman Ford sur le fichier d'entrée
  - Après avoir importé main.h, le consommateur écrit les données correspondant au plus court chemin calculé dans un fichier de sortie (binaire)
  - Ferme le fichier d'entrée et libère la mémoire allouée 

`Tests.c`

Implémentation de **tests unitaires vérifiant l'efficacité du programme** avec différents graphes en input.
- test_get_file_infos()
- void test_read_graph()
- test_bellman_ford()
- test_bellman_ford_noeud_isole()
- test_get_max()
- test_get_max_cycle_negatif()
- test_get_max_noeud_isole()
- test_get_path()
- test_get_path_noeud_isole()
- test_global()


`timer.c`

**Tests de temps** 

Calcule le temps d'exécution du programme 



`script.sh`

Tests de performance avec un fichier en input contenant un graphe à 1000 noeuds par défaut avec 1, 2, et ainsi de suite jusqu'à 8 threads passés en arguments
- Calcule la quantité de mémoire consommée lors de l'exécution
- compile et lance le fichier timer.c 




` MakeFile`

Raccourcis à passer en ligne de commande pour 
  - Compiler le programme principal (main.c) 
  - Lancer les tests unitaires (Tests.c)
  - Lancer les tests de performance 
  - Nettoyer les fichiers temporaires 
  

 
## Bugs potentiels 

Si vous souhaitez lancer les tests de performances implémentés dans script.sh sur Raspberry pi, il est possible que la fonction max_memory_occupied() dans `script.sh` n'affiche pas la mémoire occupée par le programme. Ceci est peut-être dû à l'encodage des fins de lignes qui est réalisé en CRLF et doit être remis en LF grâce à la commande suivante : sed -e 's/\r$//' fichier_origine > fichier_destination. Si le lancement des tests de consommation de mémoire ne fonctionne toujours pas, il est très probable que la version de l'OS de votre Raspberry soit trop ancienne. Les tests de temps sont quant à eux, sont fiables et ne devraient pas poser de problèmes. 

