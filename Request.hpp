#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <vector>

# define BUFF_SIZE 65536
# define REQUEST_URI_LIMIT_SIZE 1024

class Request
{
 public:
  enum Phase {
    READY,
    ON_HEADER,
    ON_BODY,
    COMPLETE
  };
  enum Method {
    DEFAULT,
    GET,
    HEAD,
    POST,
    DELETE
  };
  Request();
  Request(std::string start_line);
  Request(const Request &request);
	Request &operator=(const Request &request);
  ~Request();
  int ReceiveWithoutBody(int fd, char *buf, int buf_size);
  bool ParseStartLine(Request& request, std::string& str);
  bool Request::ParseMethod(std::string method);

  std::string without_body;

  //getter
  //std::string get_without_body() const;
 private:
  Phase m_phase_;
  std::string m_uri_;

};

void ft_strjoin(std::string& str, const std::string& buf, size_t n);



#endif