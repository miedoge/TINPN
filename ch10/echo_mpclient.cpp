#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#include <iostream>
#include <string>
using namespace std;
const int BUF_SIZE = 30;
void error_messages(const string message);
void read_routine(int sock, char *mes);
void write_routine(int sock, char *mes);

int main(int argc, char *argv[]) {
  int sock;
  pid_t pid;
  char mes[BUF_SIZE];
  struct sockaddr_in server_addr;

  if (argc != 3) {
    cout << "use:" << argv[0] << "<port>" << endl;
    exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0);
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_port = htons(atoi(argv[2]));

  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    error_messages("connect() error!");

  pid = fork();
  if (pid == 0)  // 子进程
  {
    write_routine(sock, mes);
  } else
    read_routine(sock, mes);
  close(sock);
  return 0;
}

void read_routine(int sock, char *mes) {
  while (true) {
    int str_len = read(sock, mes, BUF_SIZE);
    if (str_len == 0) return;
    mes[str_len] = 0;
    cout << "From server:" << mes << endl;
  }
}

void error_messages(const string message) {
  cout << message << endl;
  exit(1);
}

void write_routine(int sock, char *mes) {
  while (true) {
    fgets(mes, BUF_SIZE, stdin);
    if (!strcmp(mes, "q\n") || !strcmp(mes, "Q\n")) {
      shutdown(sock, SHUT_WR);  //向服务器端传递 EOF,因为fork函数复制了文件
                                //描述符，所以通过1次close调用不够
      return;
    }
    write(sock, mes, strlen(mes));
  }
}