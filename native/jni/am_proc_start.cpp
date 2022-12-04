// Code are copied from https://gist.github.com/vvb2060/a3d40084cd9273b65a15f8a351b4eb0e#file-am_proc_start-cpp

#include <unistd.h>
#include <string>
#include <cinttypes>
#include <android/log.h>
#include <sys/system_properties.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include "crawl_procfs.hpp"
#include "cus.hpp"
#include <libgen.h>
#include <sys/mount.h>
#include <vector>
#include "logging.h"

using namespace std;

int myself;

const char *MAGISKTMP = nullptr;
vector<string> module_list;

extern "C" {

struct logger_entry {
    uint16_t len;      /* length of the payload */
    uint16_t hdr_size; /* sizeof(struct logger_entry) */
    int32_t pid;       /* generating process's pid */
    uint32_t tid;      /* generating process's tid */
    uint32_t sec;      /* seconds since Epoch */
    uint32_t nsec;     /* nanoseconds */
    uint32_t lid;      /* log id of the payload, bottom 4 bits currently */
    uint32_t uid;      /* generating process's uid */
};

#define LOGGER_ENTRY_MAX_LEN (5 * 1024)
struct log_msg {
    union [[gnu::aligned(4)]] {
        unsigned char buf[LOGGER_ENTRY_MAX_LEN + 1];
        struct logger_entry entry;
    };
};

[[gnu::weak]] struct logger_list *android_logger_list_alloc(int mode, unsigned int tail, pid_t pid);
[[gnu::weak]] void android_logger_list_free(struct logger_list *list);
[[gnu::weak]] int android_logger_list_read(struct logger_list *list, struct log_msg *log_msg);
[[gnu::weak]] struct logger *android_logger_open(struct logger_list *list, log_id_t id);

typedef struct [[gnu::packed]] {
    int32_t tag;  // Little Endian Order
} android_event_header_t;

typedef struct [[gnu::packed]] {
    int8_t type;   // EVENT_TYPE_INT
    int32_t data;  // Little Endian Order
} android_event_int_t;

typedef struct [[gnu::packed]] {
    int8_t type;     // EVENT_TYPE_STRING;
    int32_t length;  // Little Endian Order
    char data[];
} android_event_string_t;

typedef struct [[gnu::packed]] {
    int8_t type;  // EVENT_TYPE_LIST
    int8_t element_count;
} android_event_list_t;

// 30014 am_proc_start (User|1|5),(PID|1|5),(UID|1|5),(Process Name|3),(Type|3),(Component|3)
typedef struct [[gnu::packed]] {
    android_event_header_t tag;
    android_event_list_t list;
    android_event_int_t user;
    android_event_int_t pid;
    android_event_int_t uid;
    android_event_string_t process_name;
//  android_event_string_t type;
//  android_event_string_t component;
} android_event_am_proc_start;

}

int run_script(const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5){
    string BB = string(MAGISKTMP) + "/.magisk/busybox/busybox";
    int p_fork = fork();
    int status = 0;
    if (p_fork == 0){
        execl(BB.data(), "sh", arg1, arg2, arg3, arg4, arg5, (char*)0);
        _exit(1);
    } else if (p_fork > 0){
        waitpid(p_fork, &status, 0);
        return status;
    } else {
        return -1;
    }
}

void run_daemon(int pid, int uid, const char *process){
    if (fork_dont_care()==0){
        struct stat ppid_st, pid_st;
        char pid_str[10];
        char uid_str[10];
        snprintf(pid_str, 10, "%d", pid);
        snprintf(uid_str, 10, "%d", uid);
        int i=0;
        vector<string> module_run;
        // stop process
        kill(pid, SIGSTOP);
        for (auto i = 0; i < module_list.size(); i++){
            string script = string(MAGISKTMP) + "/.magisk/modules/"s + module_list[i] + "/dynmount.sh"s;
            if (access(script.data(), F_OK) != 0) continue;
            LOGI("run prepareEnterMntNs for pid %d: %s", pid, module_list[i].data());
            int ret = run_script(script.data(), "prepareEnterMntNs", pid_str, uid_str, process);
            LOGI("script %s exited with status %d", script.data(), ret);
            if (ret == 0) module_run.emplace_back(module_list[i]);
        }
        kill(pid, SIGCONT);
        if (module_run.size() < 1) {
            LOGI("no module to run EnterMntNs script for pid %d", pid);
            _exit(0);
        }
        do {
            if (i>=300000) _exit(0);
            if (read_ns(pid,&pid_st) == -1 ||
                read_ns(parse_ppid(pid),&ppid_st) == -1)
                _exit(0);
            usleep(10);
            i++;
        } while (pid_st.st_ino == ppid_st.st_ino &&
                pid_st.st_dev == ppid_st.st_dev);
        
        // stop process
        kill(pid, SIGSTOP);
        if (!switch_mnt_ns(pid)){
            for (auto i = 0; i < module_run.size(); i++){
                string script = string(MAGISKTMP) + "/.magisk/modules/"s + module_run[i] + "/dynmount.sh"s;
                // run script
                LOGI("run EnterMntNs for pid %d: %s", pid, module_run[i].data());
                int ret = run_script(script.data(), "EnterMntNs", pid_str, uid_str, process);
                LOGI("script %s exited with status %d", script.data(), ret);
            }
        }
        kill(pid, SIGCONT);
        _exit(0);
    }
}



