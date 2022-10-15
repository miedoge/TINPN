#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
const int BUF_SIZE = 30;
#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
  fd_set reads, temps;
  int res, str_len;
  char msg[BUF_SIZE];

  struct timeval tiemout;
  FD_ZERO(&reads);    //初始化变量
  FD_SET(0, &reads);  //将文件描述符0对应的位设置为1

  /*
  timeout.tv_sec=5;
  timeout.tv_usec=5000;
  */
  while (true) {
    temps = reads;  //为了防止调用selecet函数后，reads设置的位内容改变。
    tiemout.tv_sec = 5;   // 5s
    tiemout.tv_usec = 0;  // 0 ms
    res = select(1, &temps, 0, 0,
                 &tiemout);  //检测终端输入，如果终端输入了数据，则返回值大于0。
    if (res == -1) {
      cout << "select() error" << endl;
      break;
    } else if (res == 0) {
      cout << "time out" << endl;
    } else {
      if (FD_ISSET(0, &temps))  //验证发生变化的值是否是标准输入端，0为输入端 1
                                //为输出端 2为错误端
      {
        str_len = read(0, msg, BUF_SIZE);
        msg[str_len] = 0;
        cout << "FROM CONSOLE:" << msg;
      }
    }
  }
  return 0;
}