#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <dirent.h>
#include <string.h>

void signal_handler(int sig);
void read_config();
void read_dir(char *d);
void read_file(struct dirent *cur_file);
void changeTimeOfLastCheck();

char dir[64];
int reading_period;
time_t now;

int main(int argc, char *argv[]) {
	pid_t pid, sid;
	signal(SIGHUP, signal_handler);
	signal(SIGTERM, signal_handler);
	pid = fork();
	switch (pid) {
	case 0:
		setsid();
		pid = fork();
		if (-1 == pid) {
			exit(EXIT_FAILURE);
		} else if (pid > 0) {
			exit(EXIT_SUCCESS);
		}
		now = time(NULL);
		// changing file-mask
		umask(0);

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		openlog("daemon_files", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
		read_config();
		syslog(LOG_INFO, "LISTENING DEMON IS STARTED\n");
		for (;;) {
			syslog(LOG_INFO, "сейчас я тут %s", dir);
			read_dir(dir);
			changeTimeOfLastCheck();
			sleep(reading_period);
		}
		break;

	case -1:
		exit(EXIT_FAILURE);

	default:
		exit(EXIT_SUCCESS);
	}

	return 0;
}

void signal_handler(int sig) {
	switch (sig) {
	case SIGHUP:
		syslog(LOG_INFO, "hangup signal catched");
		read_config();
		break;
	case SIGTERM:
		syslog(LOG_INFO, "terminate signal catched");
		exit(0);
		break;
	default:
		break;
	}
}
// done
void read_config() {
	if (chdir("/home/andrey") < 0) {
		syslog(LOG_INFO, "cant change dir");
		exit(EXIT_FAILURE);
	}
	FILE *config = fopen("./config", "r");
	fscanf(config, "%s %d", dir, &reading_period);
	syslog(LOG_INFO, "directory is %s\n period is %d\n", dir, reading_period);
	fclose(config);
	if (chdir(dir) < 0) {
		syslog(LOG_INFO, "cant change dir");
		exit(EXIT_FAILURE);
	}
	char tmp[64];
	syslog(LOG_INFO, "working dir is %s", getcwd(tmp, 64));
}

void read_dir(char *cur_dir) {
	DIR *d;
	struct dirent *entry;
	if ((d = opendir(cur_dir)) == NULL) {
		syslog(LOG_INFO,"permission denied for %s", cur_dir);
		return;
	}
	while ((entry = readdir(d)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0
				|| strcmp(entry->d_name, "..") == 0) {
			continue;
		} else if (entry->d_type == DT_DIR) {
			read_dir(entry->d_name);
		} else {
			read_file(entry);
		}
	}
}

void read_file(struct dirent *cur_file) {
	struct stat sb;
	stat(cur_file->d_name, &sb);
	if (difftime(sb.st_mtime, now) > 0) {
		syslog(LOG_INFO, "\nfile %s was changed after demon was started\n",
				cur_file->d_name);
		syslog(LOG_INFO, "time is %d\n", sb.st_mtim);
	}
}

void changeTimeOfLastCheck() {
	now = time(NULL);
}
