#include "../include/cassini.h"

const char usage_info[] = "\
   usage: cassini [OPTIONS] -l -> list all tasks\n\
      or: cassini [OPTIONS]    -> same\n\
      or: cassini [OPTIONS] -q -> terminate the daemon\n\
      or: cassini [OPTIONS] -c [-m MINUTES] [-H HOURS] [-d DAYSOFWEEK] COMMAND_NAME [ARG_1] ... [ARG_N]\n\
          -> add a new task and print its TASKID\n\
             format & semantics of the \"timing\" fields defined here:\n\
             https://pubs.opengroup.org/onlinepubs/9699919799/utilities/crontab.html\n\
             default value for each field is \"*\"\n\
      or: cassini [OPTIONS] -r TASKID -> remove a task\n\
      or: cassini [OPTIONS] -x TASKID -> get info (time + exit code) on all the past runs of a task\n\
      or: cassini [OPTIONS] -o TASKID -> get the standard output of the last run of a task\n\
      or: cassini [OPTIONS] -e TASKID -> get the standard error\n\
      or: cassini -h -> display this message\n\
\n\
   options:\n\
     -p PIPES_DIR -> look for the pipes in PIPES_DIR (default: /tmp/<USERNAME>/saturnd/pipes/)\n\
";

int main(int argc, char *argv[]) {
	errno = 0;

	// Champs pour avoir les minutes, les heures et les jours de la semaine
	char *minutes_str = "*";
	char *hours_str = "*";
	char *daysofweek_str = "*";

	// Champ pour récupérer le nom du propriétaire
	char *name;
	struct passwd *pass;
	pass = getpwuid(getuid());
	name = pass->pw_name;

	// Champs pour avoir les répertoires des tubes
	char *pipes_directory = NULL;
	int pipe_directory_change = 0;

	// Champs pour avoir les opérations et le nombre d'options
	uint16_t operation = 0;
	uint64_t taskid = -1;
	int opt = 0;
	int optc = 0;

	char *strtoull_endp;
	while((opt = getopt(argc, argv, "hlcqm:H:d:p:r:x:o:e:")) != -1) {
		optc++;
		switch (opt) {
			case 'm': minutes_str = optarg; break;
			case 'H': hours_str = optarg; break;
			case 'd': daysofweek_str = optarg; break;
			case 'p': pipes_directory = strdup(optarg); break;
			case 'l': operation = CLIENT_REQUEST_LIST_TASKS; break;
			case 'c': operation = CLIENT_REQUEST_CREATE_TASK; break;
			case 'q': operation = CLIENT_REQUEST_TERMINATE; break;
			case 'r':
				operation = CLIENT_REQUEST_REMOVE_TASK;
				taskid = strtoull(optarg, &strtoull_endp, 10);
				if(strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
				break;
			case 'x':
				operation = CLIENT_REQUEST_GET_TIMES_AND_EXITCODES;
				taskid = strtoull(optarg, &strtoull_endp, 10);
				if(strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
				break;
			case 'o':
				operation = CLIENT_REQUEST_GET_STDOUT;
				taskid = strtoull(optarg, &strtoull_endp, 10);
				if(strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
				break;
			case 'e':
				operation = CLIENT_REQUEST_GET_STDERR;
				taskid = strtoull(optarg, &strtoull_endp, 10);
				if(strtoull_endp == optarg || strtoull_endp[0] != '\0') goto error;
				break;
			case 'h': printf("%s", usage_info); return 0;
			case '?': fprintf(stderr, "%s", usage_info); goto error;
		}
	}

	// VERIFICATION DE L'EMPLACEMENT DES TUBES ET LIBERATION DE MEMOIRE
	if(pipes_directory == NULL) {
		char *tmp = concat("/tmp/", name);
		pipes_directory = concat(tmp, "/saturnd/pipes");
		free(tmp);
		pipe_directory_change = 1;
	}

	char *request_pipe_name = concat(pipes_directory, REQUEST_PIPE);
	char *reply_pipe_name = concat(pipes_directory, REPLY_PIPE);
	
	if(pipe_directory_change) free(pipes_directory);

	if(file_exists(request_pipe_name) != 0 || file_exists(reply_pipe_name) != 0) goto error;

	// ECRITURE DANS LE TUBE DE REQUETE
	int fd = -1;
	int res_write = write_request_pipe(fd, request_pipe_name, operation, optc, minutes_str, hours_str, daysofweek_str, argc, argv, taskid);

	if(res_write == EXIT_FAILURE) return EXIT_FAILURE;

	close(fd);

	// LECTURE DANS LE TUBE DE REPONSE
	int fw = -1;
	int res_read = read_reply_pipe(fw, reply_pipe_name, operation, taskid);

	if(res_read == EXIT_FAILURE) return EXIT_FAILURE;

	close(fw);

	// LIBERATION DE MEMOIRE
	free(request_pipe_name);
	free(reply_pipe_name);

	return EXIT_SUCCESS;

	error:
		if(errno != 0) perror("main");
		free(pipes_directory);
		pipes_directory = NULL;
		return EXIT_FAILURE;
}
