/* 基于多进程的服务器*/
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

const int BUF_SIZE = 30;  // 根据effective c++ 条款,用const替代宏.

void error_messages(const string message);
void read_child_proc(int sig);

int main(int argc, char *argv[]) {
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;

  pid_t pid;             // 进程ID
  struct sigaction act;  // sigaction函数.
  socklen_t addr_size;
  int str_len, state;
  char mes[BUF_SIZE];

  if (argc != 2) {
    cout << "use:" << argv[1] << "<port>" << endl;
    exit(1);
  }

  act.sa_handler = read_child_proc;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  state = sigaction(SIGCHLD, &act,
                    0);  // 注册信号处理器，注册成功后将数值返回给state

  server_sock = socket(PF_INET, SOCK_STREAM, 0);  // SERVER 套接字
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(atoi(argv[1]));

  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    error_messages("error bind()");
  }

  if (listen(server_sock, 5) == -1)  //侦听5个连接
  {
    error_messages("error listen()");
  }

  while (true) {
    addr_size = sizeof(client_addr);
    client_sock =
        accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
    if (client_sock == -1)
      continue;
    else
      cout << "有新的客户端连接到本服务器" << endl;
    pid = fork();   // 创建子进程，子进程也拥有一个套接字。
    if (pid == -1)  // fork失败
    {
      close(client_sock);
      error_messages("fork() failed");
      continue;
    }
    if (pid == 0)  // 此时此刻,子进程复制了原进程，然而他本身的pid=0,
    // 因此这里是子进程本身的运行区域，此部分向客户端提供echo服务。
    {
      close(server_sock);  //关闭服务器套接字，因为从父进程那边传递到了子进程
                           //此时关闭一个父进程套接字 并不是关闭双方互相连接。
      while ((str_len = read(client_sock, mes, BUF_SIZE)) != 0) {
        write(client_sock, mes, str_len);  //写多少返回多少.
      }
      close(client_sock);
      cout << "客户端断开连接." << endl;
      return 0;
    } else {
      close(client_sock);  //此时此刻，子进程关闭了复制过去的client
                           // sock,但是父进程的还没有关闭，因此需要在这里关闭。
    }
  }
  close(server_sock);
  return 0;
}

void error_messages(const string message) {
  cout << message << endl;
  exit(1);
}

void read_child_proc(int sig) {
  pid_t pid;
  int status;
  pid = waitpid(-1, &status, WNOHANG);
  cout << "removed proc id:" << pid << endl;
}