#ifndef SATURND_H
#define SATURN_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pwd.h>

#include "write-pipes.h"
#include "read-pipes.h"

/* Crée le démon
(il faut exécuter deux fois "fork()" pour le créer) */
void forks_demon();

#endif // SATURND_H