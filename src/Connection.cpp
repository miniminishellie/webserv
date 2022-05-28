/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/25 21:44:42 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/28 20:48:27 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "ServerManager.hpp"
#include "Libft.hpp"

Connection::Connection(void) {}

Connection::Connection(const Connection &c)
    : m_status_(c.m_status_),
      m_client_fd_(c.m_client_fd_),
      m_last_request_at_(c.m_last_request_at_),
      m_client_ip_(c.m_client_ip_),
      m_client_port_(c.m_client_port_),
      m_readed_size_(c.m_readed_size_),
      m_read_buffer_client_(c.m_read_buffer_client_),
      m_request_(c.m_request_) {}

Connection &Connection::operator=(const Connection &operand) {
  m_status_ = operand.m_status_;
  m_client_fd_ = operand.m_client_fd_;
  m_last_request_at_ = operand.m_last_request_at_;
  m_client_ip_ = operand.m_client_ip_;
  m_client_port_ = operand.m_client_port_;
  m_readed_size_ = operand.m_readed_size_;
  m_read_buffer_client_ = operand.m_read_buffer_client_;
  m_request_ = operand.m_request_;
  return (*this);
}

Connection::~Connection(void) {}

/*getter function*/
Connection::Status Connection::get_m_status() const { return m_status_; }
int Connection::get_m_client_fd() const { return m_client_fd_; }
timeval Connection::get_m_last_request_at() const { return m_last_request_at_; }
std::string Connection::get_m_client_ip() const { return m_client_ip_; }
int Connection::get_m_client_port() const { return m_client_port_; }
int Connection::get_m_readed_size() const { return m_readed_size_; }
std::string Connection::get_m_read_buffer_client() const { return m_read_buffer_client_; }
const Request& Connection::get_m_request() const { return m_request_; }

/*setter function*/
void Connection::set_m_status(Status status) { m_status_ = status; }
void Connection::set_m_client_fd(int fd) { m_client_fd_ = fd; }
void Connection::set_m_client_ip(std::string ip) { m_client_ip_ = ip; }
void Connection::set_m_client_port(int port) { m_client_port_ = port; }
void Connection::set_m_readed_size(int size) { m_readed_size_ = size; }
void Connection::set_m_read_buffer_client(std::string read_buffer) { m_read_buffer_client_ = read_buffer; }


/*member function*/
bool Connection::ParseStartLine() {
  size_t new_line;

  if ((new_line = m_read_buffer_client_.find("\r\n")) != std::string::npos) {
    std::string start_line = m_read_buffer_client_.substr(0, new_line);
    m_read_buffer_client_ = m_read_buffer_client_.erase(0, new_line + 2);
    m_request_ = Request(this, m_server_, start_line);
    return true;
  }
  else if (m_read_buffer_client_.size() > m_serverconfig_.get_m_request_uri_size_limit())
    std::cout << "URI SIZE ERROR" << std::endl;
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
  else
    std::cout << "Connection close detected by client/\n";
  return -1;
}

bool Connection::IsValidHeader(std::string header) {
  if (header.size() > m_serverconfig_.get_m_request_header_size_limit()) { //m_server->get_m_request_header_limit_size()
    std::cout << "Error: Header Size is over than limit size" << std::endl;
    return false; //throw 40005
  }
  if (header.find(":") == std::string::npos) {
    std::cout << "Error: Header doesn't have\":\"" << std::endl;
    return false;
  }
  return true;
}

