/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/25 21:44:42 by jihoolee          #+#    #+#             */
/*   Updated: 2022/06/07 21:56:11 by jihoolee         ###   ########.fr       */
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
      m_wbuf_data_size_(0),
      m_send_data_size_(0),
      m_readed_size_(0),
      m_read_buffer_client_(),
      m_read_buffer_server_(),
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
      m_wbuf_data_size_(0),
      m_send_data_size_(0),
      m_readed_size_(0),
      m_read_buffer_client_(),
      m_read_buffer_server_(),
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
      m_wbuf_data_size_(c.m_wbuf_data_size_),
      m_send_data_size_(c.m_send_data_size_),
      m_readed_size_(c.m_readed_size_),
      m_read_buffer_client_(c.m_read_buffer_client_),
      m_read_buffer_server_(c.m_read_buffer_server_),
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
  m_wbuf_data_size_ = operand.m_wbuf_data_size_;
  m_send_data_size_ = operand.m_send_data_size_;
  m_readed_size_ = operand.m_readed_size_;
  m_read_buffer_client_ = operand.m_read_buffer_client_;
  m_read_buffer_server_ = operand.m_read_buffer_server_;
  m_request_ = operand.m_request_;
  m_response_ = operand.m_response_;
  return *this;
}

Connection::~Connection(void) {}

void Connection::clear() {
  m_status_ = ON_WAIT;
  m_read_from_server_fd_ = -1;
  m_write_to_server_fd_ = -1;
  m_request_.clear();
  m_readed_size_ = 0;
  m_response_.clear();
  m_read_buffer_client_.clear();  //  TO_CHECK
  m_read_buffer_server_.clear();
  m_wbuf_.clear();
  m_wbuf_data_size_ = 0;
  m_send_data_size_ = 0;
}

/*getter function*/
ServerConfig* Connection::get_m_server_config() const { return m_server_config_; }
Connection::Status Connection::get_m_status() const { return m_status_; }
int Connection::get_m_client_fd() const { return m_client_fd_; }
timeval Connection::get_m_last_request_at() const { return m_last_request_at_; }
std::string Connection::get_m_client_ip() const { return m_client_ip_; }
int Connection::get_m_client_port() const { return m_client_port_; }
int Connection::get_m_read_from_server_fd() const { return m_read_from_server_fd_; }
int Connection::get_m_write_to_server_fd() const { return m_write_to_server_fd_; }
int Connection::get_m_wbuf_data_size() const { return m_wbuf_data_size_; }
int Connection::get_m_send_data_size() const { return m_send_data_size_; }
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
void Connection::set_m_wbuf_data_size(int size) { m_wbuf_data_size_ = size; }
void Connection::set_m_sent_data_size(int size) { m_send_data_size_ = size; }
void Connection::set_m_readed_size(int size) { m_readed_size_ = size; }
void Connection::set_m_read_buffer_client(std::string read_buffer) { m_read_buffer_client_ = read_buffer; }

void Connection::set_m_wbuf_for_send(std::string wbuf_string) {
  if (wbuf_string.empty())
    m_wbuf_ = m_response_.GetString();
  else
    m_wbuf_ = wbuf_string;
  m_wbuf_data_size_ = m_wbuf_.size();
  m_send_data_size_ = 0;
}

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
    throw 40000;
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
    throw 40006;
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
    throw ServerManager::IOError(
      ("IO error detected to read reqeust message without body for client "
        + ft::to_string(m_client_fd_)).c_str());
  else
    throw ServerManager::IOError(
      ("Connection close detected by client "
        + ft::to_string(m_client_fd_)).c_str());
  return -1;
}

bool Connection::IsValidHeader(std::string header) {
  if (header.size() > m_server_config_->get_m_request_header_size_limit()) //m_server->get_m_request_header_limit_size()
    throw 40005;
  if (header.find(":") == std::string::npos)
    return false;
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
  // previous pureum code
  while(ft::getLine(read_buf, line, m_server_config_->get_m_request_header_size_limit()) >= 0) {
    if (line == "") {
      if (!ft::hasKey(m_request_.get_m_headers(), "Host"))
        throw 40010;
      return true;
    }
    if (IsValidHeader(line))
      throw 40010;
    m_request_.AddHeader(line);
  }
  return false;
}

