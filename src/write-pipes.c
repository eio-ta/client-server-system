#include "../include/write-pipes.h"

/* ECRITURE DE VARIABLE */
void write_operation(int fd, uint16_t operation) {
	operation = htobe16(operation);
	if(write(fd, &operation, SIZE_UINT16) < SIZE_UINT16) {
		perror("write_request()");
		close(fd); 
		exit(EXIT_FAILURE);
	}
}

void write_operation_with_task_id(int fd, uint16_t operation, uint64_t task_id) {
	char buf[PIPE_BUF];
	size_t buf_size = 0;
	operation = htobe16(operation);
	bufcat(buf, &buf_size, &operation, SIZE_UINT16);
	task_id = htobe64(task_id);
	bufcat(buf, &buf_size, &task_id, SIZE_UINT64);
	if(write(fd, &buf, buf_size) < buf_size) {
		perror("write_request_with_task_id()");
		close(fd); 
		exit(EXIT_FAILURE);
	}
}

/* ECRITURE DE REQUETE PAR COMMANDE  */
void write_request_create(int fd, int optc, char *m, char *h, char *d, int argc, char **argv) {
	struct timing *tms = malloc(sizeof(struct timing));
	if(tms == NULL) {
		perror("write_request_create() timing");
		close(fd); 
		exit(EXIT_FAILURE);
	}
	if(timing_from_strings(tms, m, h, d) == 0) {
		char buf[PIPE_BUF];
		size_t buf_size = 0;
		uint16_t op = htobe16(CLIENT_REQUEST_CREATE_TASK);
		bufcat(buf, &buf_size, &op, SIZE_UINT16);
		char *cmd_name = malloc(sizeof(char) * (strlen(argv[2 * optc]) + 1));
		if(cmd_name == NULL) {
			perror("write_request_create() com");
			close(fd); 
			exit(EXIT_FAILURE);
		}
		strcpy(cmd_name, argv[2 * optc]);
		uint64_t m = htobe64(tms->minutes); uint64_t h = htobe32(tms->hours);
		bufcat(buf, &buf_size, &m, SIZE_UINT64);
		bufcat(buf, &buf_size, &h, SIZE_UINT32);
		bufcat(buf, &buf_size, &tms->daysofweek, SIZE_UINT8);
		uint32_t cmd_args_ct = htobe32(argc - (2 * optc));
		uint32_t cmd_name_l = htobe32(strlen(argv[2 * optc]));
		bufcat(buf, &buf_size, &cmd_args_ct, SIZE_UINT32);
		bufcat(buf, &buf_size, &cmd_name_l, SIZE_UINT32);
		bufcat(buf, &buf_size, cmd_name, strlen(cmd_name));
		for (unsigned int i = 2 * optc + 1; i < argc; i++) {
			uint32_t curr_arg_l = htobe32(strlen(argv[i]));
			bufcat(buf, &buf_size, &curr_arg_l, SIZE_UINT32);
			char *cmd_curr_arg;
			cmd_curr_arg = malloc(strlen(argv[i]) + 1);
			if(cmd_curr_arg == NULL){
				perror("cmd_curr_arg malloc write_request_create");
				close(fd); 
				exit(EXIT_FAILURE);
			}
			strcpy(cmd_curr_arg, argv[i]);
			bufcat(buf, &buf_size, cmd_curr_arg, strlen(cmd_curr_arg));
			free(cmd_curr_arg);
		}
		if(write(fd, buf, buf_size) < buf_size) {
			perror("write_request_create() write");
			close(fd); 
			exit(EXIT_FAILURE);
		}
		free(cmd_name);
		free(tms);
	}
}

int write_request_pipe(int fd, char *request_pipe_name, uint16_t operation, int optc, char *minutes_str, char *hours_str, char *daysofweek_str, int argc, char *argv[], uint64_t taskid) {
	if((fd = open(request_pipe_name, O_WRONLY)) == -1){
		perror("open write_request_pipe");
		close(fd); 
		return EXIT_FAILURE;
	}
	switch (operation) {
		case CLIENT_REQUEST_LIST_TASKS :
		case CLIENT_REQUEST_TERMINATE : write_operation(fd, operation); break;
		case CLIENT_REQUEST_CREATE_TASK :
			write_request_create(fd, optc, minutes_str, hours_str, daysofweek_str, argc, argv);
			break;
		case CLIENT_REQUEST_REMOVE_TASK :
		case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES :
		case CLIENT_REQUEST_GET_STDOUT :
		case CLIENT_REQUEST_GET_STDERR :
			write_operation_with_task_id(fd, operation, taskid);
			break;
	}
	return EXIT_SUCCESS;
}

