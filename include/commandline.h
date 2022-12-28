#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stringBIS.h"
#include "write-pipes.h"


// STRUCTURE //

/* Structure commandline : deux paramêtres qui représentent le nombre d'arguments et le tableau des arguments */
typedef struct commandline {
	uint32_t argc;
	string *argv;
} commandline;


// FONCTIONS //

/* Crée une commandline */
commandline create_commandline(uint32_t argc, string *argv);

/* Affiche une commandline sur le terminal */
void print_commandline(commandline *cm);

/* Convertie les arguments d'une commandline en tableau de char * */
char* cml_to_char(commandline *cm);

#endif // COMMANDLINE_H