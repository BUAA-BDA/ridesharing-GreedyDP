#ifndef MONITOR_H
#define MONITOR_H

#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

struct program_t {
	struct timespec real;
	struct timespec proc;
};

void save_time(program_t& prog);
double calc_time(const program_t& st, const program_t& ed);
int get_proc_status(int pid, const char* mark);
void watchSolution(pid_t pid, int& usedMemory, int& usedTime, int limitTime);
void watchSolutionOnce(pid_t pid, int& usedMemory);


#endif
