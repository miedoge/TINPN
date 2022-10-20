#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
using namespace std;

const int BUF_SIZE = 100;
const int NAME_SIZE = 20;

void error_message(const string msg);
void *send_msg(void *arg);
void *recv_msg(void *arg);

char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char *argv[]) {
  int client_sock;
  struct sockaddr_in server_addr;
  pthread_t send_thread, recv_thread;
  void *thread_return;

  if (argc != 4) {
    cout << "use"
         << "<PROT>" << endl;
    exit(1);
  }

  sprintf(name, "[%s]", argv[3]);

  client_sock = socket(PF_INET, SOCK_STREAM, 0);

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));

  if (connect(client_sock, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) == -1) {
    error_message("connect() error!");
  }

  pthread_create(&send_thread, NULL, send_msg, (void *)&client_sock);
  // 创建发送消息线程.
  pthread_create(&recv_thread, NULL, recv_msg, (void *)&client_sock);
  // 创建接受消息线程.

  pthread_join(send_thread, &thread_return);
  pthread_join(recv_thread, &thread_return);
  // 销毁线程.

  return 0;
}

void *send_msg(void *arg) {
  int sock = *((int *)arg);
  char name_msg[NAME_SIZE + BUF_SIZE];
  while (true) {
    fgets(msg, BUF_SIZE, stdin);  // from conselog
    if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
      close(sock);
      exit(0);
    }
    sprintf(name_msg, "%s %s", name, msg);
    write(sock, name_msg, strlen(name_msg));
  }
  return NULL;
}

void *recv_msg(void *arg) {
  int sock = *((int *)arg);
  char name_msg[NAME_SIZE + BUF_SIZE];
  int str_len;
  while (true) {
    str_len = read(sock, name_msg, NAME_SIZE + BUF_SIZE - 1);
    if (str_len == -1) {
      return (void *)-1;
    }
    name_msg[str_len] = 0;
    fputs(name_msg, stdout);
  }
  return NULL;
}

void error_message(const string msg) {
  cout << msg << endl;
  exit(1);
}
