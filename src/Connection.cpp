/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/25 21:44:42 by jihoolee          #+#    #+#             */
/*   Updated: 2022/06/03 21:20:20 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "ServerConfig.hpp"
#include "WebservConfig.hpp"
#include "ServerManager.hpp"
#include "Libft.hpp"

Connection::Connection(void)
    : m_server_manager_(NULL),
      m_webserv_config_(NULL),
      m_server_config_(NULL),
      m_status_(ON_WAIT),
      m_client_fd_(-1),
      m_client_ip_(),
      m_client_port_(-1),
      m_read_from_server_fd_(-1),
      m_write_to_server_fd_(-1),
      m_wbuf_(),
      m_readed_size_(0),
      m_read_buffer_client_(),
      m_request_(),
      m_response_() {
  this->m_last_request_at_.tv_sec = 0;
  this->m_last_request_at_.tv_usec = 0;
  set_m_last_request_at();
}

Connection::Connection(ServerManager* sm, ServerConfig* sc, int client_fd,
              std::string& client_ip, int client_port)
    : m_server_manager_(sm),
      m_webserv_config_(&sm->get_m_config()),
      m_server_config_(sc),
      m_status_(ON_WAIT),
      m_client_fd_(client_fd),
      m_client_ip_(client_ip),
      m_client_port_(client_port),
      m_read_from_server_fd_(-1),
      m_write_to_server_fd_(-1),
      m_wbuf_(),
      m_readed_size_(0),
      m_read_buffer_client_(),
      m_request_(),
      m_response_() {
  this->m_last_request_at_.tv_sec = 0;
  this->m_last_request_at_.tv_usec = 0;
  set_m_last_request_at();
}

Connection::Connection(const Connection &c)
    : m_server_manager_(c.m_server_manager_),
      m_webserv_config_(c.m_webserv_config_),
      m_server_config_(c.m_server_config_),
      m_status_(c.m_status_),
      m_client_fd_(c.m_client_fd_),
      m_last_request_at_(c.m_last_request_at_),
      m_client_ip_(c.m_client_ip_),
      m_client_port_(c.m_client_port_),
      m_read_from_server_fd_(c.m_read_from_server_fd_),
      m_write_to_server_fd_(c.m_write_to_server_fd_),
      m_wbuf_(c.m_wbuf_),
      m_readed_size_(c.m_readed_size_),
      m_read_buffer_client_(c.m_read_buffer_client_),
      m_request_(c.m_request_),
      m_response_(c.m_response_) {
}

Connection &Connection::operator=(const Connection &operand) {
  if (this == &operand)
    return *this;
  m_server_manager_ = operand.m_server_manager_;
  m_webserv_config_ = operand.m_webserv_config_;
  m_server_config_ = operand.m_server_config_;
  m_status_ = operand.m_status_;
  m_client_fd_ = operand.m_client_fd_;
  m_last_request_at_ = operand.m_last_request_at_;
  m_client_ip_ = operand.m_client_ip_;
  m_client_port_ = operand.m_client_port_;
  m_read_from_server_fd_ = operand.m_read_from_server_fd_;
  m_write_to_server_fd_ = operand.m_write_to_server_fd_;
  m_wbuf_ = operand.m_wbuf_;
  m_readed_size_ = operand.m_readed_size_;
  m_read_buffer_client_ = operand.m_read_buffer_client_;
  m_request_ = operand.m_request_;
  m_response_ = operand.m_response_;
  return *this;
}

Connection::~Connection(void) {}

/*getter function*/
ServerConfig* Connection::get_m_server_config() const { return m_server_config_; }
Connection::Status Connection::get_m_status() const { return m_status_; }
int Connection::get_m_client_fd() const { return m_client_fd_; }
timeval Connection::get_m_last_request_at() const { return m_last_request_at_; }
std::string Connection::get_m_client_ip() const { return m_client_ip_; }
int Connection::get_m_client_port() const { return m_client_port_; }
int Connection::get_m_read_from_server_fd() const { return m_read_from_server_fd_; }
int Connection::get_m_write_to_server_fd() const { return m_write_to_server_fd_; }
int Connection::get_m_readed_size() const { return m_readed_size_; }
std::string Connection::get_m_read_buffer_client() const { return m_read_buffer_client_; }
const Request& Connection::get_m_request() const { return m_request_; }

