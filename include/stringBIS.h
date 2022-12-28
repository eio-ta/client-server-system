#ifndef STRINGBIS_H
#define STRINGBIS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// STRUCTURE //

/* Structure string : deux paramètres qui représentent un mot et son nombre de caractères */
typedef struct string {
	uint32_t len;
	char *word;
} string;


// FONCTIONS //

/* Créer un string */
string* create_string(uint32_t len, char *word);

/* Change les paramètres d'un strin */
void set_string(string *str, uint32_t len, char *word);

/* Affiche le string */
void print_string(string *str);

#endif // STRINGBIS_H
