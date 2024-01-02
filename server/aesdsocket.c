#include "aesdsocket.h"
#include "linked_list.h"
#include <asm-generic/socket.h>
#include <bits/pthreadtypes.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "9000"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_LEN 256
bool caught_sigint = false;

const Node* send_data(const Node* client) {
  int rc = pthread_mutex_lock(client->data->mutex);
  if ( rc != 0 ) {
      syslog(LOG_ERR,"pthread_mutex_lock failed with %d\n",rc);
      client->data->thread_completed = false;
  }
  
  FILE* file = fopen("/var/tmp/aesdsocketdata", "r");
  char buffer[BUFSIZ * 3];
  
  while (fgets(buffer, BUFSIZ * 3, file)) {
    if (send(client->data->client_fd, buffer, strcspn(buffer, "\n") + 1, 0) == -1) {
      syslog(LOG_ERR, "client send error: %s", strerror(errno));
    }
  }
  // char* line = NULL;
  // size_t len = 0;
  // ssize_t read;
  // while ((read = getline(&line, &len, file)) != -1) {
  //   // printf("Retrieved line of length %zu:\n", read);
  //   printf("line: %s", line);
  //   if (send(client_fd, line, len, 0) == -1){
  //     syslog(LOG_ERR,"client send error: %s", strerror(errno));
  //   }
  // }
  fclose(file);
  rc = pthread_mutex_unlock(client->data->mutex);
  if ( rc != 0 ) {
      syslog(LOG_ERR,"pthread_mutex_unlock failed with %d\n",rc);
      client->data->thread_completed = false;
  }
  return client;
}

static void signal_handler(const int signal_number) {
  if ((signal_number == SIGINT) || (signal_number == SIGTERM) || (signal_number == SIGKILL)) {
    syslog(LOG_ERR, "Caught signal, exiting");
    caught_sigint = true;
    remove("/var/tmp/aesdsocketdata");
    // exit(1);
  }
}

void* handle_client(void* thread_param) {
  struct Node* client = (struct Node*) thread_param;

  syslog(LOG_INFO,"Thread spawned to handle client connection on %d", client->data->client_fd);
  while (!caught_sigint) {
    char buffer[BUFSIZ * 3];

    // while((bytes_read = recv(client_fd, buffer, BUFSIZ*3, 0)) > 0 && !caught_sigint){
    const int bytes_read = recv(client->data->client_fd, buffer, BUFSIZ * 3, 0);
    // if (bytes_read == 0 || caught_sigint) {
    //   break;
    // }
    // printf("recvd: %s", buffer);
    if (buffer[bytes_read - 1] == '\n') {
      FILE* file = fopen("/var/tmp/aesdsocketdata", "a");
      fputs(buffer, file);
      fclose(file);
      send_data(client);
      break;
    }
  }
  // printf("closing fd: %i\n", client_fd);
  client->data->thread_completed = true;
  close(client->data->client_fd);
  return thread_param;
}

void* append_timestamp(void* thread_param){
  pthread_mutex_t* mutex = thread_param;
  while (true){
    int rc = pthread_mutex_lock(mutex);
    if ( rc != 0 ) {
        syslog(LOG_ERR,"timestamp err: pthread_mutex_lock failed with %d\n",rc);
    }
    // syslog(LOG_INFO, "%s",  "timestamp");
    char outstr[200];
    time_t t;
    struct tm *tmp;

    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL) {
       syslog(LOG_ERR, "localtime");
      return thread_param;
       // exit(EXIT_FAILURE);
    }

    if (strftime(outstr, sizeof(outstr), "timestamp: %a, %d %b %Y %T %z\n", tmp) == 0) {
      syslog(LOG_ERR, "strftime returned 0");
      return thread_param;
       // exit(EXIT_FAILURE);
    }
    FILE* file = fopen("/var/tmp/aesdsocketdata", "a");
    fputs(outstr, file);
    fclose(file);
    rc = pthread_mutex_unlock(mutex);
    if ( rc != 0 ) {
        syslog(LOG_ERR,"timestamp err: pthread_mutex_unlock failed with %d\n",rc);
    }
    usleep(10 * 1000000);
  }
  return thread_param;
}

