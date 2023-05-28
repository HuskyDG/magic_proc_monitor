#pragma once

int read_ns(const int pid, struct stat *st);
int parse_ppid(int pid);
int self_pid();
int switch_mnt_ns(int pid);
void __write_module_status(const char *status, const char *propfile, const char *propmirror);
void set_nice_name(int argc, char **argv, const char *name);
void kill_all(const char *name);

extern bool write_propfd;

extern char* prop_mirror;
extern char* prop_status;

#define SECRETNAME "proc_monitor__PLACEHOLDERNAME"

#define set_nice_name(s) set_nice_name(argc,argv,s)


#define PROPFILE "/data/adb/modules/magisk_proc_monitor/module.prop"
#define WPROPFILE "/dev/" SECRETNAME
#define ___write(text) if (write_propfd) __write_module_status(text, prop_status, prop_mirror)