/*setter function*/
void Connection::set_m_status(Status status) { m_status_ = status; }
void Connection::set_m_client_fd(int fd) { m_client_fd_ = fd; }
void Connection::set_m_client_ip(std::string ip) { m_client_ip_ = ip; }
void Connection::set_m_client_port(int port) { m_client_port_ = port; }
void Connection::set_m_read_from_server_fd(int fd) { m_read_from_server_fd_ = fd; }
void Connection::set_m_write_to_server_fd(int fd) { m_write_to_server_fd_ = fd; }
void Connection::set_m_readed_size(int size) { m_readed_size_ = size; }
void Connection::set_m_read_buffer_client(std::string read_buffer) { m_read_buffer_client_ = read_buffer; }


/*member function*/
std::string Connection::GetExtension(std::string path) {
  std::string ret;

  if (path.find(".") != std::string::npos) {
    size_t idx = path.rfind(".");
    if (idx != path.size() - 1)
      ret = path.substr(idx + 1);
  }
  return ret;
}

std::string  Connection:: GetMimeTypeHeader(std::string path) {
  std::string extension = GetExtension(path);
  std::string ret;
  std::map<std::string, std::string>  mime_types = m_webserv_config_->get_m_mime_types();

  if (!extension.empty() && ft::hasKey(mime_types, extension))
    ret = "Content-type:" + mime_types[extension];
  else
    ret = "Content-type:test-for-evaluation";
  return ret;
}

time_t Connection::GetLastModified(std::string path) {
  struct stat buf;

  ft::bzero(&buf, sizeof(struct stat));
  stat(path.c_str(), &buf);
  return buf.st_mtimespec.tv_sec;
}

std::string Connection::GetLastModifiedHeader(std::string path) {
  time_t modified = GetLastModified(path);
  struct tm t;
  char buff[1024];

  ft::convertTimespecToTm(modified, &t);
  strftime(buff, sizeof(buff), "%a, %d %b %Y %X GMT", &t);
  return "Last-Modified:" + std::string(buff);
}

int	Connection::SetEnv(char **env, int idx, std::string key, std::string val) {
  char	*item;

  if (key.empty())
    return 0;
  item = ft::strsjoin(key, std::string("="), val);
  env[idx] = item;
  return 1;
}

char** Connection::DupBaseEnvWithExtraSpace(const Request& request) {
  char **base_env = m_webserv_config_->get_m_base_env();
  char **cgi_env = NULL;
  int idx = 0;
  int base_len = ft::lenDoubleStr(base_env);
  //int custom_len = request.get_m_special_header_count();

  //if ((cgi_env = reinterpret_cast<char **>(malloc(sizeof(char *) * (base_len + custom_len + CGI_META_VARIABLE_COUNT + 1)))) == 0)
  //  return NULL);
  while (base_env[idx] != NULL) {
    cgi_env[idx] = ft::strdup(base_env[idx]);
    ++idx;
  }
  // while (idx < base_len + custom_len + CGI_META_VARIABLE_COUNT + 1) /* custom len?? */
  //   cgi_env[idx++] = NULL;
  return cgi_env;
}

std::string Connection::GetCGIEnvValue(const Request& request, std::string token) {
  if (token == "CONTENT_LENGTH") {
    if (ft::hasKey(request.get_m_headers(), "Content-Length"))
      return request.get_m_headers().find("Content-Length")->second;
    return std::string("-1");
  }
  else if (token == "CONTENT_TYPE") {
    if (ft::hasKey(request.get_m_headers(), "Content-Type"))
      return request.get_m_headers().find("Content-Type")->second;
    return std::string();
  }
  else if (token == "AUTH_TYPE")
    return m_webserv_config_->get_m_cgi_version();
  else if (token == "PATH_INFO")
    return request.get_m_path_info();
  else if (token == "PATH_TRANSLATED")
    return request.get_m_path_translated();
  else if (token == "QUERY_STRING")
    return request.get_m_query();
  else if (token == "REMOTE_ADDR")
    return request.get_m_connection()->get_m_client_ip();
  else if (token == "REQUEST_METHOD")
    return request.get_m_method_to_string();
  else if (token == "REQUEST_URI")
    return request.get_m_uri();
  else if (token == "SCRIPT_NAME")
    return request.get_m_script_translated();
  else if (token == "SERVER_NAME")
    return m_server_config_->get_m_server_name();
  else if (token == "SERVER_PORT")
    return ft::to_string(m_server_config_->get_m_port());
  else if (token == "SERVER_PROTOCOL")
    return "HTTP/" + m_webserv_config_->get_m_http_version();
  else if (token == "SERVER_SOFTWARE")
    return m_webserv_config_->get_m_software_name() + "/" + m_webserv_config_->get_m_software_version();
  else if (token == "GATEWAY_INTERFACE")
    return m_webserv_config_->get_m_cgi_version();
  else
    throw 400;
}