int listen_for_clients(const int server_fd) {
  int client_fd;
  struct sockaddr client;

    if ((listen(server_fd, BACKLOG)) != 0) {
    syslog(LOG_ERR, "listen failed");
    return 1;
  }

  struct Node* clientList = NULL;
  pthread_mutex_t mutex;
  pthread_mutex_init(&mutex, NULL);

  pthread_t timestamp_thread;
  int rc = pthread_create(&timestamp_thread, NULL, append_timestamp, &mutex);
  if ( rc != 0 ) {
      syslog(LOG_ERR, "Failed to create timestamp thread, error was %d", rc);
      return 1;
  }
  while (!caught_sigint) {
      
    struct sockaddr_in address = {0};
    socklen_t addr_size;
    addr_size = sizeof(client);
    if ((client_fd = accept(server_fd, &client, &addr_size)) == -1) {
      syslog(LOG_ERR, "client accept failed: %s", strerror(errno));
      continue;
    }

    addr_size = sizeof(address);
    if ((getpeername(client_fd, (struct sockaddr *) &address, &addr_size)) == -1) {
      syslog(LOG_ERR, "client peername failed: %s", strerror(errno));
      return 1;
    }
    syslog(LOG_INFO, "Accepted a connection from: %s", inet_ntoa(address.sin_addr));

    pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));
    thread_data* params = (thread_data*)malloc(sizeof(thread_data));
    
    params->client_fd = client_fd;
    params->thread_completed = false;
    params->thread = thread;
    params->mutex = &mutex;

    struct Node* clientNode = (Node*)malloc(sizeof(Node));
    clientNode->data = params;
    clientNode->next = NULL;

    int rc = pthread_create(thread, NULL, handle_client, clientNode);
    if ( rc != 0 ) {
        syslog(LOG_ERR,"Failed to create thread, error was %d",rc);
        return 1;
    } else {
      if(clientList == NULL){
        clientList = clientNode;
      } else {
        insertAtEnd(&clientList, clientNode);
      }
    }
    // printList(clientList);
  }
  while (clientList!= NULL) {
    syslog(LOG_INFO,"joining thread for client fd: %d", clientList->data->client_fd);
    syslog(LOG_INFO, "status for cliend fd %d is %d", clientList->data->client_fd, clientList->data->thread_completed);
    // printf(" client fd: %d ", node->data->client_fd);
    pthread_join(*clientList->data->thread, NULL);
    free(clientList->data);
    clientList = clientList->next;
  }
  pthread_mutex_destroy(&mutex);
  close(server_fd);
  exit(0);
  return 0;
}

int main(const int argc, char** argv) {
  // int status;
  // bool start_program = true;
  bool do_fork = false;
  struct addrinfo hints;
  struct addrinfo* server;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  struct sigaction signals = {0};
  signals.sa_handler = signal_handler;

  if (sigaction(SIGINT, &signals, NULL) != 0) {
    syslog(LOG_ERR, "error registering sigint: %s", strerror(errno));
    // start_program = false;
    return 1;
  }
  if (sigaction(SIGTERM, &signals, NULL) != 0) {
    syslog(LOG_ERR, "error registering sigterm: %s", strerror(errno));
    // start_program = false;
    return 1;
  }
  // SIGKILL is not allowed
  // if (sigaction(SIGKILL, &signals, NULL) != 0) {
    // syslog(LOG_ERR, "error registering sigkill: %s", strerror(errno));
    // start_program = false;
    // return 1;
  // }
  if (getaddrinfo(NULL, PORT, &hints, &server) != 0) {
    syslog(LOG_ERR, "getaddrinfo failed: %s", strerror(errno));
    return 1;
  };
  const int server_fd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  if (server_fd == -1) {
    syslog(LOG_ERR, "socket failed");
    return 1;
  }
  int one = 1;
  setsockopt(server_fd,SOL_SOCKET ,SO_REUSEADDR ,&one ,sizeof(int));
  // syslog(LOG_INFO, "socket fd: %d", server_fd);
  if (bind(server_fd, server->ai_addr, server->ai_addrlen) != 0) {
    syslog(LOG_ERR, "bind failed: %s", strerror(errno));    // printf("bind failed: %s", strerror(errno));
    freeaddrinfo(server);
    return 1;
  }
  freeaddrinfo(server);
 
  if (argc > 1) {
    if (strcmp(argv[1], "-d") == 0) {
      do_fork = true;
    }
  }

  if (do_fork) {
    const int pid = fork();
    if (pid == 0) {

      listen_for_clients(server_fd);
    }
  } else {
    listen_for_clients(server_fd);
  }

}
  
