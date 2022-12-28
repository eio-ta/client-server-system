#include "../include/read-pipes.h"

/* LECTURE D'ENTIERS */

uint16_t read_uint16(int fd, uint16_t buf) {
	if(read(fd, &buf, SIZE_UINT16) < SIZE_UINT16) {
		perror("read_uint16()"); 
		close(fd); 
		return EXIT_FAILURE;
	}
	buf = be16toh(buf);
	return buf;
}

uint32_t read_uint32(int fd, uint32_t buf) {
	if(read(fd, &buf, SIZE_UINT32) < SIZE_UINT32) {
		perror("read_uint32()");
		close(fd); 
		return EXIT_FAILURE;
	}
	buf = be32toh(buf);
	return buf;
}

uint64_t read_uint64(int fd, uint64_t buf) {
	if(read(fd, &buf, SIZE_UINT64) < SIZE_UINT64) {
		perror("read_uint64()");
		close(fd); 
		return EXIT_FAILURE;
	}
	buf = be64toh(buf);
	return buf;
}

int64_t read_int64(int fd, int64_t buf) {
	if(read(fd, &buf, sizeof(int64_t)) < sizeof(int64_t)) {
		perror("read_int64()");
		close(fd); 
		return EXIT_FAILURE;
	}
	buf = be64toh(buf);
	return buf;
}


/* LECTURE DE TYPES ET DE STRUCTURES */

timing read_timing(int fd, char *timing_str) {
	timing time;
	if(read(fd, &time.minutes, SIZE_UINT64) < SIZE_UINT64){
		perror("read uint64 timing");
		close(fd); 
		exit(EXIT_FAILURE);
	}
	if(read(fd, &time.hours, SIZE_UINT32) < SIZE_UINT32){
		perror("read uint32 timing");
		close(fd); 
		exit(EXIT_FAILURE);
	}
	if(read(fd, &time.daysofweek, SIZE_UINT8) < SIZE_UINT8){
		perror("read uint8 timing");
		close(fd); 
		exit(EXIT_FAILURE);
	}
	time.hours = be32toh(time.hours);
	time.minutes = be64toh(time.minutes);
	timing_string_from_timing(timing_str, &time);
	return time;
}

commandline* read_commandline(int fd) {
	uint32_t len_cl = 0;
	len_cl = read_uint32(fd, len_cl);
    commandline *cmdl = malloc(sizeof(commandline));
	if(cmdl == NULL) {
		perror("malloc cmdl read_commandline");
		close(fd); 
		exit(EXIT_FAILURE);
	}
	cmdl->argc = len_cl;
	cmdl->argv = malloc(sizeof(string) * len_cl);
	if(cmdl->argv == NULL){
		perror("malloc cmdl->argv read_commandline");
		close(fd); 
		exit(EXIT_FAILURE);
	}
	for(uint32_t i = 0; i < len_cl; i++) {
		cmdl->argv[i] = *read_string(fd);
	}
	return cmdl;
}

string* read_string(int fd) {
	uint32_t len_str = 0;
	len_str = read_uint32(fd, len_str);
	char str[len_str + 1];
	if(read(fd, str, len_str) < len_str) {
		perror("read_string()");
		close(fd); 
		exit(EXIT_FAILURE);
	}
	str[len_str] = '\0';
	string *st = create_string(len_str + 1, str);
	return st;
}


/* LECTURE DES REQUETES */

int read_request_create(int fd, char *tasks_dir_path) {
	uint64_t id = count_directory(tasks_dir_path);
	
	char timing_str[TIMING_TEXT_MIN_BUFFERSIZE];
	read_timing(fd, timing_str);

	commandline *cmdl = read_commandline(fd);

	char *id_name = malloc(sizeof(id));
	if(id_name == NULL){
		perror("malloc id_name read_request_create");
		close(fd); 
		return EXIT_FAILURE;
	}
	sprintf(id_name, "%" PRIu64, id);
	char *task_dir_path = concat(tasks_dir_path, id_name);


	// CREATION DU DOSSIER
	recursive_mkdir(task_dir_path);


	// CREATION DES FICHIERS
	char *args_file = concat(task_dir_path, "/arguments");
	char *time_file = concat(task_dir_path, "/time");
	char *stdout_file = concat(task_dir_path, "/stdout");
	char *stderr_file = concat(task_dir_path, "/stderr");
	char *time_exitcode_file = concat(task_dir_path, "/time_exitcode");
	
	char *res = cml_to_char(cmdl);
	
	if(create_file_task(args_file, res) == EXIT_FAILURE) return EXIT_FAILURE;
	if(create_file_task(time_file, concat(timing_str, "\n")) == EXIT_FAILURE) return EXIT_FAILURE;
	if(create_file_task(stdout_file, "") == EXIT_FAILURE) return EXIT_FAILURE;
	if(create_file_task(stderr_file, "") == EXIT_FAILURE) return EXIT_FAILURE;
	if(create_file_task(time_exitcode_file, "") == EXIT_FAILURE) return EXIT_FAILURE;

	// LIBERATION DE LA MEMOIRE
	free(cmdl);
	free(id_name);
	free(args_file);
	free(time_file);
	free(stdout_file);
	free(stderr_file);
	free(time_exitcode_file);
	free(res);

    return EXIT_SUCCESS;
}

int read_request_remove(int fd, char *task_dir_path) {
	uint64_t taskid = -1;
	taskid = read_uint64(fd, taskid);

	char *id_name = malloc(sizeof(taskid));
	if(id_name == NULL){
		perror("malloc id_name read_request_remove");
		close(fd); 
		return EXIT_FAILURE;
	}
	sprintf(id_name, "%" PRIu64, taskid);
	task_dir_path = concat(task_dir_path, "/");
	task_dir_path = concat(task_dir_path, id_name);

	char *new_name = concat(task_dir_path, " (deleted)");
	if(rename(task_dir_path, new_name) == -1) return -1;

	free(id_name);
	free(new_name);
	
    return EXIT_SUCCESS;
}

