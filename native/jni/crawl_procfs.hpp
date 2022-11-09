#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include "base.hpp"

using namespace std;

template<class F>
void crawl_procfs(const F &fn) {
    DIR *procfp = opendir("/proc");
    if (procfp == nullptr)
        return;
    dirent *dp;
    int pid;
    while ((dp = readdir(procfp))) {
        pid = parse_int(dp->d_name);
        if (pid > 0 && !fn(pid))
            break;
    }
    closedir(procfp);
}
