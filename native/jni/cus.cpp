#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <string>
#include <sys/prctl.h>
#include "crawl_procfs.hpp"

static char *argv0;
static size_t name_len;

size_t strscpy(char *s, const char *ss, size_t siz) {
    for (size_t i=0; i<siz ; i++) {
        s[i] = ss[i];
    }
	return siz;
}

static void init_argv0(int argc, char **argv) {
    argv0 = argv[0];
    name_len = (argv[argc - 1] - argv[0]) + strlen(argv[argc - 1]) + 1;
}

static void fill_zero(int argc, char **argv) {
    for (int i = 0 ; i<argc ; i++) {
        char *s = argv[i];
        for (int j = 0 ; j < strlen(s) ; j++) {
    	        s[j] = '\0';
        }
    }
}

void set_nice_name(int argc, char **argv, const char *name) {
    init_argv0(argc, argv);
    fill_zero(argc,argv);
    memset(argv0, 0, name_len);
    strscpy(argv0, name, strlen(name)+1);
    prctl(PR_SET_NAME, name);
}


int read_ns(const int pid, struct stat *st) {
    char path[32];
    sprintf(path, "/proc/%d/ns/mnt", pid);
    return stat(path, st);
}

int parse_ppid(int pid) {
    char path[32];
    int ppid;

    sprintf(path, "/proc/%d/stat", pid);

    FILE *pfstat = fopen(path, "re");
    if (!pfstat)
        return -1;

    // PID COMM STATE PPID .....
    fscanf(pfstat, "%*d %*s %*c %d", &ppid);

    fclose(pfstat);

    return ppid;
}

int self_pid() {
    int pid;

    FILE *pfstat = fopen("/proc/self/stat", "re");
    if (!pfstat)
        return -1;

    // PID COMM STATE PPID .....
    fscanf(pfstat, "%d", &pid);

    fclose(pfstat);

    return pid;
}

int switch_mnt_ns(int pid) {
    char mnt[32];
    snprintf(mnt, sizeof(mnt), "/proc/%d/ns/mnt", pid);
    if (access(mnt, R_OK) == -1) return 1; // Maybe process died..

    int fd, ret;
    fd = open(mnt, O_RDONLY);
    if (fd < 0) return 1;
    // Switch to its namespace
    ret = setns(fd, 0);
    close(fd);
    return ret;
}

void __write_module_status(const char *status, int propfile_fd, int propmirror_fd){
    FILE *fp = fdopen(propmirror_fd, "re");
    if (!fp) return;
    FILE *fw = fdopen(propfile_fd, "w");
    ftruncate(propfile_fd, 0);
    if (!fw) {
        fclose(fp);
        return;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1){
        if (strstr(line, "description=") == line){
            char *z = line + strlen("description=");
            fprintf(fw, "description=[ %s ] %s\n", status, z);
        } else { 
            fprintf(fw, "%s", line);
        }
    }
}

void kill_all(const char *name) {
    crawl_procfs([=](int pid) -> bool {
        char path[128];
        sprintf(path, "/proc/%d/cmdline", pid);
        int fd = open(path, O_RDONLY);
        if (char buf[1024]; fd >= 0) {
		    if (read(fd, buf, sizeof(buf)-1) > 0 && 
				strcmp(buf, name) == 0) {
        		kill(pid, SIGKILL);
			}
        	close(fd);
        }
        return true;
    });
}