bool Connection::ParseStartLine() {
  size_t new_line;

  if ((new_line = m_read_buffer_client_.find("\r\n")) != std::string::npos) {
    std::string start_line = m_read_buffer_client_.substr(0, new_line);
    m_read_buffer_client_ = m_read_buffer_client_.erase(0, new_line + 2);
    m_request_ = Request(this, m_server_config_, start_line);
    return true;
  }
  else if (m_read_buffer_client_.size() > m_server_config_->get_m_request_uri_size_limit())
    std::cout << "URI SIZE ERROR" << std::endl;  //  throw 40006
  return false;
}

int Connection::RecvWithoutBody(char *buf, int buf_size) {
  int i = 0;
  int recv_len;

  if ((recv_len = recv(m_client_fd_, buf, buf_size, MSG_PEEK)) > 0) {
      while(i < recv_len) {
        if (buf[i] == '\r' && i + 3 < recv_len && buf[i + 1] == '\n' && buf[i + 2] == '\r' && buf[i + 3] == '\n')
          break;
        i++;
      }
      if (i == recv_len)
        return 0;
      else if ((recv_len = recv(m_client_fd_, buf, i + 4, 0)) > 0)
        return i + 4;
  }
  if (recv_len  == -1)
    std::cout << "IO error detected to read reqeust message without body for client/\n";
    // throw (Server::IOError((("IO error detected to read reqeust message without body for client ") + ft::to_string(connection.get_m_client_fd())).c_str()));
  else
    std::cout << "Connection close detected by client/\n";
		// throw (Server::IOError((("Connection close detected by client ") + ft::to_string(connection.get_m_client_fd())).c_str()));

  return -1;
}

bool Connection::IsValidHeader(std::string header) {
  if (header.size() > m_server_config_->get_m_request_header_size_limit()) { //m_server->get_m_request_header_limit_size()
    std::cout << "Error: Header Size is over than limit size" << std::endl;
    return false; //throw 40005
  }
  if (header.find(":") == std::string::npos) {
    std::cout << "Error: Header doesn't have\":\"" << std::endl;
    return false;
  }
  return true;
}

bool Connection::ParseHeader() {
  std::string& read_buf = m_read_buffer_client_;
  std::string line;

  // while (getLine(read_buf, line, REQUEST_HEADER_LIMIT_SIZE_MAX) >= 0) {
  //   if (!IsValidHeader(line)) {
  //     std::cout << "Error: Header is not Valid" << std::endl; //throw 40010
  //     return false;
  //   }
  //   AddHeader(line);
  //   line = "";
  // }
  // AddHeader(line);
  // if (!hasKey(m_request_.get_m_headers(),"Host")) {
  //   std::cout << "Error: Header does not have a Host" << std::endl;
  //   return false;
  // }
  // return true;
  while(ft::getLine(read_buf, line, m_server_config_->get_m_request_header_size_limit()) >= 0) {
    if (line == "") {
      if (!ft::hasKey(m_request_.get_m_headers(), "Host")) {
        std::cout << "Error: Header is not Valid" << std::endl; //throw 40010
        return false;
      }
      return true;
    }
    if (IsValidHeader(line)) {
      std::cout << "Error: Header is not Valid" << std::endl; //throw 40010
      return false;
    }
    m_request_.AddHeader(line);
  }
  return false;
}

bool Connection::IsRequestHasBody() {
  if (m_request_.get_m_method() == Request::POST) {
    if (m_request_.get_m_transfer_type() == Request::CHUNKED)
      return true;
    if (ft::hasKey(m_request_.get_m_headers(), "Content-Length") && m_request_.get_m_content_length() > 0)
      return true;
  }
  return false;
}

