#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int parse_int(std::string_view s) {
    int val = 0;
    for (char c : s) {
        if (!c) break;
        if (c > '9' || c < '0')
            return -1;
        val = val * 10 + c - '0';
    }
    return val;
}

int fork_dont_care() {
    if (int pid = fork()) {
        waitpid(pid, nullptr, 0);
        return pid;
    } else if (fork()) {
        exit(0);
    }
    return 0;
}
