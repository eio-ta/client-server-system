#ifndef READ_PIPES
#define READ_PIPES

#include <stdint.h>
#include <endian.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>

#include "commandline.h"
#include "timing.h"
#include "timing-text-io.h"
#include "server-reply.h"
#include "client-request.h"

#define REPLY_PIPE "/saturnd-reply-pipe"


/* Enregistrement des tailles des UINT_T */
#define SIZE_UINT8 1
#define SIZE_UINT16 2
#define SIZE_UINT32 4
#define SIZE_UINT64 8


// FONCTIONS

/* Lecture d'un type particulier */
uint16_t read_uint16(int fd, uint16_t buf);
uint32_t read_uint32(int fd, uint32_t nbtask);
uint64_t read_uint64(int fd, uint64_t taskid);
int64_t read_int64(int fd, int64_t taskid);

timing read_timing(int fd, char *timing_str);
commandline* read_commandline(int fd);
string* read_string(int fd);

/* Lecture dans le tube de requête */
int read_request_create(int fd, char *task_name);
int read_request_remove(int fd, char *task_dir_path);
char *read_stdout_stderr(int fd, char *task_dir_path, uint64_t taskid, int request);

/* Lecture dans le tube de réponse */
void read_reply_ls(int fd, uint64_t taskid, char *timing_str);
void read_reply_create(int fd, uint64_t taskid);
int read_reply_time_exitcode(int fd);
int read_reply_remove(int fd);
int read_reply_stdout_stderr(int fd);

/* Fonction principale : gestion de la lecture d'une opération 
   Retourne en cas de succès EXIT_SUCCESS. En cas d'échec, EXIT_FAILLURE.
*/
int read_reply_pipe(int fw, char *reply_pipe_name, uint16_t operation, uint64_t taskid);

#endif // READ_PIPES
