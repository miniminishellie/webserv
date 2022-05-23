/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:27:17 by plee              #+#    #+#             */
/*   Updated: 2022/05/23 21:15:23 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"

Connection::Connection() {
  
}

Connection::Connection(const Connection &Connection) {
  
}

Connection &Connection::operator=(const Connection &connection) {
  
}

Connection::~Connection() {
  
}


bool Connection::ParseMethod(std::string method) {
  if (method == "GET")
    m_request_.set_m_method(Request::GET);
  else if (method == "POST")
    m_request_.set_m_method(Request::POST);
  else if (method == "DELETE")
    m_request_.set_m_method(Request::DELETE);
  else 
    return false;
  return true;
}

void Connection::Parse(std::string start_line) {
  m_request_.set_m_phase(Request::ON_HEADER);
  std::vector<std::string> parsed = Split(start_line, ' ');
  if (parsed.size() != 3)
    std::cout << "StartLine TOKEN _NUM ERROR" << std::endl;
  if (!ParseMethod(parsed[0]))
    std::cout << "StartLine METHOD ERROR" << std::endl;
  if (parsed[1].length() > REQUEST_URI_LIMIT_SIZE)
    std::cout << "StartLine URI_LENGTH ERROR" << std::endl;
  m_request_.set_m_uri(parsed[1]);
  m_request_.set_m_uri_type(Request::FILE);
  m_request_.set_m_protocol(parsed[2]);
  if (m_request_.get_m_protocol != "HTTP/1.1")
    throw (50501);
  //m_special_header_count_ = 0;
  //URI parsing 해야함
  
}

bool Connection::ParseStartLine() {
  size_t new_line;

  if ((new_line = without_body.find("\r\n")) != std::string::npos) {
    std::string start_line = without_body.substr(0, new_line);
    without_body = without_body.substr(new_line + 2);
    Parse(start_line);
    return true;
  }
  else if (m_read_buffer_client_.size() > REQUEST_URI_LIMIT_SIZE_MAX)
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
  if (header.size() > REQUEST_HEADER_LIMIT_SIZE) { //m_server->get_m_request_header_limit_size()
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
  std::string key = trim(header.substr(0, pos));
  std::string value = trim(header.substr(pos + 1));

  for (size_t i = 0 ; i < key.length() ; ++i)
    key[i] = (i == 0 || key[i - 1] == '-') ? std::toupper(key[i]) : std::tolower(key[i]);
  std::pair<std::map<std::string, std::string>::iterator, bool> result = m_headers_.insert(std::make_pair(key, value));
  if (!result.second)
    std::cout << "Error: Duplicate Header" << std::endl; // throw 40003
  if (key == "Transfer-Encoding" && value.find("chunked") != std::string::npos)
    m_request_.set_m_transfer_type(Request::CHUNKED);
  if (key == "Content-Length") {
    m_request_.set_m_content_length(ft_stoi(value));
    if (m_request_.get_m_content_length() > LIMIT_CLIENT_BODY_SIZE)
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
  std::string line;
  std::string& read_buf = m_read_buffer_client_;

  while (getLine(read_buf, line, REQUEST_HEADER_LIMIT_SIZE_MAX) >= 0) {
    if (!IsValidHeader(line)) {
      std::cout << "Error: Header is not Valid" << std::endl;
      return false;
    }
    AddHeader(line);
    line = "";
  }
  AddHeader(line);
  if (!hasKey(m_request_.get_m_headers(),"Host")) {
    std::cout << "Error: Header does not have a Host" << std::endl;
    return false;
  }
  return true;
}

// bool parseHeader(Connection& connection, Request& request)
// {
// 	std::string& rbuf = const_cast<std::string&>(connection.get_m_rbuf_from_client());
// 	std::string line;

// 	while (ft::getline(rbuf, line, REQUEST_HEADER_LIMIT_SIZE_MAX) >= 0)
// 	{
// 		if (line == "")
// 		{
// 			if (!ft::hasKey(request.get_m_headers(), "Host"))
// 				throw (40011);
// 			return (true);
// 		}
// 		if (!request.isValidHeader(line))
// 			throw (40010);
// 		request.addHeader(line);
// 	}
// 	return (false);
// }

void Connection::RecvRequest() {
  
  char buf[BUFFER_SIZE];
  int fd;
  int count = BUFFER_SIZE - m_read_buffer_client_.size();
  Request::Phase phase = m_request_.get_m_phase();
  m_status_ = ON_RECV;

  if (phase == Request::READY && (count = RecvWithoutBody(buf, sizeof(buf))) > 0)
    addReadbufferClient(buf, count);
  if (phase == Request::READY && ParseStartLine())
    phase = Request::ON_HEADER;
  if (phase == Request::ON_HEADER && ParseHeader()) {
    phase = Request::ON_BODY;
    if (!IsRequestHasBody())
      return ;
  }
  if (phase == Request::ON_BODY && (fd = open("./request.txt", O_RDONLY)) > 0  && (count = ReceiveBody(fd, buf, sizeof(buf), body)) > 0) {
    ft_strjoin(body, buf, count);
    close(fd);
  } 
  if (phase == Request::ON_BODY && ParseBody(body))
    phase = Request::COMPLETE;
  if (phase == Request::COMPLETE)
    ;//set_m_last_request_at();
}

void Connection::addReadbufferClient(const char* str, int size) { m_read_buffer_client_.append(str, size); }