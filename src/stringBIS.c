#include "../include/stringBIS.h"

string* create_string(uint32_t len, char *word) {
	string *str = malloc(sizeof(str));
	if(str == NULL){
		perror("malloc str creat_string");
		exit(EXIT_FAILURE);
	}
	str->len = len;
	str->word = malloc(len);
	if(str->word == NULL){
		perror("malloc str->word creat_string");
		exit(EXIT_FAILURE);
	}
	memcpy(str->word, word, len);
	return str;
}

void set_string(string *str, uint32_t len, char *word) {
	str->len = len;
	str->word = word;
}

void print_string(string *str) {
	printf("%s", str->word);
}