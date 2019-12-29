#include <stdio.h>
#include "keyin.h"
#include <string.h>

char *getstr(char *buf, size_t buf_size) {
	if (fgets(buf, buf_size, stdin) == NULL) {
		perror("Input Error: ");
		return NULL;
	}	
	
	/* Overwrite unwanted newline character */
	if (*(buf + strlen(buf) - 1) == '\n')
		*(buf + strlen(buf) - 1) = '\0';
		
	return buf;
}

long int get_long(char *buf, size_t buf_size) {
	printf("%s, %lu\n", buf, buf_size);
	return 0;
}

