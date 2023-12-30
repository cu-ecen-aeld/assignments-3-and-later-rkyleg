#pragma once
#include <pthread.h>
#include <stdbool.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

typedef struct thread_data{
    int client_fd;
    bool thread_completed;
    pthread_t* thread;
    pthread_mutex_t* mutex;
} thread_data;
