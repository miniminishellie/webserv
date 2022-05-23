#include "Request.hpp"

Request::Request() {

}

Request::Request(const Request &request) {

}

// Request &Request::operator=(const Request &request) {

// }

Request::~Request() {

}

//getter
Request::Phase Request::get_m_phase_() const { return m_phase_; }

//setter
void Request::set_m_phase_(Phase phase) { m_phase_ = phase; }

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
  std::vector<std::string> parsed = Split(start_line, ' ');
  if (parsed.size() != 3)
    std::cout << "StartLine TOKEN _NUM ERROR" << std::endl;
  if (!ParseMethod(parsed[0]))
    std::cout << "StartLine METHOD ERROR" << std::endl;
  if (parsed[1].length() > REQUEST_URI_LIMIT_SIZE)
    std::cout << "StartLine URI_LENGTH ERROR" << std::endl;
  m_uri_ = parsed[1];
  m_uri_type_ = FILE;

  if ((m_protocol = parsed[2]) != "HTTP/1.1")
		throw (50501);
	m_special_header_count_ = 0;
  //URI parsing 해야함
}

bool Request::ParseStartLine(std::string &without_body) {
  size_t new_line;

  if ((new_line = without_body.find("\r\n")) != std::string::npos) {
    std::string start_line = without_body.substr(0, new_line);
    without_body = without_body.substr(new_line + 2);
    Parse(start_line);
    return true;
  }
  // else if (get_m_rbuf_from_client().size() > REQUEST_URI_LIMIT_SIZE_MAX)
  //   std::cout << "URI SIZE ERROR" << std::endl;
  return false;
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
  std::string key = Trim(header.substr(0, pos));
  std::string value = Trim(header.substr(pos + 1));

  for (size_t i = 0 ; i < key.length() ; ++i)
    key[i] = (i == 0 || key[i - 1] == '-') ? std::toupper(key[i]) : std::tolower(key[i]);
  std::pair<std::map<std::string, std::string>::iterator, bool> result = m_headers_.insert(std::make_pair(key, value));
  if (!result.second)
    std::cout << "Error: Duplicate Header" << std::endl;
  if (key == "Transfer-Encoding" && value.find("chunked") != std::string::npos)
    m_transfer_type_ = CHUNKED;
  if (key == "Content-Length") {
    m_content_length_ = ft_stoi(value);
    if (m_content_length_ > LIMIT_CLIENT_BODY_SIZE)
     std::cout << "Content length header value is over than body limit size" << std::endl;
    if (m_content_length_ < 0)
      std::cout << "Content-Length header value is less than 0" << std::endl;
  }
  if (key[0] == 'X')
    ++m_special_header_count_;

  // std::map<std::string, std::string>::iterator iter2;
  // for (iter2 = m_headers_.begin(); iter2 != m_headers_.end(); iter2++)
  //   std::cout << iter2->first << "," << iter2->second << std::endl;
}

int getLine(std::string& str, std::string &line) {
  size_t new_line;

  if ((new_line = str.find("\r\n")) != std::string::npos) {
    line = str.substr(0, new_line);
    str = str.substr(new_line + 2);
    return line.size();
  }
  else
    return -1;
}

bool Request::ParseHeader(std::string header) {
  std::string line;
  int index;
  while (((index = getLine(header, line)) <= header.size()) && index > 0) {
    if (!IsValidHeader(line)) {
      std::cout << "Error: Header is not Valid" << std::endl;
      return false;
    }
    AddHeader(line);
    line = "";
  }
  AddHeader(line);
  if (!hasKey(m_headers_,"Host")) {
    std::cout << "Error: Header does not have a Host" << std::endl;
    return false;
  }
  return true;
}

bool Request::IsRequestHasBody() {
  if (m_method_ == POST){
    if (m_transfer_type_ == CHUNKED)
      return (true);
    if (hasKey(m_headers_, "Content-Length") && m_content_length_ > 0)
      return (true);
  }
  return false;
}

