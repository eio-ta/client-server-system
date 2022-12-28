#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>


// GESTION DES FICHIERS OU DE DOSSIERS //

/* Crée récursivement des répertoires avec le chemin en paramètre */
void recursive_mkdir(const char *path);

/* Compte le nombre de répertoires dans un dossier */
int count_directory(char *path);

/* Compte le nombre de tâches existantes (non supprimées) dans un dossier */
uint32_t count_tasks_not_deleted(char *path);

/* Crée et écris un contenu dans un fichier avec le chemin en paramètre */
int create_file_task(char *path, char *contents);

/* Récupère ce qui est écrit dans un fichier
(notamment pour récupérer le timing, ou les arguments d'une commande) */
char *read_file(char *path, char *contents);

/* Vérifie si un chemin de répertoire existe
Renvoie 0 en cas de succès, sinon -1 */
int file_exists(char *filename);


// CONCATÉNATION ET ECRITURE //

/* Concatène deux chaines de caractères */
char *concat(const char *str1, const char *str2);

/* Ecriture dans un buffer */
void bufcat(char *buf, size_t *buf_size, void *val, size_t size);

/* Compte le nombre d'éléments dans un tableau de char * */
uint32_t count_arguments(char *argv);

#endif // CASSINI