bool Connection::IsRequestHasBody() {  // 수정 필요함
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

  // if (m_request_.get_m_method() == Request::POST && m_request_.get_m_transfer_type() == Request::CHUNKED)
  //   return 0;
  if ((read_size = recv(m_client_fd_, buf, buf_size, 0)) > 0)
    return read_size;
  else if (read_size == -1)
    throw ServerManager::IOError(
      ("IO error detected to read request message without body for client "
        + ft::to_string(m_client_fd_)).c_str());
  else
    throw ServerManager::IOError(
      ("Connection close detected by client "
      + ft::to_string(m_client_fd_)).c_str());
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
    throw 41101;
  int content_length = m_request_.get_m_content_length();

  if (m_readed_size_ + static_cast<int>(m_read_buffer_client_.size()) <= content_length) {
    m_request_.AddContent(m_read_buffer_client_);
    m_readed_size_ += m_read_buffer_client_.size();
    m_read_buffer_client_ =  m_read_buffer_client_.erase(0, m_read_buffer_client_.size());
  } else {
    std::string part = m_read_buffer_client_.substr(0, content_length - m_readed_size_);
    m_request_.AddContent(part);
     m_read_buffer_client_ =  m_read_buffer_client_.erase(0, m_read_buffer_client_.size());
    m_readed_size_ = content_length;
  }
  return m_readed_size_ == content_length;
}

int Connection::getChunkedSize(std::string& str, std::string& len) {
  int content_length;

  if (!ft::getNewLine(str, len))
    return -1;
  try {
    content_length = ft::stoi(len, 16);
  } catch (std::exception& e) {
      throw 40017;
  }
  if (content_length < 0)
    throw 40016;
  if (content_length == 0) {
    if (len[0] != '0')
      throw 40017;
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
      throw 40018;
    }
    if (m_read_buffer_client_.size() < content_length + 2) {
      m_read_buffer_client_.insert(0, len + "\r\n");
      return false;
    }
    if (m_read_buffer_client_.substr(content_length, 2) != "\r\n")
      throw 40021;
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

void Connection::makeResponse401() {
  std::string header = "WWW-Authenticate:Basic realm=\"";

  header.append(m_request_.get_m_locationconfig()->get_m_auth_basic_realm());
  header.append("\", charset=\"UTF-8\"");
  return CreateResponse(40101, headers_t(1, header));
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
  m_request_.set_m_phase(Request::COMPLETE);
  m_status_ = TO_SEND;
  m_server_manager_->addEvent(m_client_fd_, EVFILT_WRITE, EV_ADD | EV_ENABLE,
                              0, 0, NULL);
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
    m_server_manager_->addEvent(m_write_to_server_fd_, EVFILT_WRITE,
                                EV_ADD | EV_ENABLE, 0, 0, NULL);
    m_server_manager_->addCGIConnectionMap(m_write_to_server_fd_, this);
    m_server_manager_->insertFd(m_write_to_server_fd_, ServerManager::FD_CGI);
    //m_manager->fdSet(connection.get_m_write_to_server_fd(), ServerManager::WRITE_SET);
  }
  else
    close(parent_write_fd[1]);
  m_read_from_server_fd_ = child_write_fd[0];
  m_server_manager_->addEvent(m_read_from_server_fd_, EVFILT_READ,
                              EV_ADD | EV_ENABLE, 0, 0, NULL);
  m_server_manager_->addCGIConnectionMap(m_read_from_server_fd_, this);
  m_server_manager_->insertFd(m_write_to_server_fd_, ServerManager::FD_CGI);
  //m_manager->fdSet(connection.get_m_read_from_server_fd(), ServerManager::READ_SET);
  ft::freeDoublestr(&env);
}

void Connection::ExecutePost(const Request& request) {
  if (request.get_m_headers().find("Content-Length")->second == "0")
    return ExecuteGet(request);
  else
    return CreateResponse(40023);
}

namespace {
static const std::string base64_chars =
              "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
              "abcdefghijklmnopqrstuvwxyz"
              "0123456789+/";

static inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(unsigned char const* buf, unsigned int bufLen) {
  std::string ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (bufLen--) {
    char_array_3[i++] = *(buf++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;
      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }
  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';
    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;
    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];
    while((i++ < 3))
      ret += '=';
  }
  return ret;
}

