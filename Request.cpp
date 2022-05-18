#include "Request.hpp"

std::vector<std::string> split(std::string s, char c) {
  std::vector<std::string> result;
  size_t pos = 0;
  while ((pos = s.find(c)) != std::string::npos) {
    if (pos != 0)
      result.push_back(s.substr(0, pos));
    s.erase(0, pos + 1);
  }
  if (s.length() != 0)
    result.push_back(s);
  return result;
}

Request::Request() {

}

Request::Request(std::string start_line){
  m_phase_ = ON_HEADER;
  std::vector<std::string> parsed = split(start_line, ' ');
  if (parsed.size() != 3)
    std::cout << "StartLine TOKEN _NUM ERROR" << std::endl;
  if (!ParseMethod(parsed[0]))
    std::cout << "StartLine METHOD ERROR" << std::endl;
  if (parsed[1].length() > REQUEST_URI_LIMIT_SIZE)
    std::cout << "StartLine URI_LENGTH ERROR" << std::endl;
  m_uri_ = parsed[1];
}

Request::Request(const Request &request) {

}

// Request &Request::operator=(const Request &request) {

// }

Request::~Request() {

}

bool Request::ParseMethod(std::string method)
{

}

void ft_strjoin(std::string& str, const std::string& buf, size_t n) {
  str.append(buf.c_str(), n);
}

int Request::ReceiveWithoutBody(int fd, char *buf, int buf_size) {
  int i = 0;
  int read_size = 0;
  if ((read_size = read(fd, buf, BUFF_SIZE)) > 0) {
      while(i < read_size) {
        if (buf[i] == '\r' && i + 3 < read_size && buf[i + 1] == '\n' && buf[i + 2] == '\r' && buf[i + 3] == '\n')
          break;
        i++;
      }
      if (i == read_size)
        return 0;
      else
        return i + 4;
  }
  if (read_size  == -1)
    std::cout << "READ ERROR/\n";
  else
    std::cout << "EOF/\n";
  return -1;
}

bool Request::ParseStartLine(Request& request, std::string& without_body) {
  size_t new_line;
  if ((new_line = without_body.find("\r\n")) != std::string::npos) {
    std::string start_line = without_body.substr(0, new_line);
    request = Request(start_line);
    std::cout << start_line << std::endl;
    std::cout << without_body << std::endl;
  }
  return true;
}

int main(void) {
  char  buf[BUFF_SIZE + 1];
  int fd;
  int count = 0;
  int i = 0;
  Request::Phase phase = Request::READY;
  Request request;
  std::string without_body;

  if ((fd = open("./request.txt", O_RDONLY)) > 0 && phase == Request::READY && (count = request.ReceiveWithoutBody(fd, buf, sizeof(buf))) > 0) {
    ft_strjoin(without_body, buf, count);
    close(fd);
  }
  else {
    std::cout << "ERROR/\n";
  }
  if (phase == Request::READY && request.ParseStartLine(request, without_body))
    phase = Request::ON_HEADER;

  return 0;
}
