#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 1024
void error_message(char const *messages);
int main(int argc, char *argv[]) {
  int server_sock;
  char message[BUF_SIZE];
  int str_len;
  socklen_t client_addr_size;
  struct sockaddr_in server_addr, client_addr;

  if (argc != 2) {
    cout << "use:" << argv[0] << "<port>" << endl;
    exit(1);
  }

  server_sock = socket(PF_INET, SOCK_DGRAM, 0);
  if (server_sock == -1) {
    error_message("error UPD_socket");
  }

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(atoi(argv[1]));

  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    error_message("bind() error");
  }

  while (1) {
    client_addr_size = sizeof(client_addr);
    str_len = recvfrom(server_sock, message, BUF_SIZE, 0,
                       (struct sockaddr *)&client_addr, &client_addr_size);

    sendto(server_sock, message, str_len, 0, (struct sockaddr *)&client_addr,
           client_addr_size);
    }
  close(server_sock);
  return 0;
}

void error_message(char const *messages) {
  fputs(messages, stderr);
  fputc('\n', stderr);
  exit(1);
}