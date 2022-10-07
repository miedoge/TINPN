#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
using namespace std;
#define BUF_SIZE 1024

void error_messages(char const *message);

int main(int argc, char *argv[]) {
  int client_sock;
  char message[BUF_SIZE];
  int str_len;

  socklen_t addr_size;
  struct sockaddr_in server_addr, client_addr;

  if (argc != 3) {
    cout << "use:" << argv[0] << "<prot>" << endl;
    exit(1);
  }

  client_sock = socket(PF_INET, SOCK_DGRAM, 0);
  if (client_sock == -1) {
    error_messages("error client sokcet()");
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));

  while (1) {
    fputs("输入信息，输入q退出服务连接", stdout);
    fgets(message, sizeof(message), stdin);
    if (!strcmp(message, "Q\n") || !strcmp(message, "q\n")) {
      break;
    }
    sendto(client_sock, message, strlen(message), 0,
           (struct sockaddr *)&server_addr, sizeof(server_addr));
    addr_size = sizeof(client_addr);
    str_len = recvfrom(client_sock, message, BUF_SIZE, 0,
                       (struct sockaddr *)&client_addr, &addr_size);

    message[str_len] = 0;
    cout << "message：" << message << endl;
  }
  close(client_sock);
  return 0;
}

void error_messages(char const *message) {
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}