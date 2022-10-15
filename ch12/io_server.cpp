/* 基于I/O复用的服务器*/
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include <iostream>
#include <string>
using namespace std;

const int BUF_SIZE = 100;  // 根据effective c++ 条款,用const替代宏.

void error_messages(const string message);

int main(int argc, char *argv[]) {
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  struct timeval timeout;
  fd_set reads, temps;

  socklen_t addr_size;
  int fd_max, str_len, fd_num, i;
  char msg[BUF_SIZE];

  if (argc != 2) {
    cout << "use:" << argv[0] << "<port>" << endl;
    exit(1);
  }

  server_sock = socket(PF_INET, SOCK_STREAM, 0);
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[1]));

  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    error_messages("bind() error");
  }

  if (listen(server_sock, 5) == -1) {
    error_messages("listen() error");
  }

  FD_ZERO(&reads);
  FD_SET(server_sock, &reads);  // 注册server套接字
  fd_max = server_sock;
  cout << "hello world" << endl;

  while (true) {
    temps = reads;
    timeout.tv_sec = 5;
    timeout.tv_usec = 5000;

    if ((fd_num = select(fd_max + 1, &temps, 0, 0, &timeout)) == -1) break;
    if (fd_num == 0) continue;

    for (i = 0; i < fd_max + 1; i++) {
      if (FD_ISSET(i, &temps))  //查找发生变化的套接字fd（file des
      {
        if (i == server_sock) {
          addr_size = sizeof(client_addr);
          client_sock =
              accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);

          FD_SET(client_sock, &reads);  //注册客户端套接字

          if (fd_max < client_sock) {
            fd_max = client_sock;
          }

          cout
              << "connect client:" << client_sock
              << endl;  // 记得cout缓存区问题，这里如果不换行，将无法输出连接信息。
        } else {        //也就是客户端给服务器传递消息了。

          str_len = read(i, msg, BUF_SIZE);
          if (str_len == 0) {
            FD_CLR(i, &reads);
            close(i);
            printf("closed client: %d \n", i);
            cout
                << "close client:" << i
                << endl;  // 记得cout缓存区问题，这里如果不换行，将无法输出断开连接信息。
          } else {
            write(i, msg, str_len);
          }
        }
      }
    }
  }
  close(server_sock);
  return 0;
}

void error_messages(const string message) {
  cout << message << endl;
  exit(1);
}
