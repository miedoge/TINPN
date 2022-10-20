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
const int MAX_CLIENT = 256;

void error_message(const string msg);
void *handle_client(void *arg);
void send_msg(char *msg, int len);

int client_count = 0;
int client_sockets[MAX_CLIENT];  //定义一个数组存放client套接字.
pthread_mutex_t mutx;

int main(int argc, char *argv[]) {
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_size;
  pthread_t t_id;

  if (argc != 2) {
    cout << "use:" << argv[0] << "<port>" << endl;
    exit(1);
  }

  pthread_mutex_init(&mutx, NULL);  //创建互斥锁.

  server_sock = socket(PF_INET, SOCK_STREAM, 0);  // TCP
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[1]));

  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    error_message("bind() error");
  }

  if (listen(server_sock, 10) == -1) {
    error_message("listen() error");
  }

  while (true) {
    client_addr_size = sizeof(client_addr);
    client_sock =
        accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
    // 从这里开始 上锁
    pthread_mutex_lock(&mutx);
    client_sockets[client_count++] = client_sock;
    pthread_mutex_unlock(&mutx);

    pthread_create(&t_id, NULL, handle_client, (void *)&client_sock);
    //开启多线程为新客户端服务,同时传client_sock作为参数.
    pthread_detach(t_id);
    // 销毁线程 避免阻塞.
    cout << "connect client:" << inet_ntoa(client_addr.sin_addr) << endl;
  }
  close(server_sock);
  return 0;
}

void error_message(const string msg) {
  cout << msg << endl;
  exit(1);
}

void *handle_client(void *arg) {
  int client_sock = *((int *)arg);
  int str_len = 0;
  char msg[BUF_SIZE];

  while ((str_len = read(client_sock, msg, sizeof(msg))) != 0)
    send_msg(msg, str_len);
  pthread_mutex_lock(&mutx);
  for (int i = 0; i < client_count; i++) {
    if (client_sock == client_sockets[i]) {
      while (i++ < client_count - 1) {
        client_sockets[i] = client_sockets[i + 1];
      }
      break;
    }
  }
  client_count--;
  pthread_mutex_unlock(&mutx);
  close(client_sock);
  return NULL;
}

void send_msg(char *msg, int len) {
  pthread_mutex_lock(&mutx);
  for (int i = 0; i < client_count; i++) {
    write(client_sockets[i], msg, len);
  }
  pthread_mutex_unlock(&mutx);
}