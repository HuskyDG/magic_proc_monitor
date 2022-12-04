#pragma once

int read_ns(const int pid, struct stat *st);
int parse_ppid(int pid);
int self_pid();
int switch_mnt_ns(int pid);
