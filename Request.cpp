#include "Request.hpp"

Request::Request() {

}

Request::Request(const Request &request) {

}

// Request &Request::operator=(const Request &request) {

// }

Request::~Request() {

}

bool Request::ParseMethod(std::string method) {
  if (method == "GET")
    m_method_ = GET;
  else if (method == "POST")
    m_method_ = POST;
  else if (method == "DELETE")
    m_method_ = DELETE;
  else 
    return false;
  return true;
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

void Request::Parse(std::string start_line) {
  m_phase_ = ON_HEADER;
  std::vector<std::string> parsed = ft::Split(start_line, ' ');
  if (parsed.size() != 3)
    std::cout << "StartLine TOKEN _NUM ERROR" << std::endl;
  if (!ParseMethod(parsed[0]))
    std::cout << "StartLine METHOD ERROR" << std::endl;
  if (parsed[1].length() > REQUEST_URI_LIMIT_SIZE)
    std::cout << "StartLine URI_LENGTH ERROR" << std::endl;
  m_uri_ = parsed[1];
  m_uri_type_ = FILE;
  m_protocol = parsed[2];
  //URI parsing 해야함
}

bool Request::ParseStartLine(std::string &without_body) {
  size_t new_line;

  if ((new_line = without_body.find("\r\n")) != std::string::npos) {
    std::string start_line = without_body.substr(0, new_line);
    //std::cout << without_body;
    without_body = without_body.substr(new_line + 2);
    //std::cout << without_body;
    Parse(start_line);
    return true;
  }
  // else if (get_m_rbuf_from_client().size() > REQUEST_URI_LIMIT_SIZE_MAX)
  //   std::cout << "URI SIZE ERROR" << std::endl;
  return false;
}

int GetLine(std::string& header, std::string &line) {
  size_t new_line;

  if ((new_line = header.find("\r\n")) != std::string::npos) {
    line = header.substr(0, new_line);
    header = header.substr(new_line + 2);
    return line.size();
  }
  else
    return -1;
}

bool Request::IsValidHeader(std::string header) {
  if (header.size() > REQUEST_HEADER_LIMIT_SIZE) {
    std::cout << "Error: Header Size is over than limit size" << std::endl;
    return false;
  }
  if (header.find(":") == std::string::npos) {
    std::cout << "Error: Header doesn't have\":\"" << std::endl;
    return false;
  }
  return true;
}

void Request::AddHeader(std::string header) {
  size_t pos = header.find(":");
  std::string key = ft::Trim(header.substr(0, pos));
  std::string value = ft::Trim(header.substr(pos + 1));

  for (size_t i = 0 ; i < key.length() ; ++i)
    key[i] = (i == 0 || key[i - 1] == '-') ? std::toupper(key[i]) : std::tolower(key[i]);
  std::pair<std::map<std::string, std::string>::iterator, bool> result = m_headers_.insert(std::make_pair(key, value));
  if (!result.second)
    std::cout << "Error: Duplicate Header" << std::endl;
  if (key == "Transfer-Encoding" && value.find("chunked") != std::string::npos)
    m_transfer_type_ = CHUNKED;
  if (key == "Content-Length") {
    int content_length = ft::stoi(value);
    if (content_length > LIMIT_CLIENT_BODY_SIZE)
     std::cout << "Content length header value is over than body limit size" << std::endl;
    if (content_length < 0)
      std::cout << "Content-Length header value is less than 0" << std::endl;
  }
  if (key[0] == 'X')
    ++m_special_header_count_;
  std::map<std::string, std::string>::iterator iter;
  for (iter = m_headers_.begin(); iter != m_headers_.end(); iter++)
    std::cout << iter->first << "," << iter->second << std::endl;
}

bool Request::ParseHeader(std::string header) {
  std::string line;
  int index;

  while (((index = GetLine(header, line)) <= header.size()) && index > 0) {
    if (!IsValidHeader(line)) {
      std::cout << "Error: Header is not Valid" << std::endl;
      return false;
    }
    AddHeader(line);
  }
  if (HasKey(m_headers_,"Host")) {
    std::cout << "Error: Header does not have a Host" << std::endl;
    return false;
  }
  return true;
}

bool Request::IsRequestHasBody(Request& request) {
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
    ft::ft_strjoin(without_body, buf, count);
    close(fd);
  }
  else {
    std::cout << "ERROR/\n";
  }
  if (phase == Request::READY && request.ParseStartLine(without_body))
    phase = Request::ON_HEADER;
  if (phase == Request::ON_HEADER && request.ParseHeader(without_body))
    phase = Request::ON_BODY;
  if (request.IsRequestHasBody(request) && phase == Request::ON_BODY) {

  }

  return 0;
}
