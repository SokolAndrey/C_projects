//============================================================================
// Name        : findProcesses.cpp
// Author      : Andrei
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string.h>

// get itself proc id
pid_t get_pid_from_proc() {
	char target[32];
	int pid;
	readlink("/proc/self", target, sizeof(target));
	sscanf(target, "%d", &pid);
	return (pid_t) pid;
}

void print_proc_arguments(pid_t pid) {
	int fd;
	char filename[24];
	char arg_list[1024];
	size_t length;
	char *next_arg;

	/* Generate the name of the cmdline file for the process. */
	snprintf(filename, sizeof(filename), "/proc/%d/cmdline", (int) pid);
	/* Read the contents of the file. */
	fd = open(filename, O_RDONLY);
	length = read(fd, arg_list, sizeof(arg_list));
	close(fd);
	/* read does not NUL-terminate the buffer, so do it here. */
	arg_list[length] = '\0';
	/* Loop over arguments. Arguments are separated by NULs. */
	next_arg = arg_list;
	while (next_arg < arg_list + length) {
		/* Print the argument. Each is NUL-terminated, so just treat it
		 like an ordinary string. */
		printf("%s\n", next_arg);
		/* Advance to the next argument. Since each argument is
		 NUL-terminated, strlen counts the length of the next argument,
		 not the entire argument list. */
		next_arg += strlen(next_arg) + 1;
	}
}

void print_process_environment(pid_t pid) {
	int fd;
	char filename[24];
	char environment[8192];
	size_t length;
	char* next_var;
	/* Generate the name of the environ file for the process. */
	snprintf (filename, sizeof (filename), "/proc/%d/environ", (int) pid);
	/* Read the contents of the file. */
	fd = open(filename, O_RDONLY);
	length = read(fd, environment, sizeof(environment));
	close(fd);
	/* read does not NUL-terminate the buffer, so do it here. */
	environment[length] = '\0';
	/* Loop over variables.  Variables are separated by NULs. */
	next_var = environment;
	while (next_var < environment + length) {
		/* Print the variable.  Each is NUL-terminated, so just treat it
		 like an ordinary string.  */
		printf ("%s\n", next_var);
		/* Advance to the next variable.  Since each variable is
		 NUL-terminated, strlen counts the length of the next variable,
		 not the entire variable list.  */
		next_var += strlen(next_var) + 1;
	}
}

int get_TTY_of_proc(pid_t pid){
	int TTY;
	char filename[24];
	snprintf (filename, sizeof (filename), "/proc/%d/stat", (int) pid);
	int fd = open(filename, O_RDONLY);
	return TTY;
}
int main(int argc, char* argv[]) {
	char com[] ="ps a | grep ";
	char str[sizeof(com) + sizeof(argv[1])];
	strcpy(str, com);
	strcat(str,argv[1]);
	int res = system(str);
	std::cout << res;
	return 0;
}
