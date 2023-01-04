#pragma once

int read_ns(const int pid, struct stat *st);
int parse_ppid(int pid);
int self_pid();
int switch_mnt_ns(int pid);
void __write_module_status(const char *status, const char *propfile, const char *propmirror);


#define PROPFILE std::string(std::string(MODPATH) + "/module.prop").data()
#define PROPMIRR std::string(std::string(MAGISKTMP) + "/.magisk/modules/" + std::string(MODNAME) + "/module.prop").data()
#define ___write(text) if (MAGISKTMP) __write_module_status(text, PROPFILE, PROPMIRR)