int Connection::RecvBody(char *buf, int buf_size) {
  int i = 0;
  int read_size = 0;

  if (m_request_.get_m_method() == Request::POST && m_request_.get_m_transfer_type() == Request::CHUNKED)
    return 0;
  if ((read_size = recv(m_client_fd_, buf, buf_size, 0)) > 0)
    return read_size;
  else if (read_size == -1) {
    std::cout << "IO error detected to read reqeust message without body for client " << std::endl; // throw (Server::IOError((("IO error detected to read reqeust message without body for client ") + ft::to_string(connection.get_m_client_fd())).c_str()));
		// throw (Server::IOError((("IO error detected to read reqeust message without body for client ") + ft::to_string(connection.get_m_client_fd())).c_str()));
    return -1;
  }
  else {
    std::cout <<"Connection close detected by client " << std::endl; //throw (Server::IOError((("Connection close detected by client ") + ft::to_string(connection.get_m_client_fd())).c_str()));
    return -1;
  }

  // if ((read_size = read(fd, buf, BUFFER_SIZE)) > 0) {
  //   while(i < read_size) {
  //     if (buf[i] == '\r' && i + 3 < read_size && buf[i + 1] == '\n' && buf[i + 2] == '\r' && buf[i + 3] == '\n')
  //         break;
  //     i++;
  //   }
  //   if (i == read_size)
  //       return 0;
  // }
  // ft_str_index_join(body, buf, i + 4);
  // if (read_size  == -1)
  //   std::cout << "READ ERROR/\n";
  // return i;
}

bool Connection::ReadGeneralBody() {
  if (!ft::hasKey(m_request_.get_m_headers(), "Content-Length"))
    std::cout << "Content-Lenth required" << std::endl; //throw 41101
  if (m_readed_size_ + static_cast<int>(m_read_buffer_client_.size()) <= m_request_.get_m_content_length()) {
    m_request_.AddContent(m_read_buffer_client_);
    m_readed_size_ += m_read_buffer_client_.size();
    m_read_buffer_client_ =  m_read_buffer_client_.erase(0, m_read_buffer_client_.size());
  }
  else {
    std::string part = m_read_buffer_client_.substr(0, m_request_.get_m_content_length() - m_readed_size_);
    m_request_.AddContent(part);
     m_read_buffer_client_ =  m_read_buffer_client_.erase(0, m_read_buffer_client_.size());
    m_readed_size_ = m_request_.get_m_content_length();
  }
  return m_readed_size_ == m_request_.get_m_content_length();
}

int Connection::getChunkedSize(std::string& str, std::string& len) {
  int content_length;

  if (!ft::getNewLine(str, len))
    return -1;
  try {
    content_length = ft::stoi(len, 16);
  } catch (std::exception& e) {
      std::cout << "In chunked request, failed to convert trnasfer-size(maybe not number)" << std::endl;//throw (40017);
    }
  if (content_length < 0)
    std::cout << "In chunked request, failed to convert trnasfer-size(maybe negative number)" << std::endl; //throw (40016);
  if (content_length == 0) {
    if (len[0] != '0')
      std::cout << "In chunked request, failed to convert trnasfer-size(maybe not number)" << std::endl;//throw (40017);
    }
  return content_length;
}

bool Connection::ReadChunkedBody() {
  while (true) {
    std::string len;
    int content_length = getChunkedSize(m_read_buffer_client_, len);
    if (content_length == -1)
      return false;
    if (content_length == 0) {
      if (m_read_buffer_client_.find("\r\n") == std::string::npos) {
        m_read_buffer_client_.insert(0, len + "\r\n");
        return false;
      }
      if (m_read_buffer_client_.size() >= 2 && m_read_buffer_client_[0] == '\r' && m_read_buffer_client_[1] == '\n') {
        m_read_buffer_client_ =  m_read_buffer_client_.erase(0, 2);
        return true;
      }
        std::cout << "In chunked request, for of end-line is not '\\r\\n" << std::endl; //throw (40018);
    }
    if (m_read_buffer_client_.size() < content_length + 2) {
      m_read_buffer_client_.insert(0, len + "\r\n");
      return false;
    }
    if (m_read_buffer_client_.substr(content_length, 2) != "\r\n")
      std::cout << "In chunked request, readed-size and content-length value is not equal" << std::endl; //throw (40021);
    m_request_.AddContent(m_read_buffer_client_.substr(0, content_length));
    m_read_buffer_client_ = m_read_buffer_client_.erase(0, content_length + 2);
  }
}

bool Connection::ParseBody() {
  if (m_request_.get_m_method() == Request::POST && m_request_.get_m_transfer_type() == Request::CHUNKED)
    return true;
  if (m_request_.get_m_transfer_type() == Request:: GENERAL)
    return ReadGeneralBody();
  if (m_request_.get_m_transfer_type() == Request:: CHUNKED)
    return ReadChunkedBody();
  return false;
}

