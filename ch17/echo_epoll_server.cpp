/* 12章的select服务器更改*/
#include <arpa/inet.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
using namespace std;

const int BUF_SIZE = 100;  // 根据effective c++ 条款,用const替代宏.
const int EPOLL_SIZE = 50;

void error_messages(const string message);

int main(int argc, char *argv[]) {
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;

  socklen_t addr_size;
  int str_len, i;
  char msg[BUF_SIZE];

  struct epoll_event *ep_events;
  struct epoll_event event;
  int epoll_fd, event_cnt;

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
  epoll_fd = epoll_create(
      EPOLL_SIZE);  // EPOLL_SIZE可以省略，自从linux 2.6.8后，操作系统默认忽略此参数。
  ep_events = (epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);
  event.events = EPOLLIN;  // 需要读取数据的事件
  event.data.fd = server_sock;

  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_sock, &event);
  //例程epfd 中添加文件描述符 serv_sock，目的是监听 enevt 中的事件
  while (true) {
    event_cnt = epoll_wait(epoll_fd, ep_events, EPOLL_SIZE, -1);
    // 获取改变的文件描述符,返回数量。
    if (event_cnt == -1) {
      cout << "epoll_wait() error" << endl;
      break;
    }
    for (i = 0; i < event_cnt; i++) {
      if (ep_events[i].data.fd == server_sock)  // 客户端发出请求连接
      {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr,
                             &addr_size);  //记得&解引用

        event.events = EPOLLIN;
        event.data.fd = client_sock;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &event);

        cout << "connect client:" << client_sock << endl;
      } else  //服务器向客户端返回信息.
      {
        str_len = read(ep_events[i].data.fd, msg, BUF_SIZE);

        if (str_len == 0)  //断开连接,即收到EOF
        {
          epoll_ctl(epoll_fd, EPOLL_CTL_DEL, ep_events[i].data.fd, 0);
          close(ep_events[i].data.fd);
          cout << "close client:" << ep_events[i].data.fd << endl;
        } else {
          write(ep_events[i].data.fd, msg,
                str_len);  //跟原来的差不多，来多少返回多少。
        }
      }
    }
  }

  close(server_sock);
  close(epoll_fd);
  return 0;
}

void error_messages(const string message) {
  cout << message << endl;
  exit(1);
}