char *read_stdout_stderr(int fd, char *task_dir_path, uint64_t taskid, int request) {
	taskid = read_uint64(fd, taskid);
	char *id_name = malloc(sizeof(uint64_t));
	sprintf(id_name, "%" PRIu64, taskid);

	task_dir_path = concat(task_dir_path, id_name);

	if(file_exists(task_dir_path) != 0) return NULL;
	char *stdout_file = concat(task_dir_path, "/stdout");
	char *stderr_file = concat(task_dir_path, "/stderr");

	char *args = NULL;
	if(request == 0) args = read_file(stdout_file, args);
	else args = read_file(stderr_file, args);

	if(strlen(args) == 0) return NULL;

	return args;
}

/* LECTURE DES REPONSES */

void read_reply_ls(int fd, uint64_t taskid, char *timing_str) {
	uint16_t buf = 0;
	uint32_t nbtask = 0;
	buf = read_uint16(fd, buf);
	nbtask = read_uint32(fd, nbtask);
	for (uint32_t i = 0; i < nbtask; i++) {
		taskid = read_uint64(fd, taskid);
		timing time = read_timing(fd, timing_str);
		if((!(timing_string_from_timing(timing_str, &time))) == 1) {
			perror("read_reply_ls() timing");
		}
		printf("%lu: %s", taskid, timing_str);
		print_commandline(read_commandline(fd));
		printf("\n");
	}
}

void read_reply_create(int fd, uint64_t taskid) {
	uint16_t buf = 0;
	buf = read_uint16(fd, buf);
	taskid = read_uint64(fd, taskid);
	printf("%lu\n", taskid);
}

int read_reply_remove(int fd) {
	uint16_t reptype = 0;
	reptype = read_uint16(fd, reptype);
	if(reptype == SERVER_REPLY_OK) {
		return EXIT_SUCCESS;
	} else {
		uint16_t error = 0;
		error = read_uint16(fd, error);
		if(error == SERVER_REPLY_ERROR_NOT_FOUND) {
			printf("il n'existe aucune tâche avec cet identifiant\n");
		}
		return EXIT_FAILURE;
	}
}

int read_reply_time_exitcode(int fd) {
	char res[100];
	uint16_t reptype = 0;
	reptype = read_uint16(fd, reptype);
	if(reptype == SERVER_REPLY_OK) {
		uint32_t nbruns = 0;
		nbruns = read_uint32(fd, nbruns);
		for (uint32_t i = 0; i < nbruns; i++) {
			int64_t time = 0;
			time = read_int64(fd, time);
			time_t timestmp = time;
			struct tm *timeInfo = localtime(&timestmp);
			strftime(res, 1000, "%Y-%m-%d %H:%M:%S", timeInfo);
			printf("%s", res);
			uint16_t exitcode = 0;
			exitcode = read_uint16(fd, exitcode);
			printf(" %u\n", exitcode);
		}
		return EXIT_SUCCESS;
	} else {
		uint16_t error = 0;
		error = read_uint16(fd, error);
		if(error == SERVER_REPLY_ERROR_NOT_FOUND) {
			printf("il n'existe aucune tâche avec cet identifiant");
		}
		perror("read_reply_time_exitcode()");
		return EXIT_FAILURE;
	}
}

int read_reply_stdout_stderr(int fd) {
	uint16_t reptype = 0;
	reptype = read_uint16(fd, reptype);
	if(reptype == SERVER_REPLY_ERROR) {
		uint16_t errcode = 0;
		errcode = read_uint16(fd, errcode);
		if(errcode == SERVER_REPLY_ERROR_NOT_FOUND) printf("il n'existe aucune tâche avec cet identifiant\n");
		else printf("la tâche n'a pas encore été exécutée au moins une fois\n");
		return EXIT_FAILURE;
	} else {
		string *str = read_string(fd);
		print_string(str);
		return EXIT_SUCCESS;
	}
}

int read_reply_pipe (int fd, char *reply_pipe_name, uint16_t operation, uint64_t taskid) {
	if((fd = open(reply_pipe_name, O_RDONLY)) == -1){
		perror("open read_reply_pipe");
		close(fd); 
		return EXIT_FAILURE;
	}

	char timing_str[TIMING_TEXT_MIN_BUFFERSIZE];
	uint16_t buf = 0;
	
	switch (operation) {
		case CLIENT_REQUEST_LIST_TASKS : read_reply_ls(fd, taskid, timing_str); break;
		case CLIENT_REQUEST_CREATE_TASK : read_reply_create(fd, taskid); break;
		case CLIENT_REQUEST_TERMINATE : read_uint16(fd, buf); break;
		case CLIENT_REQUEST_REMOVE_TASK :
			if(read_reply_remove(fd) == EXIT_FAILURE) return EXIT_FAILURE;
			break;
		case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES:
			if(read_reply_time_exitcode(fd) == EXIT_FAILURE) return EXIT_FAILURE;
			break;
		case CLIENT_REQUEST_GET_STDOUT:
			if(read_reply_stdout_stderr(fd) == EXIT_FAILURE) return EXIT_FAILURE;
			break;
		case CLIENT_REQUEST_GET_STDERR:
			if(read_reply_stdout_stderr(fd) == EXIT_FAILURE) return EXIT_FAILURE;
			break;
	}
	return EXIT_SUCCESS;
}