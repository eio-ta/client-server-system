# SYSTÈME D'EXPLOITATION 2021 - 2022

## Sommaire
1. [Introduction et informations](README.md#introduction-et-informations)
2. [Compilation, exécution et arguments possibles](README.md#compilation-execution-et-arguments-possibles)
3. [Application du programme](README.md#application-du-programme)

----------------------------------------------------------------------




## Introduction et informations


**Informations généraux**
- L'énoncé du projet : [enonce.md](https://gaufre.informatique.univ-paris-diderot.fr/aggount/sy5-projet-2021-2022/blob/master/enonce.md)
- Le protocole de l'énoncé : [protocole.md](https://gaufre.informatique.univ-paris-diderot.fr/aggount/sy5-projet-2021-2022/blob/master/protocole.md)
- Membres du groupe : [AUTHORS.md](https://gaufre.informatique.univ-paris-diderot.fr/aggount/sy5-projet-2021-2022/blob/master/AUTHORS.md)
- Architecture du projet : [ARCHITECTURE.md](https://gaufre.informatique.univ-paris-diderot.fr/aggount/sy5-projet-2021-2022/blob/master/ARCHITECTURE.md)




## Compilation execution et arguments possibles


**Compilation et éxécution**

Ouvrez un terminal et suivez les commandes indiquées.

**Compilation et exécution avec le Makefile**

- `make` sans argument lancera la compilation des fichiers.
- `./saturnd` va lancer un serveur en fond.
- `./cassini arg1 (arg2 ... argn)` va exécuter les commandes souhaitées avec les arguments utilisés.
- `make distclean` effacera les fichiers produits (plus précisément les exécutables) par la compilation.


**Les arguments possibles lors de l'exécution**

**Pour les arguments :**

1. `-l` : Liste toutes les tâches.
2. `-c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] [ARG_n]` : Crée une nouvelle tâche.
3. `-r TASKID` : Supprime une tâche.
4. `-x TASKID` : Liste l'heure d'exécution et la valeur de retour de toutes les exécutions précédentes de la tâche.
5. `-o TASKID` : Affiche la sortie standard de la dernière exécution de la tâche.
6. `-e TASKID` : Affiche la sortie erreur standard de la dernière exécution de la tâche.
7. `-q` : Termine le démon.




## Application du programme

```
USER@MACHINE:~/sy5-projet-2021-2022$ ./saturnd
USER@MACHINE:~/sy5-projet-2021-2022$ ./cassini -c echo test
0
USER@MACHINE:~/sy5-projet-2021-2022$ ./cassini -c -m 2 echo test-1
1
USER@MACHINE:~/sy5-projet-2021-2022$ ./cassini -c -m 2 -H 20 echo test-2
2
USER@MACHINE:~/sy5-projet-2021-2022$ ./cassini -l
0: * * * echo test
1: 2 * * echo test-1
2: 2 20 * echo test-2
USER@MACHINE:~/sy5-projet-2021-2022$ ./cassini -r 3
il n'existe aucune tâche avec cet identifiant
USER@MACHINE:~/sy5-projet-2021-2022$ ./cassini -r 2
elody@Nvidia:~/sy5-projet-2021-2022$ ./cassini -l
0: * * * echo test
1: 2 * * echo test-1
```


----------------------------------------------------------------------