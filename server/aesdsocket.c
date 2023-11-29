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

#define PORT "9000"  // the port users will be connecting to
#define BACKLOG 10     // how many pending connections queue will hold
#define MAX_LEN 256
bool caught_sigint = false;

void send_data(int client_fd) {
  FILE* file = fopen("/var/tmp/aesdsocketdata", "r");
  // fgets(buffer, BUFSIZ, file);
  char buffer[BUFSIZ * 3];
  while (fgets(buffer, BUFSIZ * 3, file)) {
    // Remove trailing newline
    // buffer[strcspn(buffer, "\n")] = 0;
    // printf("%s\n", /xbuffer);
    if (send(client_fd, buffer, strcspn(buffer, "\n") + 1, 0) == -1) {
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
}

static void signal_handler(int signal_number) {
  if ((signal_number == SIGINT) || (signal_number == SIGTERM) || (signal_number == SIGKILL)) {
    syslog(LOG_ERR, "Caught signal, exiting");
    caught_sigint = true;
    remove("/var/tmp/aesdsocketdata");
    exit(1);
  }
}

void handle_client(const int client_fd) {
  while (!caught_sigint) {
    char buffer[BUFSIZ * 3];

    // while((bytes_read = recv(client_fd, buffer, BUFSIZ*3, 0)) > 0 && !caught_sigint){
    const int bytes_read = recv(client_fd, buffer, BUFSIZ * 3, 0);
    // if (bytes_read == 0 || caught_sigint) {
    //   break;
    // }
    // printf("recvd: %s", buffer);
    if (buffer[bytes_read - 1] == '\n') {
      FILE* file = fopen("/var/tmp/aesdsocketdata", "a");
      fputs(buffer, file);
      fclose(file);
      send_data(client_fd);
      break;
    }
  }
  // printf("closing fd: %i\n", client_fd);
  close(client_fd);
  exit(0);
}

int listen_for_clients(const int server_fd) {
  int client_fd;
  struct sockaddr client;

  if ((listen(server_fd, BACKLOG)) != 0) {
    syslog(LOG_ERR, "listen failed");
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

    const int child_pid = fork();
    if (child_pid == 0) {
      // this is the child process
      // close(server_fd); // child doesn't need the listener
      handle_client(client_fd);
    } else {
      waitpid(child_pid, NULL, 0);
    }
    // close(client_fd); // parent doesn't need this
    // exit(0);
  }
  close(server_fd);
  return 0;
}

int main(int argc, char** argv) {
  int status;
  bool start_program = true;
  bool do_fork = false;
  struct addrinfo hints;
  struct addrinfo* server;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  struct sigaction signals = {0};
  signals.sa_handler = signal_handler;

  if (sigaction(SIGINT, &signals, NULL) != 0) {
    syslog(LOG_ERR, "error registering sigint: %s", strerror(errno));
    start_program = false;
  }
  if (sigaction(SIGTERM, &signals, NULL) != 0) {
    syslog(LOG_ERR, "error registering sigterm: %s", strerror(errno));
    start_program = false;
  }
  if (sigaction(SIGKILL, &signals, NULL) != 0) {
    syslog(LOG_ERR, "error registering sigkill: %s", strerror(errno));
    start_program = false;
  }
  if ((status = getaddrinfo(NULL, PORT, &hints, &server)) != 0) {
    syslog(LOG_ERR, "getaddrinfo failed");
    return 1;
  };
  const int server_fd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
  if (server_fd == -1) {
    syslog(LOG_ERR, "socket failed");
    return 1;
  }
  syslog(LOG_INFO, "socket fd: %d", server_fd);
  if ((status = bind(server_fd, server->ai_addr, server->ai_addrlen)) != 0) {
    syslog(LOG_ERR, "bind failed: %s", strerror(errno));
    // printf("bind failed: %s", strerror(errno));
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
  
