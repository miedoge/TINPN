#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#define BUF_SIZE 1024
using namespace std;
void error_message(char const *message);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "use:" << argv[0] << "<port>" << endl;
    exit(1);
  }

  int server_socket, client_socket;
  char message[BUF_SIZE];
  int str_len, i;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_size;

  server_socket = socket(PF_INET, SOCK_STREAM, 0);

  if (server_socket == -1) {
    error_message("socket() error");
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;  // TCP
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(atoi(argv[1]));  //字符串转换成整型数 port
  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    error_message("bind()error");
  }

  if (listen(server_socket, 5) == -1)  // max user = 5
  {
    error_message("listen() error");
  }

  // client

  client_addr_size = sizeof(client_addr);

  // key
  for (i = 0; i < 5; i++) {
    cout << "doge!" << endl;
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
                           &client_addr_size);
    cout << "client_socket" << endl;
    if (client_socket == -1) {
      error_message("error accept()");
    } else {
      cout << "connected client" << i + 1 << endl;
    }
    while ((str_len = read(client_socket, message, BUF_SIZE)) != 0)
      write(client_socket, message, str_len);

    close(client_socket);
  }
  close(server_socket);
  return 0;
}

void error_message(char const *message) {
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}