void ProcessBuffer(struct logger_entry *buf) {
    auto *eventData = reinterpret_cast<const unsigned char *>(buf) + buf->hdr_size;
    auto *event_header = reinterpret_cast<const android_event_header_t *>(eventData);
    if (event_header->tag != 30014) return;
    auto *am_proc_start = reinterpret_cast<const android_event_am_proc_start *>(eventData);
    if (MAGISKTMP) {
        LOGI("proc_monitor: user=[%" PRId32"] pid=[%" PRId32"] uid=[%" PRId32"] process=[%.*s]\n",
           am_proc_start->user.data, am_proc_start->pid.data, am_proc_start->uid.data,
           am_proc_start->process_name.length, am_proc_start->process_name.data);
        run_daemon(am_proc_start->pid.data, am_proc_start->uid.data, am_proc_start->process_name.data);
    } else {
        printf("%" PRId32" %" PRId32" %" PRId32" %.*s\n",
           am_proc_start->user.data, am_proc_start->pid.data, am_proc_start->uid.data,
           am_proc_start->process_name.length, am_proc_start->process_name.data);
    }
}

[[noreturn]] void Run() {
    while (true) {
        bool first;
        __system_property_set("persist.log.tag", "");

        unique_ptr<logger_list, decltype(&android_logger_list_free)> logger_list{
            android_logger_list_alloc(0, 1, 0), &android_logger_list_free};
        auto *logger = android_logger_open(logger_list.get(), LOG_ID_EVENTS);
        if (logger != nullptr) [[likely]] {
            first = true;
        } else {
            continue;
        }

        struct log_msg msg{};
        while (true) {
            if (android_logger_list_read(logger_list.get(), &msg) <= 0) [[unlikely]] {
                break;
            }
            if (first) [[unlikely]] {
                first = false;
                continue;
            }

            ProcessBuffer(&msg.entry);
        }

        sleep(1);
    }
}

void kill_other(struct stat me){
    crawl_procfs([=](int pid) -> bool {
        struct stat st;
        char path[128];
        sprintf(path, "/proc/%d/exe", pid);
        if (stat(path,&st)!=0)
            return true;
        if (pid == myself)
            return true;
        if (st.st_dev == me.st_dev && st.st_ino == me.st_ino) {
            fprintf(stderr, "Killed: %d\n", pid);
            kill(pid, SIGKILL);
        }
        return true;
    });
}

void prepare_modules(){
    string MODULEDIR = string(MAGISKTMP) + "/.magisk/modules";
    DIR *dirfp = opendir(MODULEDIR.data());
    if (dirfp != nullptr){
        dirent *dp;
        while ((dp = readdir(dirfp))) {
            if (dp->d_name == "."sv || dp->d_name == ".."sv) continue;
            char buf[strlen(MODULEDIR.data()) + 1 + strlen(dp->d_name) + 20];
            snprintf(buf, sizeof(buf), "%s/%s/", MODULEDIR.data(), dp->d_name);
            char *z = buf + strlen(MODULEDIR.data()) + 1 + strlen(dp->d_name) + 1;
            strcpy(z, "disable");
            if (access(buf, F_OK) == 0) continue;
            strcpy(z, "remove");
            if (access(buf, F_OK) == 0) continue;
            LOGI("Magisk module: %s", dp->d_name);
            module_list.emplace_back(string(dp->d_name));
        }
        closedir(dirfp);
    }
}
            

int main(int argc, char *argv[]) {
    if (getuid()!=0) return 1;
    struct stat me;
    myself = self_pid();
    
    if (stat(argv[0],&me)!=0)
        return 1;

    if (argc > 1 && argv[1] == "--stop"sv) {
        kill_other(me);
        return 0;
    }
    if (argc > 1 && argv[1] == "--start"sv) {
        MAGISKTMP = "/sbin";
        if (argc > 2) MAGISKTMP = argv[2];
        kill_other(me);
        if (fork_dont_care()==0){
            fprintf(stderr, "New daemon: %d\n", self_pid());
            LOGI("MAGISKTMP is %s", MAGISKTMP);
            if (switch_mnt_ns(1))
                _exit(0);
            signal(SIGTERM, SIG_IGN);
            prepare_modules();
            Run();
            _exit(0);
        }
        return 0;
    }
    Run();
}
