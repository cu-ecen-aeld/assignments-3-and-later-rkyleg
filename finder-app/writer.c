#include <stdio.h>
#include <sys/syslog.h>
#include <syslog.h>
#include <stdlib.h>
int main(int argc, char** argv){
  openlog("writer", 0, LOG_USER);
  if (argc < 3){
    syslog(LOG_DEBUG, "please specify input text and a filename");
    // exit(1);
    return 1;
  }
  // printf("num args: %d\n", argc);
  // printf("text: %s\n", argv[1]);
  // printf("file: %s\n", argv[2]);
  syslog(LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);
    
  FILE* file = fopen(argv[1], "w");
  fputs(argv[2], file);
  fclose(file);
  closelog();
  return 0;
}