void Connection::set_m_last_request_at() {
  timeval now;

  if (gettimeofday(&now, reinterpret_cast<struct timezone *>(NULL)) == -1)
    return ;
  this->m_last_request_at_ = now;
  return ;
}

void Connection::addReadbufferClient(const char* str, int size) {
  m_read_buffer_client_.append(str, size);
}

void Connection::RecvRequest(void) {
  char buf[BUFFER_SIZE];
  int fd;
  int read_size = BUFFER_SIZE - m_read_buffer_client_.size();
  Request::Phase phase = m_request_.get_m_phase();
  m_status_ = ON_RECV;

  if (phase == Request::READY && (read_size = RecvWithoutBody(buf, sizeof(buf))) > 0)
    addReadbufferClient(buf, read_size);
  if (phase == Request::READY && ParseStartLine())
    phase = Request::ON_HEADER;
  if (phase == Request::ON_HEADER && ParseHeader()) {
    m_request_.set_m_phase(phase = Request::ON_BODY);
    if (!IsRequestHasBody())
      return ;
  }
  if (phase == Request::ON_BODY && (read_size = RecvBody(buf, sizeof(buf))) > 0)
    addReadbufferClient(buf, read_size);
  if (phase == Request::ON_BODY && ParseBody())
    phase = Request::COMPLETE;
  if (phase == Request::COMPLETE)
    set_m_last_request_at();
  m_request_.set_m_phase(phase);
}

void Connection::ReportCreateNewRequestLog(int status)
{
	std::string text = "[Failed][Request][Server:" + m_server_config_->get_m_server_name() + "][CIP:"
	+ m_client_ip_ + "][CFD:" + ft::to_string(m_client_fd_) + "]["
	+ ft::to_string(status) + "][" + Response::status[status] + "] Failed to create new Request.\n";
	ft::log(ServerManager::log_fd, text);
	return ;
}

std::string Connection::GetDateHeader() {
  char buff[1024];
  struct tm t;
  timeval now;

  gettimeofday(&now, NULL);
  ft::convertTimespecToTm(now.tv_sec, &t);
  strftime(buff, sizeof(buff), "%a, %d %b %Y %X GMT", &t);
  return "Last-Modified:" + std::string(buff);
}

std::string Connection::GetServerHeader() {
  return "Server:" + m_server_config_->get_m_server_name();
}

void Connection::CreateCGIResponse(int& status, headers_t& headers, std::string& body) {
  status = 200;
  headers_t headers_in_body = ft::splitStringByChar(ft::rtrimString(body.substr(0, body.find("\r\n\r\n")), "\r\n"));
  std::string key, value;

  for (headers_t::iterator it = headers_in_body.begin(); it != headers_in_body.end(); it++) {
    key = ft::trimString(it->substr(0, it->find(":")), " \t");
    value = ft::trimString(it->substr(it->find(":") + 1), " \r\n\t");
    std::cout << key << " : " << value << std::endl;
    if (key == "Status" || key == "status")
      status = ft::stoi(value);
    else if (!key.empty() && !value.empty())
      headers.push_back(key + ":" + value);
  }
  if (body.find("\r\n\r\n") != std::string::npos)
    body = body.substr(body.find("\r\n\r\n") + 4);
  else if (body.find("\n\n") != std::string::npos)
    body = body.substr(body.find("\n\n") + 2);
  else
    body = "";
  if (body.size() == 0)
    return ;
  headers.push_back("Transfer-Encoding:chunked");
}

