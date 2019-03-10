#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 1024
#define CMD_LEN 1024

/*Prototype*/
int server();

int main(){
  int STATUS = -1;
  STATUS = server();
  if(STATUS == 1)
    printf("Fatal Error! Please reset terminal session and try again !\n");
  else if(STATUS == 0)
    printf("Server Exited Successfully\n");

  return 0;
}

void success()
{
  printf("YEAH!\n");
}

void echo_msg(const char * msg)
{
  char cmd[128];

  /*if (strncmp("noecho", msg, 6))
  {
    strcpy(response, msg);
    response[strlen(msg)] = 0;
  }  
  else
  {
    strcpy(response, "ACK");
    response[4] = 0;
  }*/

  strcpy(cmd, msg);   /*unsafe memory operation >here<*/
}

int server(){

  struct sockaddr_in sin;
  char end = 0;
  unsigned int len;
  int s, new_s;
  char str[INET_ADDRSTRLEN];
  char response[MAX_LINE];
  char buf[MAX_LINE];

  /* build address data structure */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons(SERVER_PORT);
  /* setup passive open */
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("simplex-talk: socket");
    return 1;
  }
 
  inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
  printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

  if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
    perror("simplex-talk: bind");
    return 1;
  }
  else
    printf("Server bind done.\n");

  listen(s, MAX_PENDING);
  /* wait for connection, then receive and print text */
  while(!end) {
    if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
      perror("simplex-talk: accept");
      return 1;
    }
    printf("Server Listening.\n");
    while((len = recv(new_s, buf, sizeof(buf), 0)) && !end ){
      // printf("LEN: %d\n", len);
      printf("%s", buf);
      // clear_buf(buf,MAX_LINE);
      if(strncmp(buf,"exit", 4) == 0)
      {
        end = 1;
        break;
      }
      echo_msg(buf);
      send(new_s, "ACK",4, 0);
      //send(new_s, response, strlen(response), 0);
    }
    close(new_s);
  }
  return 0;
}

// Stack Protection 
// ASLR random
