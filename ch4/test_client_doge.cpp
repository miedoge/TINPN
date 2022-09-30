#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>

#define BUF_SIZE 1024
using namespace std;
void error_message(char const* message);

int main(int argc, char* argv[]) {
  int sock;
  char message[BUF_SIZE];
  int str_len;
  struct sockaddr_in ser_adr;

  if (argc != 3) {
    cout << "use:" << argv[0] << "<port>" << endl;
    exit(1);
  }

  sock = socket(PF_INET, SOCK_STREAM, 0);

  if (sock == -1) {
    error_message("socket() error ");
  }

  memset(&ser_adr, 0, sizeof(ser_adr));
  ser_adr.sin_family = AF_INET;
  ser_adr.sin_addr.s_addr = inet_addr(argv[1]);
  ser_adr.sin_port = htons(atoi(argv[2]));

  if (connect(sock, (struct sockaddr*)&ser_adr, sizeof(ser_adr)) == -1) {
    error_message("error connect()");
  } else {
    cout << "connected" << endl;
  }

  while (1) {
    fputs("input message (d to quit):", stdout);
    fgets(message, BUF_SIZE, stdin);

    if (!strcmp(message, "d\n") || !strcmp(message, "D\n")) {
      break;
    }

    write(sock, message, strlen(message));
    str_len = read(sock, message, BUF_SIZE - 1);
    message[str_len] = 0;
    cout << "message from server:" << message << endl;
  }
  close(sock);
  return 0;
}

void error_message(char const* message) {
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}