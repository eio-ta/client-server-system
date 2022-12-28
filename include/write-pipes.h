#ifndef WRITE_REQUEST_H
#define WRITE_REQUEST_H

#include <stdint.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>

#include "stringBIS.h"
#include "timing.h"
#include "timing-text-io.h"
#include "client-request.h"
#include "server-reply.h"
#include "utils.h"

#define REQUEST_PIPE "/saturnd-request-pipe"


/* Enregistrement des tailles des UINT_T */
#define SIZE_UINT8 1
#define SIZE_UINT16 2
#define SIZE_UINT32 4
#define SIZE_UINT64 8


// FONCTIONS //

/* Écriture dans un tube selon les paramètres à écrire */ 
void write_operation(int fd, uint16_t operation);
void write_operation_with_task_id(int fd, uint16_t operation, u_int64_t task_id);
void write_request_create(int fd, int optc, char *m, char *h, char *d, int argc, char **argv);

/* Écriture dans le tube de réponse */
int write_reply_error_remove(int fd);
int write_reply_list(int fd, char *tasks_dir_path);
int write_reply_ok_stderr_stdout(int fd, char *res_char);
int write_reply_notok_stderr_stdout(int fd, char *task_dir_path, uint64_t taskid);
void write_reply_times_exitcode(int fd, uint16_t taskid, char* task_dir_path);

/* Fonction principale : gestion de l'écriture d'une opération 
   Retourne en cas de succès EXIT_SUCCESS. En cas d'échec, EXIT_FAILLURE.
*/
int write_request_pipe(int fd, char *request_pipe_name, uint16_t operation, int optc, char *minutes_str, char *hours_str, char *daysofweek_str, int argc, char *argv[], uint64_t taskid);

#endif // WRITE_REQUEST_H