int Request::ReceiveBody(int fd, char *buf, int buf_size, std::string& body) {
  int i = 0;
  int read_size = 0;

  if (m_method_ == POST && m_transfer_type_ == CHUNKED)
    return (0);
  if ((read_size = read(fd, buf, BUFF_SIZE)) > 0) {
    while(i < read_size) {
      if (buf[i] == '\r' && i + 3 < read_size && buf[i + 1] == '\n' && buf[i + 2] == '\r' && buf[i + 3] == '\n')
          break;
      i++;
    }
    if (i == read_size)
        return 0;
  }
  ft_str_index_join(body, buf, i + 4);
  if (read_size  == -1)
    std::cout << "READ ERROR/\n";
  return i;
}

void Request::AddContent(std::string added_content) {
	if (m_content_.size() + added_content.size() > LIMIT_CLIENT_BODY_SIZE)
		throw (41301);
	m_content_.append(added_content);
}

bool Request::ReadGeneralBody(std::string& body) {
  if (!hasKey(m_headers_, "Content-Length"))
    std::cout << "Content-Lenth required" << std::endl;
  // if (connection.get_m_readed_size() + body.size() <= m_content_length_) {
  //   AddContent(body);
  //   connection.set_m_readed_size(connection.get_m_readed_size() + body.size());
  //   connection.decreaseRbufFromClient(body.size());
  // }
  // else {
  //   std::string part = body.substr(0, m_content_length_ - connection.get_m_readed_size());
  //   AddContent(part);
  //   //connection.decreaseRbufFromClient(part.size());
  //   connection.set_m_readed_size(m_content_length_);
  // }
  // return (connection.get_m_readed_size() == m_content_length_);
}

int getChunkedSize(std::string& str, std::string& len) {
  int content_length;

  if (!getNewLine(str, len))
			return (-1);
		try {
			content_length = ft_stoi(len, 16);
		} catch (std::exception& e) {
			throw (40017);
		}
		if (content_length < 0)
			throw (40016);
		if (content_length == 0)
		{
			if (len[0] != '0')
				throw (40017);
		}
		return (content_length);
	}

bool Request::ReadChunkedBody(std::string& body) {
  while (true) {
    std::string len;
    int content_length = getChunkedSize(body, len);
    if (content_length == -1)
      return false;
    if (content_length == 0) {
      if (body.find("\r\n") == std::string::npos) {
        body.insert(0, len + "\r\n");
        return false;
      }
      if (body.size() >= 2 && body[0] == '\r' && body[1] == '\n') {
        //connection.decreaseRbufFromClient(2);
        return true;
      }
        throw (40018);
    }
    if (body.size() < content_length + 2) {
      body.insert(0, len + "\r\n");
      return false;
    }
    if (body.substr(content_length, 2) != "\r\n")
      throw (40021);
    AddContent(body.substr(0, content_length));
    //connection.decreaseRbufFromClient(content_length + 2);
  }
}

bool Request::ParseBody(std::string body) {
	if (m_method_ == POST && m_transfer_type_ == CHUNKED)
		return true;
	if (m_transfer_type_ == GENERAL)
		return (ReadGeneralBody(body));
	if (m_transfer_type_ == CHUNKED)
		return (ReadChunkedBody(body));
	return false;
}

void Request::ReceiveRequest(std::string& without_body) {
  char buf[BUFF_SIZE + 1];
  int fd;
  int count = 0;
  std::string body;

  if (m_phase_ == Request::READY && ParseStartLine(without_body))
    m_phase_ = Request::ON_HEADER;
  if (m_phase_ == Request::ON_HEADER && ParseHeader(without_body)) {
    m_phase_ = Request::ON_BODY;
    if (!IsRequestHasBody())
      return ;
  }
  if (m_phase_ == Request::ON_BODY && (fd = open("./request.txt", O_RDONLY)) > 0  && (count = ReceiveBody(fd, buf, sizeof(buf), body)) > 0) {
    ft_strjoin(body, buf, count);
    close(fd);
  }
  if (m_phase_ == Request::ON_BODY && ParseBody(body))
    m_phase_ = COMPLETE;
  if (m_phase_ == Request::COMPLETE)
    ;//set_m_last_request_at();
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
  request.set_m_phase_(phase);
  request.ReceiveRequest(without_body);
  return 0;
}