void Connection::AddHeader(std::string header) {
  size_t pos = header.find(":");
  std::string key = ft::trimString(header.substr(0, pos));
  std::string value = ft::trimString(header.substr(pos + 1));

  for (size_t i = 0 ; i < key.length() ; ++i)
    key[i] = (i == 0 || key[i - 1] == '-') ? std::toupper(key[i]) : std::tolower(key[i]);
  std::pair<std::map<std::string, std::string>::iterator, bool> result = m_request_.get_m_headers().insert(std::make_pair(key, value));
  if (!result.second)
    std::cout << "Error: Duplicate Header" << std::endl; // throw 40003
  if (key == "Transfer-Encoding" && value.find("chunked") != std::string::npos)
    m_request_.set_m_transfer_type(Request::CHUNKED);
  if (key == "Content-Length") {
    m_request_.set_m_content_length(ft::stoi(value));
    if (m_request_.get_m_content_length() > m_serverconfig_.get_m_client_body_size_limit())
     std::cout << "Content length header value is over than body limit size" << std::endl;
    if (m_request_.get_m_content_length() < 0)
      std::cout << "Content-Length header value is less than 0" << std::endl;
  }
  // if (key[0] == 'X')
  //   ++m_special_header_count_;

  // std::map<std::string, std::string>::iterator iter2;
  // for (iter2 = m_headers_.begin(); iter2 != m_headers_.end(); iter2++)
  //   std::cout << iter2->first << "," << iter2->second << std::endl;
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
  while(ft::getLine(read_buf, line, m_serverconfig_.get_m_request_header_size_limit()) >= 0) {
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
    AddHeader(line);
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
  return (content_length);
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
    return (ReadGeneralBody());
  if (m_request_.get_m_transfer_type() == Request:: CHUNKED)
    return (ReadChunkedBody());
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
	std::string text = "[Failed][Request][Server:" + m_serverconfig_.get_m_server_name() + "][CIP:"
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
  ft::ConvertTimespecToTm(now.tv_sec, &t);
  strftime(buff, sizeof(buff), "%a, %d %b %Y %X GMT", &t);
  return "Last-Modified:" + std::string(buff);
}

std::string Connection::GetServerHeader(Server* server) {
  return "Server:" + m_serverconfig_.get_m_server_name();
}

void Connection::CreateCGIResponse(int& status, headers_t& headers, std::string& body) {
  status = 200;
  headers_t headers_in_body = ft::splitStringByChar(ft::rtrimString(body.substr(0, body.find("\r\n\r\n")), "\r\n"));
  std::string key, value;
  for (headers_t::iterator it = headers_in_body.begin(); it != headers_in_body.end(); it++)
  {
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
  headers.push_back(GetServerHeader(m_server_));

  if (status == CGI_SUCCESS_CODE)
    CreateCGIResponse(status, headers, body);
  if (status >= 400 && status <= 599) {
    body = m_serverconfig_.get_m_default_error_page_path();
    body.replace(body.find("#ERROR_CODE"), 11, ft::to_string(status));
    body.replace(body.find("#ERROR_CODE"), 11, ft::to_string(status));
    body.replace(body.find("#ERROR_DESCRIPTION"), 18, Response::status[status]);
    body.replace(body.find("#ERROR_DESCRIPTION"), 18, Response::status[status]);
    body.replace(body.find("#PORT"), 5, ft::to_string(m_serverconfig_.get_m_port()));
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

void Connection::SolveRequest(const Request& request)
{
  LocationConfig* locationconfig = request.get_m_locationconfig();
  Request::Method method = request.get_m_method();
  std::string methodString = request.get_m_method_to_string();

  if (!ft::hasKey(locationconfig->get_m_allow_method(), methodString)) {
    headers_t headers(1, "Allow:" + ft::containerToString(locationconfig->get_m_allow_method(), ", "));
    return (CreateResponse(40501, headers));
  }
  // if (isAuthorizationRequired(location)) {
  // 	if (!hasCredential(request)) {
  // 		return (makeResponse401(this, request, connection));
  // 	} else {
  // 		std::vector<std::string> credential = ft::splitStringByChar(request.get_m_headers().find("Authorization")->second, ' ');
  // 		if (!isValidCredentialForm(credential))
  // 			return (CreateResponse(40022));
  // 		else if (!isValidCredentialContent(location, credential))
  // 			return (CreateResponse( 40301));
  // 	}
  // }
  if (request.get_m_uri_type() == Request::DIRECTORY)
    return (executeAutoindex(connection, request));
  else if (request.get_m_uri_type() == Request::CGI)
    return (executeCGI(connection, request));
  else if (method == Request::GET)
    executeGet(connection, request);
  else if (method == Request::POST)
    executePost(connection, request);
  else if (method == Request::DELETE)
    executeDelete(connection, request);
  else
    throw (400);
}

void Connection::WriteCreateNewRequestLog(const Request& request)
{
  if (request.get_m_method() != Request::POST)
    return ;
  int cfd = request.get_m_connection()->get_m_client_fd();
  std::string text = ft::GetTimestamp() + "[Created][Request][Server:" + m_serverconfig_.get_m_server_name() + "]"
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
  } catch (Server::IOError& e) {
    throw (e);
  } catch (std::exception& e) {
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