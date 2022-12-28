#include "../include/commandline.h"

commandline create_commandline(uint32_t argc, string *argv) {
	commandline cm;
	cm.argc = argc;
	cm.argv = argv;
	return cm;
}

void print_commandline(commandline *cm) {
	for (int i = 0; i < cm->argc; i++) {
		printf(" %s", cm->argv[i].word);
	}
}

char* cml_to_char(commandline *cm) {
	char *res = "";
	for(int i = 0; i < cm->argc; i++) {
		res = concat(res, cm->argv[i].word);
		res = concat(res, " ");
	}
	return res;
}