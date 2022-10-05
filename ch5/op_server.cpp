#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#define BUF_SIZE 1024
#define OPSZ 4
using namespace std;
int ysq(int len, int a[], char op);
void error_message(char const *message);
int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "use" << argv[0] << " <port>" << endl;
    exit(1);
  }

  int server_socket, client_socket;
  char message[BUF_SIZE];
  int str_len, i, result;
  int recv_cnt, recv_len;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_size;

  server_socket = socket(PF_INET, SOCK_STREAM, 0);

  if (server_socket == -1)  // link error
  {
    error_message("socket() error");
  }

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(atoi(argv[1]));

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    error_message("bind() error");
  }

  if (listen(server_socket, 5) == -1) {
    error_message("listen() error");
  }

  client_addr_size = sizeof(client_addr);

  for (i = 0; i < 5; i++) {
    str_len = 0;
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
                           &client_addr_size);
    if (client_socket == -1) {
      error_message("accept() error");
    } else {
      cout << "connected success client��" << i + 1 << endl;
    }
    read(client_socket, &str_len, 1);  // �����������Ϣ;
    cout << "str_len:" << str_len << endl;

    recv_len = 0;
    while ((str_len * OPSZ + 1) > recv_len) {
      recv_cnt = read(client_socket, &message[recv_len], BUF_SIZE - 1);
      recv_len += recv_cnt;
      cout << "recv_len:" << recv_len << ",recv_cnt:" << recv_cnt << endl;
    }
    result = ysq(str_len, (int *)message, message[recv_len - 1]);
    cout << "result:" << result << endl;
    write(client_socket, (char *)&result, sizeof(result));
    // while ((str_len = read(client_socket, message, BUF_SIZE)) != 0) {
    //   write(client_socket, message, str_len);
    // }
    close(client_socket);
  }
  close(server_socket);
  return 0;
}
int ysq(int len, int a[], char op) {
  int i, result = a[0];

  switch (op) {
    case '*':
      for (i = 1; i < len; i++) {
        result *= a[i];
      }
      break;
    case '-':
      for (i = 1; i < len; i++) {
        result -= a[i];
      }
      break;
    case '+':
      for (i = 1; i < len; i++) {
        result += a[i];
      }
      break;
    case '/':
      for (i = 1; i < len; i++) {
        result /= a[i];
      }
      break;
  }
  cout << "ok" << endl;
  return result;
}
void error_message(char const *message) {
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}