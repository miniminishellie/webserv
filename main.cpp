#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "Request.hpp"
# define BUFF_SIZE 65536

int ReceiveWithoutBody(char *buf, int buf_size)
{
  
}

void ft_strjoin(std::string& str, const std::string& buf)
{
  str.append(buf);
}

int main(void) {
  char  buf[BUFF_SIZE + 1];
  int fd;
  int count = 0;
  std::string result;
  ssize_t read_size;

  if ((fd = open("./request.txt", O_RDONLY)) > 0) {
    while((read_size = read(fd, buf, BUFF_SIZE)) > 0) {
      buf[read_size] = '\0';
      if (count = ReceiveWithoutBody(buf, sizeof(buf)))

      ft_strjoin(result, buf);
    }
    close(fd);
  } else {
    std::cout << "파일 열기 실패\n";
  }

  return 0;
}

// 파일 읽어서 char* buf에 읽고 그 중 CRLF 찾아서 총 4바이트 
// 헤더 끝까지 찾아서 string으로 엮고
// parsing
