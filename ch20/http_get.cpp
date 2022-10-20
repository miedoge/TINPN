#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
using namespace std;
const int BUF_SIZE = 1024;
const int SMALL_MSG = 100;

void error_message(const string msg);
void send_data(FILE *fp, char *ct, char *file_name);
void send_error(FILE *fp);
void *request_handler(void *arg);
char *content_type(char *file);

int main(int argc, char *argv[]) {
  int server_sock, client_sock;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_size;
  char msg[BUF_SIZE];
  pthread_t t_id;

  if (argc != 2) {
    cout << "use:" << argv[0] << "PORT" << endl;
    exit(1);
  }

  server_sock = socket(PF_INET, SOCK_STREAM, 0);
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(atoi(argv[1]));

  if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1)
    error_message("bind() error");
  if (listen(server_sock, 20) == -1) error_message("listen() error");

  while (true) {
    client_addr_size = sizeof(client_addr);
    client_sock =
        accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
    cout << "connection request:" << inet_ntoa(client_addr.sin_addr)
         << ntohs(client_addr.sin_port) << endl;
    pthread_create(&t_id, NULL, request_handler, &client_sock);
    pthread_detach(t_id);
  }
  close(server_sock);
  return 0;
}

void error_message(const string msg) {
  cout << msg << endl;
  exit(1);
}

void *request_handler(void *arg) {
  int client_sock = *((int *)arg);  // 获取线程传递过来的客户端套接字.
  char req_line[SMALL_MSG];
  FILE *client_read;
  FILE *client_write;

  char method[10];
  char ct[15];
  char file_name[30];

  client_read = fdopen(client_sock, "r");        // 注意是fd open不是fopen
  client_write = fdopen(dup(client_sock), "w");  // dup 复制文件描述符用.
  fgets(req_line, SMALL_MSG, client_read);

  if (strstr(req_line, "HTTP/") == NULL)  //作用是返回字符串中首次出现子串的地址
  {
    send_error(client_write);  // 404
    fclose(client_read);
    fclose(client_write);
    return nullptr;
  }
  strcpy(
      method,
      strtok(req_line,
             " /"));  //函数返回被分解的最后一个子字符串 即去掉req_lien 中的 /
  strcpy(file_name, strtok(NULL, " /"));
  strcpy(ct, content_type(file_name));

  if (strcmp(method, "GET") != 0)  //暂时只支持get访问.
  {
    send_error(client_write);
    fclose(client_read);
    fclose(client_write);
    return nullptr;
  }
  fclose(client_read);
  send_data(client_write, ct, file_name);
}

void send_data(FILE *fp, char *ct, char *file_name) {
  char protocol[] = "HTTP/1.0 200 OK\r\n";
  char server[] = "Server:Linux Web Server \r\n";
  char client_len[] = "Conten-length:2048\r\n";
  char client_type[SMALL_MSG];
  char msg[BUF_SIZE];
  FILE *send_file;

  sprintf(client_type, "Content-type:%s\r\n\r\n", ct);
  send_file = fopen(file_name, "r");
  if (send_file == NULL)  // 404
  {
    send_error(fp);
    return;
  }

  // 传输http头信息.
  fputs(protocol, fp);
  fputs(server, fp);
  fputs(client_len, fp);
  fputs(client_type, fp);

  //传输request数据
  while (fgets(msg, BUF_SIZE, send_file) != NULL) {
    fputs(msg, fp);
    fflush(fp);  //将缓冲区的内容写入至文件中.
  }
  fflush(fp);
  fclose(fp);
}

char *content_type(char *file) {
  char extension[SMALL_MSG];
  char file_name[SMALL_MSG];
  strcpy(file_name, file);
  strtok(file_name, ".");
  strcpy(extension, strtok(NULL, "."));

  if (!strcmp(extension, "html") || !strcmp(extension, "htm")) {
    return "text/html";

  } else
    return "text/plain";
}

void send_error(FILE *fp) {
  char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
  char server[] = "Server:Linux Web Server \r\n";
  char client_len[] = "Content-length:2048\r\n";
  char client_type[] = "Content-type:text/html\r\n\r\n";
  char conten[] =
      "<html><head><title>NETWORK</title></head>"
      "<body><font size=+5><br>发生错误！ 查看请求文件名和请求方式!"
      "</font></body></html>";

  fputs(protocol, fp);
  fputs(server, fp);
  fputs(client_len, fp);
  fputs(client_type, fp);
  fflush(fp);
}