#include "../include/utils.h"

void recursive_mkdir(const char *path) {
	char tmp[256];
	char *p = NULL;
	size_t len = 0;
	snprintf(tmp, sizeof(tmp), "%s", path);
	len = strlen(tmp);
	if (tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for (p = tmp + 1; *p; p++)
		if (*p == '/') {
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	mkdir(tmp, S_IRWXU);
}

int count_directory(char *path) {
	int file_count = 0;
	DIR *dirp;
	struct dirent *entry;

	dirp = opendir(path);
	while((entry = readdir(dirp)) != NULL) {
		file_count++;
    }
	closedir(dirp);
	return file_count - 2;
}

uint32_t count_tasks_not_deleted(char *path) {
	uint32_t file_count = 0;
	DIR *dirp;
	struct dirent *entry;

	dirp = opendir(path);
	while((entry = readdir(dirp)) != NULL) {
		char *res = strstr(entry->d_name, "(deleted)");
		if(res == NULL) file_count++;
    }
	closedir(dirp);
	return file_count - 2;
}

int create_file_task(char *path, char *contents) {
	int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror("open creat_file_task");
		close(fd);
		return EXIT_FAILURE;
	}
	if(write(fd, contents, strlen(contents)) < strlen(contents)){
		perror("write create_file_task");
		close(fd); 
		return EXIT_FAILURE;
	}
	close(fd);
	return EXIT_SUCCESS;
}

char *read_file(char *path, char *contents) {
	int fh;
	fh = open(path, O_RDONLY);
	if(fh == -1) {
		perror("open");
		return NULL;
    } else {
		struct stat v;
        stat(path, &v);
		int size = v.st_size;
		contents = malloc(size * sizeof(char));
		int ret = read(fh, contents, size);
		if(ret == -1) {
			perror("read"); return NULL;
		}
		close(fh);
		contents = concat(contents, "\0");
		return contents;
	}
}

int file_exists(char *filename) {
	struct stat buf;
	return stat(filename, &buf);
}

char *concat(const char *str1, const char *str2) {
	char *result = malloc(sizeof(char) * (strlen(str1) + strlen(str2) + 1));
	if(result == NULL) {
		perror("concat()");
		exit(EXIT_FAILURE);
	}
	strcpy(result, str1); strcat(result, str2);
	return result;
}

void bufcat(char *buf, size_t *buf_size, void *val, size_t size) {
	memcpy(buf + *buf_size, val, size); *buf_size += size;
}

uint32_t count_arguments(char *argv) {
	uint32_t nb_args = 0;
	for(int i = 0; i < strlen(argv); i++) {
		if(argv[i] == ' ') nb_args++;
	}
	return nb_args;
}