void Connection::CreateResponse(int status, headers_t headers, std::string body) {
  if (status >= 40000) {
    ReportCreateNewRequestLog(status);
    status /= 100;
  }

  headers.push_back(GetDateHeader());
  headers.push_back(GetServerHeader());

  if (status == CGI_SUCCESS_CODE)
    CreateCGIResponse(status, headers, body);
  if (status >= 400 && status <= 599) {
    body = m_server_config_->get_m_default_error_page_path();
    body.replace(body.find("#ERROR_CODE"), 11, ft::to_string(status));
    body.replace(body.find("#ERROR_CODE"), 11, ft::to_string(status));
    body.replace(body.find("#ERROR_DESCRIPTION"), 18, Response::status[status]);
    body.replace(body.find("#ERROR_DESCRIPTION"), 18, Response::status[status]);
    body.replace(body.find("#PORT"), 5, ft::to_string(m_server_config_->get_m_port()));
  }
  if (!ft::hasKey(ft::stringVectorToMap(headers), "Transfer-Encoding"))
    headers.push_back("Content-Length:" + ft::to_string(body.size()));
  if (!body.empty())
    headers.push_back("Content-Language:ko-KR");
  if (status / 100 != 2)
    headers.push_back("Connection:close");
  if (status / 100 == 3)
    headers.push_back("Location:/");
  if (status == 504)
    headers.push_back("Retry-After:3600");

  Response& response = const_cast<Response&>(m_response_);
  response = Response(this, status, body);
  headers_t::iterator it = headers.begin();
  for (; it != headers.end(); ++it) {
    std::string key = ft::rtrimString((*it).substr(0, (*it).find(":")), " ");
    std::string value = ft::ltrimString((*it).substr((*it).find(":") + 1), " ");
    response.addHeader(key, value);
  }
  // writeCreateNewResponseLog(response);
  const_cast<Request&>(m_request_).set_m_phase(Request::COMPLETE);
  m_status_ = TO_SEND;
  //m_manager->fdSet(response.get_m_connection()->get_m_client_fd(), ServerManager::WRITE_SET);
}

bool makeAutoindexContent(HtmlWriter& html, std::string cwd, std::string directory_uri) {
  DIR *dir = NULL;
  struct dirent *de = NULL;
  char buff[1024];
  int idx = 7;

  if ((dir = opendir(cwd.c_str())) == NULL)
    return false;
  while ((de = readdir(dir)) != NULL) {
    std::string name = de->d_name;
    if (name == "." || (name != ".." && name[0] == '.'))
      continue ;
    if (de->d_type == DT_DIR || de->d_type == DT_REG) {
      std::string content;
      content.append(html.MakeLink(directory_uri + name, name));
      content.append(std::string(51 - std::string(name).size(), ' '));

      struct stat buf;
      struct tm t;
      ft::bzero(&buf, sizeof(struct stat));
      stat((std::string(cwd) + "/" + name).c_str(), &buf);
      ft::convertTimespecToTm(buf.st_mtimespec.tv_sec, &t);
      strftime(buff, sizeof(buff), "%d-%h-%G %H:%M", &t);

      content.append(std::string(buff));
      content.append(std::string(20 - ft::to_string(de->d_reclen).size(), ' '));
      content.append(ft::to_string(de->d_reclen));
      html.Add_line(idx++, content);
    }
  }
  closedir(dir);
  return true;
}

void MakeAutoindexForm(HtmlWriter& html, const Request& request) {
  std::string title = "Index of " + request.get_m_uri();

  html.Add_title(title);
  html.Add_bgcolor("white");
  html.Add_tag("\"white\">\n", "h1", title, false);
  html.Add_tag("/h1>\n", "hr", "", true);
  html.Add_tag("hr>\n", "pre", "", true);
}

int GetValidIndexFd(const Request& request) {
  std::set<std::string> index = request.get_m_locationconfig()->get_m_index();
  std::set<std::string>::iterator it = index.begin();
  struct stat buf;
  int fd = -1;
  std::string path, body;

  for (; it != index.end(); ++it) {
    path = request.get_m_script_translated();
    if (path[path.size() - 1] != '/')
      path.push_back('/');
    path.append(*it);
    stat(path.c_str(), &buf);
    if (S_ISREG(buf.st_mode) && (fd = open(path.c_str(), O_RDONLY)) > -1)
      break ;
  }
  return fd;
}