std::vector<unsigned char> base64_decode(std::string const& encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::vector<unsigned char> ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);
      char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
      for (i = 0; (i < 3); i++)
          ret.push_back(char_array_3[i]);
      i = 0;
    }
  }
  if (i) {
    for (j = i; j <4; j++)
      char_array_4[j] = 0;
    for (j = 0; j <4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);
    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
    for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
  }
    return ret;
}

void basic_decode(std::string data, std::string& key, std::string& value) {
  std::string decoded_data = ft::containerToString(base64_decode(data), "");

  if (decoded_data.find(":") == std::string::npos || decoded_data.find(":") == decoded_data.size() - 1)
    return;
  int idx = decoded_data.find(":");

  key = decoded_data.substr(0, idx);

  std::vector<unsigned char> value_base(decoded_data.begin()+ idx + 1, decoded_data.end());
  value = base64_encode(&value_base[0], value_base.size());
}

bool isAuthorizationRequired(LocationConfig* location) {
  return !location->get_m_auth_basic_realm().empty();
}

bool hasCredential(const Request& request) {
  return ft::hasKey(request.get_m_headers(), "Authorization");
}

bool isValidCredentialForm(std::vector<std::string> credential) {
  return (credential.size() == 2 && credential[0] != "basic");
}

bool isValidCredentialContent(LocationConfig* location,
                              std::vector<std::string>& credential) {
  std::string key, value;
  basic_decode(credential[1], key, value);
  return (key.empty() || value.empty() ||
          !ft::hasKey(location->get_m_auth_basic_file, key) ||
          location->get_m_auth_basic_file().find(key)->second != value);
}
}  //  anonymous namespace

void Connection::SolveRequest() {
  LocationConfig* locationconfig = m_request_.get_m_locationconfig();
  Request::Method method = m_request_.get_m_method();
  std::string methodString = m_request_.get_m_method_to_string();

  if (!ft::hasKey(locationconfig->get_m_allow_method(), methodString)) {
    headers_t headers(1, "Allow:" + ft::containerToString(locationconfig->get_m_allow_method(), ", "));
    return CreateResponse(40501, headers);
  }
  if (isAuthorizationRequired(locationconfig)) {
    if (!hasCredential(m_request_)) {
      return makeResponse401();
    } else {
      std::vector<std::string> credential =
        ft::splitStringByChar(
          m_request_.get_m_headers().find("Authorization")->second, ' ');
      if (!isValidCredentialForm(credential))
        return CreateResponse(40022);
      else if (!isValidCredentialContent(locationconfig, credential))
        return CreateResponse(40301);
    }
  }
  if (m_request_.get_m_uri_type() == Request::DIRECTORY)
    return ExecuteAutoindex(m_request_);
  else if (m_request_.get_m_uri_type() == Request::CGI)
    return ExecuteCGI(m_request_);
  else if (method == Request::GET)
    ExecuteGet(m_request_);
  else if (method == Request::POST)
    ExecutePost(m_request_);
  else if (method == Request::DELETE)
    ExecuteDelete(m_request_);
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
  } catch (ServerManager::IOError& e) {
    throw (e);
  }catch (std::exception& e) {
    ft::log(ServerManager::log_fd,
      std::string("[Failed][Request] Failed to create request because ")
        + e.what());
    CreateResponse(50001);
    return true;
  }
  if (m_request_.get_m_phase() == Request::COMPLETE) {
    WriteCreateNewRequestLog(m_request_);
    m_status_ = ON_EXECUTE;
    SolveRequest();
    return true;
  }
  return false;
}

