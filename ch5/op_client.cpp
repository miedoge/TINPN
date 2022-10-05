#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4
using namespace std;

void error_message(char const* messages);

int main(int argc, char* argv[]) {
  int sock, str_len;
  int op_cnt;
  int result;
  char message[BUF_SIZE];
  struct sockaddr_in server_addr;

  if (argc != 3) {
    cout << "use:" << argv[0] << "<port>" << endl;
    exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0);

  if (sock == -1) {
    error_message("socket() error");
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[2]));
  server_addr.sin_addr.s_addr = inet_addr(argv[1]);

  if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    error_message("error connect()");
  } else {
    cout << "connected succcesful" << endl;
  }
  cout << "请输入要参与计算的个数：" << endl;
  cin >> op_cnt;
  message[0] = (char)op_cnt;
  for (int i = 0; i < op_cnt; i++) {
    cout << "数字" << i << ":";
    scanf("%d", (int*)&message[i * OPSZ + 1]);
  }
  fgetc(stdin);
  cout << "输入操作符:" << endl;
  scanf("%c", &message[message[0] * OPSZ + 1]);
  write(sock, message, op_cnt * OPSZ + 2);
  read(sock, &result, RLT_SIZE);
  cout << "结果为:" << result << endl;
  close(sock);
  return 0;
}
void error_message(char const* message) {
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}