char** Connection::CreateCGIEnv(const Request& request) {
  char **env = DupBaseEnvWithExtraSpace(request);
  int idx = ft::lenDoubleStr(m_webserv_config_->get_m_base_env());
  SetEnv(env, idx++, "AUTH_TYPE", "");
  SetEnv(env, idx++, "CONTENT_LENGTH", GetCGIEnvValue(request, "CONTENT_LENGTH"));
  SetEnv(env, idx++, "CONTENT_TYPE", GetCGIEnvValue(request, "CONTENT_TYPE"));
  SetEnv(env, idx++, "GATEWAY_INTERFACE", GetCGIEnvValue(request, "GATEWAY_INTERFACE"));
  SetEnv(env, idx++, "PATH_INFO", GetCGIEnvValue(request, "PATH_INFO"));
  SetEnv(env, idx++, "PATH_TRANSLATED", GetCGIEnvValue(request, "PATH_TRANSLATED"));
  SetEnv(env, idx++, "QUERY_STRING", GetCGIEnvValue(request, "QUERY_STRING"));
  SetEnv(env, idx++, "REMOTE_ADDR", GetCGIEnvValue(request, "REMOTE_ADDR"));
  SetEnv(env, idx++, "REQUEST_METHOD", GetCGIEnvValue(request, "REQUEST_METHOD"));
  SetEnv(env, idx++, "REQUEST_URI", GetCGIEnvValue(request, "REQUEST_URI"));
  SetEnv(env, idx++, "SCRIPT_NAME", GetCGIEnvValue(request, "SCRIPT_NAME"));
  SetEnv(env, idx++, "SERVER_NAME", GetCGIEnvValue(request, "SERVER_NAME"));
  SetEnv(env, idx++, "SERVER_PORT", GetCGIEnvValue(request, "SERVER_PORT"));
  SetEnv(env, idx++, "SERVER_PROTOCOL", GetCGIEnvValue(request, "SERVER_PROTOCOL"));
  SetEnv(env, idx++, "SERVER_SOFTWARE", GetCGIEnvValue(request, "SERVER_SOFTWARE"));

  std::map<std::string, std::string>::const_iterator it = request.get_m_headers().begin();
  for (; it != request.get_m_headers().end(); ++it) {
    if (it->first[0] == 'X') {
      std::string new_header = "HTTP_";
      for (std::string::const_iterator it2 = it->first.begin(); it2 != it->first.end(); ++it2)
        new_header.push_back(std::toupper(*it2));
      SetEnv(env, idx++, new_header, it->second);
    }
  }
  return env;
}

void Connection::ExecuteAutoindex(const Request& request) {
  char cwd[BUFFER_SIZE];

  getcwd(cwd, sizeof(cwd));
  if (request.get_m_locationconfig()->get_m_autoindex()) {
    HtmlWriter html;
    MakeAutoindexForm(html, request);
    std::string directory_uri = request.get_m_uri();
    if (directory_uri[directory_uri.size() - 1] != '/')
      directory_uri.push_back('/');
    if (!makeAutoindexContent(html, request.get_m_script_translated(), directory_uri))
      return CreateResponse(50002);
    return CreateResponse(200, headers_t(), html.get_m_body());
  }
  else {
    int fd = GetValidIndexFd(request);
    if (fd == -1)
      return CreateResponse(40403);
    return CreateResponse(200, headers_t(), ft::getStringFromFd(fd));
  }
}

void Connection::ExecuteGet(const Request& request) {
  std::string path = request.get_m_path_translated();
  std::string body;

  try {
    body = ft::getStringFromFile(path, m_server_config_->get_m_client_body_size_limit());
  } catch (std::overflow_error& e) {
    return CreateResponse(41304);
  }
  headers_t headers(1, GetMimeTypeHeader(path));
  if (headers[0].empty())
    return CreateResponse(41501);
  headers.push_back(GetLastModifiedHeader(path));
  return CreateResponse(200, headers, body);
}

void Connection::Closes(int fd1, int fd2, int fd3, int fd4, int fd5) {
  if (fd1 != -1)
    close(fd1);
  if (fd2 != -1)
    close(fd2);
  if (fd3 != -1)
    close(fd3);
  if (fd4 != -1)
    close(fd4);
  if (fd5 != -1)
    close(fd5);
}
void Connection::ExecveCGI(const Request& request, char **env, int *parent_write_fd, int *child_write_fd) {
  Closes(parent_write_fd[1], child_write_fd[0]);
  dup2(parent_write_fd[0], 0);
  dup2(child_write_fd[1], 1);
  /* child process */
  char *arg[2] = { const_cast<char *>(request.get_m_script_translated().c_str()), NULL };
  std::string script_name = GetCGIEnvValue(request, "SCRIPT_NAME");
  std::string ext = script_name.substr(script_name.rfind(".") + 1);
  if (ext == "php" && execve("./php-cgi", arg, env) == -1)
    exit(EXIT_FAILURE);
  else if (execve(arg[0], arg, env) == -1)
    exit(EXIT_FAILURE);
  exit(EXIT_FAILURE);
}

