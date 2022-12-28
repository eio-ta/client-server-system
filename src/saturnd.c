#include "../include/saturnd.h"

void forks_demon(){
    pid_t pid;

    // Premier fork
    pid = fork(); 

    if(pid < 0) {
        perror("first fork deamon");
        exit(EXIT_FAILURE);
    }

	// PERE
    if(pid > 0) exit(EXIT_SUCCESS);
    
	// DEUXIEME FORK
    pid = fork();

	if(pid < 0) {
        perror("second fork deamon");
        exit(EXIT_FAILURE);
    }
	
	// PERE
    if(pid > 0) exit(EXIT_SUCCESS);
}

void exec_task(char *task_folder, struct tm *t) {
	char *args_file = concat(task_folder, "/arguments");
	char *stdout_file = concat(task_folder, "/stdout");
	char *stderr_file = concat(task_folder, "/stderr");
	int args_fd = open(args_file, O_RDONLY);
	char *cmd;
	char buffer[2];
	cmd = malloc(PIPE_BUF);
	int offset = 0;
	while(read(args_fd, buffer, 1) != 0) {
		memcpy(cmd + offset, buffer, 1);
		offset++;
	}
	cmd[offset] = '\0';
	int argsc = count_arguments(cmd) - 1;
	int cmdac = 1 + count_arguments(cmd);
	char *cmd_name = malloc(sizeof(char) * 1024);
	char *token;
	token = strtok(cmd, " ");
	strcpy(cmd_name, token);
	char *args[cmdac];
	for (int i = 0; i < cmdac; i++) {
		args[i] = malloc(1024);
	}
	strcpy(args[0], cmd_name);
	int i = 1;
	while(token != NULL) {
		token = strtok(NULL, " ");
		if (i <= argsc) 
			strcpy(args[i], token);
		i++;
	}
	args[cmdac - 1] = NULL;
	char *time_file = concat(task_folder, "/time");
	char *c = malloc(PIPE_BUF);
	int tf_fd = open(time_file, O_RDONLY);
	char buf[1];
	offset = 0;
	while (read(tf_fd, buf, 1) != 0) {
		memcpy(c + offset, buf, 1);
		offset++;
	}
	c[offset] = '\0';
	char *m = "";
	char *h = "";
	char *d = "";
	token = strtok(c, " ");
	m = concat(m, token);
	h = concat(h, strtok(NULL, " "));
	d = concat(d, strtok(NULL, " "));
	struct timing *tim = NULL;
	timing_from_strings(tim, m, h, d);
	if (1) {
		int fstdout = open(stdout_file, O_WRONLY | O_TRUNC);
		close(STDOUT_FILENO);
		dup2(fstdout, STDOUT_FILENO);
		int fstderr = open(stderr_file, O_WRONLY | O_TRUNC);
		close(STDERR_FILENO);
		dup2(fstderr, STDERR_FILENO);
		int r = fork();
		int retval;
		switch (r) {
			case -1:
				// Erreur à gérer
				exit(0);
				break;
			case 0:
				retval = execvp(cmd_name, args);
				break;
		}
		char *buf = "";
		char *time_exitcode_file = concat(task_folder, "/time_exitcode");
		time_t now = time(NULL);
		char *time_str = malloc(sizeof(time_t));
		sprintf(time_str, "%ld", now);
		char *retval_str = malloc(sizeof(retval));
		sprintf(retval_str, "%d", retval);
		buf = concat(buf, time_str);
		buf = concat(buf, " ");
		buf = concat(buf, retval_str);
		buf = concat(buf, "\n");
		int tmce_fd = open(time_exitcode_file, O_WRONLY | O_APPEND);
		write(tmce_fd, buf, strlen(buf));
	}
}

