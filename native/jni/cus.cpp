#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sched.h>
#include <string>

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

void __write_module_status(const char *status, const char *propfile, const char *propmirror){
    FILE *fp = fopen(propfile, "re");
    if (!fp) return;
    FILE *fw = fopen(propmirror, "w");
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
    fclose(fp);
    fclose(fw);
}