void Connection::ExecuteCGI(const Request& request)
{
  pid_t pid;
  int parent_write_fd[2];
  int child_write_fd[2];
  char **env;
  Request::Method method = request.get_m_method();
  std::string body;

  if ((env = CreateCGIEnv(request)) == NULL)
    return CreateResponse(50003);
  pipe(parent_write_fd);
  pipe(child_write_fd);
  pid = fork();
  if (pid == 0)
    return ExecveCGI(request, env, parent_write_fd, child_write_fd);
  else if (pid < 0) {
    Closes(parent_write_fd[0], parent_write_fd[1], child_write_fd[0], child_write_fd[1]);
    return CreateResponse(50004, headers_t());
  }

  Closes(parent_write_fd[0], child_write_fd[1]);
  if (fcntl(parent_write_fd[1], F_SETFL, O_NONBLOCK) == -1)
    throw std::runtime_error("write fnctl error");
  if (fcntl(child_write_fd[0], F_SETFL, O_NONBLOCK) == -1)
    throw std::runtime_error("read fnctl error");
  m_status_ = Connection::ON_EXECUTE;
  if (method == Request::POST) {
    m_write_to_server_fd_ = parent_write_fd[1];
    if (request.get_m_transfer_type() == Request::GENERAL)
      m_wbuf_ = m_request_.get_m_content();
    //m_manager->fdSet(connection.get_m_write_to_server_fd(), ServerManager::WRITE_SET);
  }
  else
    close(parent_write_fd[1]);
  m_read_from_server_fd_ = child_write_fd[0];
  //m_manager->fdSet(connection.get_m_read_from_server_fd(), ServerManager::READ_SET);
  ft::freeDoublestr(&env);
}

void Connection::ExecutePost(const Request& request) {
  if (request.get_m_headers().find("Content-Length")->second == "0")
    return ExecuteGet(request);
  else
    return CreateResponse(40023);
}

void Connection::SolveRequest(const Request& request) {
  LocationConfig* locationconfig = request.get_m_locationconfig();
  Request::Method method = request.get_m_method();
  std::string methodString = request.get_m_method_to_string();

  if (!ft::hasKey(locationconfig->get_m_allow_method(), methodString)) {
    headers_t headers(1, "Allow:" + ft::containerToString(locationconfig->get_m_allow_method(), ", "));
    return CreateResponse(40501, headers);
  }
  // if (isAuthorizationRequired(location)) {
  // 	if (!hasCredential(request)) {
  // 		return makeResponse401(this, request, connection));
  // 	} else {
  // 		std::vector<std::string> credential = ft::splitStringByChar(request.get_m_headers().find("Authorization")->second, ' ');
  // 		if (!isValidCredentialForm(credential))
  // 			return CreateResponse(40022));
  // 		else if (!isValidCredentialContent(location, credential))
  // 			return CreateResponse( 40301));
  // 	}
  // }
  if (request.get_m_uri_type() == Request::DIRECTORY)
    return ExecuteAutoindex(request);
  else if (request.get_m_uri_type() == Request::CGI)
    return ExecuteCGI(request);
  else if (method == Request::GET)
    ExecuteGet(request);
  else if (method == Request::POST)
    ExecutePost(request);
  else if (method == Request::DELETE)
    ExecuteDelete(request);
  else
    throw (400);
}

void Connection::ExecuteDelete(const Request& request) {
  if (unlink(request.get_m_path_translated().c_str()) == -1)
    CreateResponse(204);
  else
    return CreateResponse(204);
}

void Connection::WriteCreateNewRequestLog(const Request& request)
{
  if (request.get_m_method() != Request::POST)
    return ;
  int cfd = request.get_m_connection()->get_m_client_fd();
  std::string text = ft::getTimestamp() + "[Created][Request][Server:" + m_server_config_->get_m_server_name() + "]"
  + "[CFD:" + ft::to_string(cfd) + "][Method:" \
  + request.get_m_method_to_string() + "][URI:" + request.get_m_uri() + "]";
  if (request.get_m_method() == Request::GET)
    text.append("[Query:" + request.get_m_query() + "]");
  text.append(" New request created.\n");
  ft::log(ServerManager::log_fd, text);
  return ;
}

bool Connection::RunRecvAndSolve() {
  try {
    RecvRequest();
  } catch (int status_code) {
    CreateResponse(status_code);
    return true;
  } /*catch (Server::IOError& e) {
    throw (e);
  } */catch (std::exception& e) {
    ft::log(ServerManager::log_fd, std::string("[Failed][Request] Failed to create request because ") + e.what());
    CreateResponse(50001);
    return true;
  }
  const Request& request = m_request_;
  if (request.get_m_phase() == Request::COMPLETE) {
    WriteCreateNewRequestLog(request);
    m_status_ = ON_EXECUTE;
    SolveRequest(m_request_);
    return true;
  }
  return false;
}
