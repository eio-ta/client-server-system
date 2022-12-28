#ifndef TIMING_TEXT_IO_H
#define TIMING_TEXT_IO_H

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include "timing.h"

#define TIMING_TEXT_MIN_BUFFERSIZE 1024


// FONCTION PRINCIPALES //

/* Écris une structure timing.
En cas de succès, retourne 0. En cas d'echec, retourne -1. */
int timing_from_strings(struct timing *dest, char *minutes_str, char *hours_str, char *daysofweek_str);

/* Écris un texte répresentant le temps dans un char*, et ajoute le caractère '\0'.
Le buffer doit être capable de contenir au moins TIMING_TEXT_MIN_BUFFERSIZE caractères
Retourne le nombre de caractères écrits, le '\0' non compris. */
int timing_string_from_timing(char *dest, const struct timing *timing);


// FONCTIONS AUXILAIRES //

int timing_field_from_string(uint64_t *dest, const char *string, unsigned int min, unsigned int max);
int timing_range_from_string(uint64_t *dest, const char *string, unsigned int min, unsigned int max);
int timing_uint_from_string(unsigned long int *dest, const char *string);

int timing_string_from_field(char *dest, unsigned int min, unsigned int max, uint64_t field);
int timing_string_from_range(char *dest, unsigned int start, unsigned int stop);

#endif // TIMING_TEXT_IO_H