/* ECRITURE DES REPONSES PAR COMMANDE*/
int write_reply_error_remove(int fd) {
	uint16_t notOK = SERVER_REPLY_ERROR;
	uint16_t err = SERVER_REPLY_ERROR_NOT_FOUND;

	char buf[PIPE_BUF];
	size_t buf_size = 0;
	notOK = htobe16(notOK);
	bufcat(buf, &buf_size, &notOK, SIZE_UINT16);
	err = htobe16(err);
	bufcat(buf, &buf_size, &err, SIZE_UINT16);

	if(write(fd, buf, buf_size) < buf_size) {
		perror("write_request_remove() write");
		close(fd); 
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int write_reply_list(int fd, char *tasks_dir_path) {
	char buf[PIPE_BUF];
	size_t buf_size = 0;

	uint16_t OK = SERVER_REPLY_OK;
	bufcat(buf, &buf_size, &OK, SIZE_UINT16);

	uint32_t nb_tasks_not_deleted = count_tasks_not_deleted(tasks_dir_path);
	nb_tasks_not_deleted = htobe32(nb_tasks_not_deleted);
	bufcat(buf, &buf_size, &nb_tasks_not_deleted, SIZE_UINT32);

	uint32_t nb_task = count_directory(tasks_dir_path);
	for(uint64_t i = 0; i < nb_task; i++) {
		char *id_name = malloc(sizeof(i));
		sprintf(id_name, "%" PRIu64, i);

		char *taskid_path = concat(tasks_dir_path, id_name);

		if(!file_exists(taskid_path)) {
			// CONVERTION DE ID EN UINT64_T
			uint64_t taskid = i;
			taskid = htobe64(taskid);
			bufcat(buf, &buf_size, &taskid, SIZE_UINT64);

			// RECUPERATION LES DONNEES DE TEMPS
			char *time_file = concat(taskid_path, "/time");

			char *time = NULL;
			time = read_file(time_file, time);
			char *ptr = strtok(time, " ");

			char *min = ptr;
			ptr = strtok(NULL, " ");
			char *ho = ptr;
			ptr = strtok(NULL, " ");
			char *day = ptr;

			struct timing *tms = malloc(sizeof(struct timing));
			timing_from_strings(tms, min, ho, day);

			uint64_t m = htobe64(tms->minutes);
			uint32_t h = htobe32(tms->hours);
			bufcat(buf, &buf_size, &m, SIZE_UINT64);
			bufcat(buf, &buf_size, &h, SIZE_UINT32);
			bufcat(buf, &buf_size, &tms->daysofweek, SIZE_UINT8);

			free(time_file);
			free(time);
			free(tms);

			// RECUPERATION LES DONNEES DES ARGUMENTS
			char *args_file = concat(taskid_path, "/arguments");

			char *args = NULL;
			args = read_file(args_file, args);
			uint32_t nb = count_arguments(args);
			uint32_t write_nb = htobe32(nb);
			bufcat(buf, &buf_size, &write_nb, SIZE_UINT32);
			ptr = strtok(args, " ");

			for(int i = 0; i < nb; i++) {
				uint32_t curr_arg_l = strlen(ptr);
				curr_arg_l = htobe32(curr_arg_l);
				bufcat(buf, &buf_size, &curr_arg_l, SIZE_UINT32);

				char *cmd_curr_arg;
				cmd_curr_arg = malloc(strlen(ptr) + 1);
				strcpy(cmd_curr_arg, ptr);
				bufcat(buf, &buf_size, cmd_curr_arg, strlen(cmd_curr_arg));
				free(cmd_curr_arg);

				ptr = strtok(NULL, " ");
			}

			free(args_file);
			free(args);
		}
	}

	if(write(fd, buf, buf_size) < buf_size) {
		perror("write_request_list() write");
		close(fd); exit(-1);
	}

	return EXIT_SUCCESS;
}

int write_reply_ok_stderr_stdout(int fd, char *res_char) {
	uint16_t OK = SERVER_REPLY_OK;

	char buf[PIPE_BUF];
	size_t buf_size = 0;
	OK = htobe16(OK);
	bufcat(buf, &buf_size, &OK, SIZE_UINT16);

	uint32_t curr_arg_l = strlen(res_char);
	curr_arg_l = htobe32(curr_arg_l);
	bufcat(buf, &buf_size, &curr_arg_l, SIZE_UINT32);

	char *cmd_curr_arg;
	cmd_curr_arg = malloc(strlen(res_char) + 1);
	strcpy(cmd_curr_arg, res_char);
	bufcat(buf, &buf_size, cmd_curr_arg, strlen(cmd_curr_arg));
	free(cmd_curr_arg);
	
	if(write(fd, buf, buf_size) < buf_size) {
		perror("write_request_stdout_stderr() write");
		close(fd); 
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int write_reply_notok_stderr_stdout(int fd, char *task_dir_path, uint64_t taskid) {
	uint16_t notOK = SERVER_REPLY_ERROR;

	char buf[PIPE_BUF];
	size_t buf_size = 0;
	notOK = htobe16(notOK);
	bufcat(buf, &buf_size, &notOK, SIZE_UINT16);

	char *id_name = malloc(sizeof(uint64_t));
	sprintf(id_name, "%" PRIu64, taskid);
	task_dir_path = concat(task_dir_path, id_name);

	uint16_t err;
	if(file_exists(task_dir_path) != 0) err = SERVER_REPLY_ERROR_NOT_FOUND;
	else err = SERVER_REPLY_ERROR_NEVER_RUN;
	err = htobe16(err);
	bufcat(buf, &buf_size, &err, SIZE_UINT16);

	if(write(fd, buf, buf_size) < buf_size) {
		perror("write_request_remove() write");
		close(fd); 
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void write_reply_times_exitcode(int fd, uint16_t taskid, char* task_dir_path){
	//Lecture du fichier time_exitcode
	uint16_t reptype = SERVER_REPLY_OK;
	if(!file_exists(task_dir_path)){
		reptype = SERVER_REPLY_ERROR;
	}
	uint32_t nbruns;
	char* tec;
	char *id_name = malloc(SIZE_UINT64);
	if(id_name == NULL){
		perror("malloc id_name read_times_and_exit_code_request");
		close(fd); 
		exit(EXIT_FAILURE);
	}
	
	sprintf(id_name, "%u" PRIu64, taskid);
	task_dir_path = concat(task_dir_path, "/");
	task_dir_path = concat(task_dir_path, id_name);

	char *time_exitcode_file = concat(task_dir_path, "/time_exitcode");

	int exitcode_file_fd = open(time_exitcode_file,O_RDONLY);

	char buffer[2];

	tec = malloc(1024);
	int offset = 0;

	while(read(exitcode_file_fd, buffer, 1) != 0) {
		if (strcmp(buffer, "\n") == 0) {
			nbruns ++;
		}
		memcpy(tec + offset, buffer, 1);
		offset++;
	}
	tec[offset] = '\0';

	//Ecriture dans la pipe 
	char buf[PIPE_BUF];
	size_t buf_size = 0;

	reptype = htobe16(reptype);
	bufcat(buf,&buf_size,&reptype,SIZE_UINT16);

	if(reptype == SERVER_REPLY_OK){
		nbruns = htobe32(nbruns);
		bufcat(buf,&buf_size,&nbruns,SIZE_UINT32);

		bufcat(buf,&buf_size,&tec,sizeof(tec));
	}
	if(reptype == SERVER_REPLY_ERROR){
		uint16_t error = SERVER_REPLY_ERROR_NOT_FOUND;
		error = htobe16(error);

		bufcat(buf,&buf_size,&error,SIZE_UINT16);
	}
	
	if(write(fd, &buf, buf_size) < buf_size){
			perror("write reply times_exitcode");
			close(fd);
			exit(EXIT_FAILURE);
	}
}