bool Connection::runSend() {
  Connection::Status status = m_status_;
  int fd = m_client_fd_;

  if (status == Connection::TO_SEND) {
    set_m_wbuf_for_send();
    m_status_ = ON_SEND;
  }
  sendFromWbuf();
  if (m_wbuf_data_size_ == m_send_data_size_) {
    m_status_ = ON_WAIT;
    m_server_manager_->addEvent(m_client_fd_, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    writeSendResponseLog(m_response_);
    if (m_response_.get_m_status_code() / 100 != 2)
      throw ServerManager::IOError("send error response.");
    else
      this->clear();
  }
  return m_wbuf_data_size_ == m_send_data_size_;
}

void Connection::sendFromWbuf() {
  int count = m_wbuf_data_size_ - m_send_data_size_;

  if (count > BUFFER_SIZE)
    count = BUFFER_SIZE;
  count = send(m_client_fd_, m_wbuf_.c_str() + m_send_data_size_, count, 0);
  if (count == 0 || count == -1)
    throw ServerManager::IOError(
      ("IOError detected to send respoonse message to cient"
      + ft::to_string(m_client_fd_)).c_str());
  m_send_data_size_ += count;
}

void Connection::writeSendResponseLog(const Response& response) {
  std::string text = ft::getTimestamp() + "[Sended][Response][Server:" + m_server_config_->get_m_server_name() + "][" \
  + ft::to_string(response.get_m_status_code()) + "][" + response.get_m_status_description() + "][CFD:" \
  + ft::to_string(response.get_m_connection()->get_m_client_fd()) + "][headers:" \
  + ft::to_string(response.get_m_headers().size()) + "][body:" + ft::to_string(response.get_m_content().size()) + "]";
  text.append(" Response sended\n");
  ft::log(ServerManager::log_fd, text);
}

void Connection::addReadbufferServer(const char* str, int size) {
  m_read_buffer_server_.append(str, size);
}

// void Connection::writeChunkedBodyToCGIScript() {
//   std::string& rbuf = m_read_buffer_client_;
//   int client_fd = m_client_fd_;
//   int to_child_fd = m_write_to_server_fd_;
//   char buf[BUFFER_SIZE];
//   int count;

//   if (rbuf.size() < 70000 && /*FD_ISSET(client_fd_, ServerManager::READ_COPY_SET*/) {
//     if ((count = recv(m_client_fd_, buf, sizeof(buf), 0)) > 0)
//       addReadbufferClient(buf, count);
//     else if (count == -1)
//       throw ServerManager::IOError(("IOERROR"));
//     else
//       throw ServerManager::IOError(("Connection close detected by client"));
//   }
// }

void Connection::writeSavedBodyToCGIScript() {
  int to_child_fd = m_write_to_server_fd_;
  const std::string& data = m_wbuf_;

  if (!data.empty()) {
    int count = data.size() > BUFFER_SIZE ? BUFFER_SIZE : data.size();
    count = write(to_child_fd, data.c_str(), count);
    if (count == 0 || count == -1)
      throw ServerManager::IOError(
        ("IO Error detected from write body to child proccess"
        + ft::to_string(to_child_fd)).c_str());
    m_wbuf_ = m_wbuf_.erase(0, count);
  } else {
    close(to_child_fd);
    m_write_to_server_fd_ = -1;
    m_server_manager_->addEvent(to_child_fd, EVFILT_WRITE,
                                EV_DELETE, 0, 0, NULL);
  }
}

bool Connection::runExecute(ServerManager::CGIMode mode) {
  int from_child_fd = m_read_from_server_fd_;
  int to_child_fd = m_write_to_server_fd_;
  int stat;
  bool read_end = false;

  if (from_child_fd != -1 && mode == ServerManager::CGI_READ) {
    char buf[BUFFER_SIZE];
    int count = read(from_child_fd, buf, sizeof(buf));

    if (count == 0)
      read_end = true;
    else if (count > 0)
      addReadbufferServer(buf, count);
    else
      throw ServerManager::IOError("IO error detected to read from child proccess");


  if (to_child_fd != 1 && mode == ServerManager::CGI_WRITE) {
    // if (m_request_.get_m_method() == Request::POST &&
    //     m_request_.get_m_transfer_type == Request::CHUNKED)
    //   writeChunkedBodyToCGIScript();
    // else
      writeSavedBodyToCGIScript();
  }

  waitpid(-1, &stat, WNOHANG);
  if (WIFEXITED(stat) && read_end == true && m_write_to_server_fd_ == -1) {
    if (from_child_fd != -1) {
      close(from_child_fd);
      m_server_manager_->addEvent(from_child_fd, EVFILT_WRITE,
                                  EV_DELETE, 0, 0, NULL);
      std::string body = m_read_buffer_server_;
      m_read_buffer_server_.clear();
      m_wbuf_.clear();
      if (m_request_.get_m_uri_type() == Request::CGI) {
        int body_size = m_request_.get_m_locationconfig()->get_m_limit_client_body_size();
        if (body.size() > body_size + body.find("\r\n\r\n" + 4)) {
          CreateResponse(41301);
        } else {
          std::cout << body << std::endl;
          CreateResponse(CGI_SUCCESS_CODE, headers_t(), body);
        }
      } else {
        CreateResponse(200, headers_t(), body);
      }
      m_status_ = TO_SEND;
      return true;
    }
    return false;
  }
}
