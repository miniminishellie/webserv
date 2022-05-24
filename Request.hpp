#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "libft.hpp"
#include <map>

# define BUFF_SIZE 65536
# define REQUEST_URI_LIMIT_SIZE 1024
# define REQUEST_URI_LIMIT_SIZE_MIN 64
# define REQUEST_URI_LIMIT_SIZE_MAX 8192
# define REQUEST_HEADER_LIMIT_SIZE 8192
# define LIMIT_CLIENT_BODY_SIZE 200000000

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
  enum URIType { DIRECTORY, FILE, CGI };
  enum TransferType { GENERAL, CHUNKED };
  Request();
  Request(const Request &request);
	Request &operator=(const Request &request);
  ~Request();
  int ReceiveWithoutBody(int fd, char *buf, int buf_size);
  bool ParseStartLine(std::string &without_body);
  void Parse(std::string start_line);
  bool ParseMethod(std::string method);
  bool ParseHeader(std::string header);
  void AddHeader(std::string header);
  bool IsValidHeader(std::string header);
  bool IsRequestHasBody();
  void ReceiveRequest(std::string& without_body);
  int ReceiveBody(int fd, char *buf, int buf_size, std::string& body);
  bool ParseBody(std::string body);
  bool ReadGeneralBody(std::string &body);
  bool ReadChunkedBody(std::string &body);
  void AddContent(std::string added_content);
  // std::string without_body;

  //getter
  //std::string get_without_body() const;
  Phase get_m_phase_() const;

  //setter
  void set_m_phase_(Phase phase);

 private:
  Phase m_phase_;
  Method m_method_;
  URIType m_uri_type_;
  TransferType m_transfer_type_;
  std::string m_uri_;
  std::string m_protocol;
  std::map<std::string, std::string> m_headers_;
  int m_special_header_count_;
  std::string m_content_;
  int m_content_length_;
};


template <typename T, typename V>
  bool hasKey(T container, V value) { return (container.find(value) != container.end()); }

#endif