int main(int argc, char *argv[]) {
	forks_demon();

	// Champs pour récupérer le nom du propriétaire
	char *name;
	struct passwd *pass;
	pass = getpwuid(getuid());
	name = pass->pw_name;

	// Champs pour avoir les répertoires des tubes et des répertoires des tâches
	char *pipes_directory = "/tmp/";
	pipes_directory = concat(pipes_directory, name);
	pipes_directory = concat(pipes_directory, "/saturnd");

	char *pipes_dir_path = concat(pipes_directory, "/pipes");

	char *request_pipe_name = concat(pipes_dir_path, REQUEST_PIPE);
	char *reply_pipe_name = concat(pipes_dir_path, REPLY_PIPE);

	recursive_mkdir(pipes_dir_path);

	mkfifo(request_pipe_name, 0777);
	mkfifo(reply_pipe_name, 0777);

	char *tasks_dir_path = concat(pipes_directory, "/tasks/");

	recursive_mkdir(tasks_dir_path);
	
	// Champs pour récupérer les valeurs de la lecture du tube (si besoin)
	uint16_t opcode = 0;
	int res_int = 1;
	uint64_t taskid = -1;
	char *res_char = NULL;
	uint16_t repOK = SERVER_REPLY_OK;

	pid_t pid = fork();
	if (pid<0)
		return EXIT_FAILURE;
	if (pid == 0) {
		while (1) {
			sleep (60);
						time_t curr_time;
			struct tm *timeinfo;
			time (&curr_time);
			timeinfo = localtime (&curr_time);
			// we need hours, minutes, daysofweek
			DIR *dirp;
			struct dirent *entry;

			dirp = opendir(tasks_dir_path);
			while((entry = readdir(dirp)) != NULL) {
				if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0)
					continue;
				exec_task(concat(tasks_dir_path, entry->d_name), timeinfo);
			}
		closedir(dirp);
		}
	} if(pid > 0) {
		while(1) {
		sleep(1);
        
		// LECTURE DU TUBE DE REQUETE
		int fd = -1;
		fd = open(request_pipe_name, O_RDONLY);
		if(fd == -1) {
			perror("open read_request_pipe");
			close(fd); 
			return EXIT_FAILURE;
		}
		opcode = read_uint16(fd, opcode);
		switch (opcode) {
			case CLIENT_REQUEST_CREATE_TASK : read_request_create(fd, tasks_dir_path); break;
			case CLIENT_REQUEST_REMOVE_TASK : res_int = read_request_remove(fd, tasks_dir_path); break;
			case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES: taskid = read_uint64(fd, taskid); break;
			case CLIENT_REQUEST_GET_STDOUT : res_char = read_stdout_stderr(fd, tasks_dir_path, taskid, 0); break;
			case CLIENT_REQUEST_GET_STDERR : res_char = read_stdout_stderr(fd, tasks_dir_path, taskid, 1); break;
		}
		close(fd);
		
		// ECRITURE DU TUBE DE REPONSE
		int fw = -1;
		fw = open(reply_pipe_name, O_WRONLY);
		if(fw == -1) {
			perror("open write_reply_pipe");
			close(fw); 
			return EXIT_FAILURE;
		}
		switch (opcode) {
			case CLIENT_REQUEST_LIST_TASKS : write_reply_list(fw, tasks_dir_path); break;
			case CLIENT_REQUEST_CREATE_TASK : write_operation_with_task_id(fw, repOK, count_directory(tasks_dir_path)-1); break;
			case CLIENT_REQUEST_REMOVE_TASK :
				if(res_int == EXIT_SUCCESS) write_operation(fw, repOK);
				else write_reply_error_remove(fw);
				break;
			case CLIENT_REQUEST_GET_TIMES_AND_EXITCODES: write_reply_times_exitcode(fw, taskid, tasks_dir_path); break; 
				break;
			case CLIENT_REQUEST_GET_STDOUT :
				if(res_char != NULL) write_reply_ok_stderr_stdout(fw, res_char);
				else write_reply_notok_stderr_stdout(fw, tasks_dir_path, taskid);
				break;
			case CLIENT_REQUEST_GET_STDERR :
				if(res_char != NULL) write_reply_ok_stderr_stdout(fw, res_char);
				else write_reply_notok_stderr_stdout(fw, tasks_dir_path, taskid);
				break;
			case CLIENT_REQUEST_TERMINATE : write_operation(fw, repOK);
				close(fd);
				return EXIT_SUCCESS;
		}
		close(fw);
	}
	}


	
    
	// LIBERATION DE MEMOIRE
	free(pipes_directory);
	free(pipes_dir_path);
	free(request_pipe_name);
	free(reply_pipe_name);
	free(tasks_dir_path);
    
	return EXIT_SUCCESS;
}