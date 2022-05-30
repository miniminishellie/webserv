/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/28 15:27:34 by plee              #+#    #+#             */
/*   Updated: 2022/05/30 20:46:23 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Connection.hpp"
#include "Libft.hpp"

std::map<int, std::string> make_status () {
  std::map<int, std::string> status_map;

  status_map[100] = "Continue";
  status_map[200] = "OK";
  status_map[201] = "Created";
  status_map[202] = "Accepted";
  status_map[204] = "No Content";
  status_map[205] = "Reset Content";
  status_map[206] = "Partial Content";
  status_map[299] = "CGI OK";
  status_map[300] = "Multiple Choice";
  status_map[301] = "Moved Permanently";
  status_map[303] = "See Other";
  status_map[305] = "Use Proxy";
  status_map[307] = "Temporary Redirect";
  status_map[400] = "Bad Request";
  status_map[401] = "Unauthorized";
  status_map[403] = "Forbidden";
  status_map[404] = "Not Found";
  status_map[405] = "Method Not Allowed";
  status_map[406] = "Not Acceptable";
  status_map[407] = "Proxy Authentication Required";
  status_map[408] = "Request Timeout";
  status_map[409] = "Conflict";
  status_map[410] = "Gone";
  status_map[411] = "Length Required";
  status_map[412] = "Precondition Failed";
  status_map[413] = "Payload Too Large";
  status_map[414] = "URI Too Long";
  status_map[415] = "Unsupported Media Type";
  status_map[416] = "Requested Range Not Satisfiable";
  status_map[417] = "Expectation Failed";
  status_map[500] = "Internal Server Error";
  status_map[501] = "Not Implemented";
  status_map[503] = "Service Unavailable";
  status_map[504] = "Gateway Timeout";
  status_map[505] = "HTTP Version Not Supported";

  status_map[40000] = "Bad Request: start line element count is not 3";
  status_map[40001] = "Bad Request: Method is not normal";
  status_map[40002] = "Bad Request: URI parse failed(translated-path is empty)";
  status_map[40003] = "Bad Request: failed to add request header to map(maybe: duplicate header)";
  status_map[40004] = "Bad Request: Content-Length header value is less than 0";
  status_map[40005] = "Bad Request: header size is greater than request header limit size";
  status_map[40006] = "Bad Request: start line size is greater than request uri limit size";
  status_map[40007] = "Bad Request: Failed to get start line(recv function failure)";
  status_map[40008] = "Bad Request: header line size is greater than request header limit size";
  status_map[40009] = "Bad Request: Failed to get header line(recv function failure)";
  status_map[40010] = "Bad Request: Unvalid Header(maybe not found ':')";
  status_map[40011] = "Bad Request: Not Found host header";
  status_map[40012] = "Bad Request: In general request, failed to read content by large content-length";
  status_map[40013] = "Bad Request: In general request, failed to read content by normal content-length";
  status_map[40014] = "Bad Request: In general request, readed-size and content-length value is not equal";
  status_map[40015] = "Bad Request: In chunked request, failed to read trnasfer-size";
  status_map[40016] = "Bad Request: In chunked request, failed to convert trnasfer-size(maybe negative number)";
  status_map[40017] = "Bad Request: In chunked request, failed to convert trnasfer-size(maybe not number)";
  status_map[40018] = "Bad Request: In chunked request, for of end-line is not '\\r\\n'";
  status_map[40019] = "Bad Request: In chunked request, failed to read content by large content-length";
  status_map[40020] = "Bad Request: In chunked request, failed to read content by normal content-length";
  status_map[40021] = "Bad Request: In chunked request, readed-size and content-length value is not equal";
  status_map[40022] = "Bad Request: Credential Form unvalid";
  status_map[40023] = "Bad Request: Not CGI-prgoram, POST method, Content-Length is not 0";
  
  status_map[40101] = "Unauthorized";
  status_map[40301] = "Forbidden: Credential Content unvalid";	
  status_map[40401] = "Not Found: No suitable location";
  status_map[40402] = "Not Found: Requested uri is not in server";
  status_map[40403] = "Not Found: Autoindex off, index file is not found";
  status_map[40404] = "Not Found: CGI file is not found";
  status_map[40501] = "Method Not Allowed";
  status_map[40502] = "Bad Request: Access Directory other than GET method";
  status_map[41101] = "Length Required";
  status_map[41301] = "Payload Too Large: Failed to add content in request";
  status_map[41302] = "Payload Too Large: Failed to add origin in request";
  status_map[41303] = "Payload Too Large: Content length header value is over than body limit size";
  status_map[41304] = "Payload Too Large: File size is too large in GET method";
  status_map[41305] = "Payload Too Large: File size is too large in HEAD method";
  status_map[41306] = "Payload Too Large: CGI output size is too large";
  status_map[41307] = "Payload Too Large: LOCATION SIZE OVER";
  status_map[41401] = "Bad Request: uri size is greater than request uri limit size";
  status_map[41501] = "Unsupported Media Type: in GET method";
  status_map[41502] = "Unsupported Media Type: in HEAD method";
  status_map[41503] = "Unsupported Media Type: in PUT method";

  status_map[50001] = "Internal Server Error: recvRequest function throw std::exception";
  status_map[50002] = "Internal Server Error: MakeAutoindex function failed";
  status_map[50003] = "Internal Server Error: createCGIEnv function return NULL";
  status_map[50004] = "Internal Server Error: Failed to fork process for executeCGI";
  status_map[50301] = "Internal Server Error: Too many Stacked Response exists";
  status_map[50401] = "Gateway Timeout";
  status_map[50501] = "HTTP Version Not Supported";

  return status_map;
}

std::map<int, std::string> Response::status = make_status();

Response::Response()
{
  this->m_connection_type_ = KEEP_ALIVE;
  this->m_transfer_type_ = GENERAL;
  this->m_phase_ = READY;
}

Response::Response(Connection* connection, int status_code, std::string body)
{
  this->m_connection_ = connection;
  this->m_connection_type_ = KEEP_ALIVE;
  this->m_status_code_ = status_code;
  this->m_status_description_ = Response::status[status_code];
  this->m_content_ = body;
  this->m_transfer_type_ = GENERAL;
  this->m_phase_ = READY;
}

Response::Response(const Response& r)
{
  this->m_connection_ = r.get_m_connection();
  this->m_connection_type_ = r.get_m_connection_type();
  this->m_status_code_ = r.get_m_status_code();
  this->m_status_description_ = r.get_m_status_description();
  this->m_transfer_type_ = r.get_m_transfer_type();
  this->m_content_ = r.get_m_content();
  this->m_headers_ = r.get_m_headers();
  this->m_phase_ = r.get_m_phase();
}

Response& Response::operator=(const Response& ref) {
  if (this == &ref)
    return *this;
  this->m_connection_ = ref.get_m_connection();
  this->m_connection_type_ = ref.get_m_connection_type();
  this->m_status_code_ = ref.get_m_status_code();
  this->m_status_description_ = ref.get_m_status_description();
  this->m_transfer_type_ = ref.get_m_transfer_type();
  this->m_content_ = ref.get_m_content();
  this->m_headers_ = ref.get_m_headers();
  this->m_phase_ = ref.get_m_phase();
  return *this;
}

Response::~Response() {}

/*getter function*/
Connection *Response::get_m_connection() const { return m_connection_; }
Response::ConnectionType Response::get_m_connection_type() const { return m_connection_type_; }
int Response::get_m_status_code() const { return m_status_code_; }
std::string Response::get_m_status_description() const { return m_status_description_; }
const std::map<std::string, std::string>& Response::get_m_headers() const { return m_headers_; }
Response::TransferType Response::get_m_transfer_type() const { return m_transfer_type_; }
std::string Response::get_m_content() const { return m_content_; }
Response::Phase Response::get_m_phase() const { return m_phase_; }

/*setter function*/
void Response::addHeader(std::string header_key, std::string header_value)
{
  if (header_key == "Transfer-Encoding" && header_value.find("chunked") != std::string::npos)
    this->m_transfer_type_ = CHUNKED;
  else if (header_key == "Connection" && header_value == "close")
    this->m_connection_type_ = CLOSE;
  else
    this->m_headers_[header_key] = header_value;
}
void Response::addContent(const std::string& body) { m_content_ += body; }
void Response::set_m_status_code(int status_code) { m_status_code_ = status_code; }
void Response::set_m_transfer_type(TransferType transfer_type) { m_transfer_type_ = transfer_type; }
void Response::set_m_phase(Phase phase) { m_phase_ = phase; }
void Response::clear() {
  m_status_code_ = -1;
  m_status_description_.clear();
  m_headers_.clear();
  m_transfer_type_ = GENERAL;
  m_content_.clear();
  m_phase_ = READY;
}

/*member function*/
std::string Response::GetString() const {	
  std::string message;
  std::map<std::string, std::string>::const_iterator it = this->m_headers_.begin();

  message = "HTTP/1.1 " + ft::to_string(this->m_status_code_) + " " + this->m_status_description_ + "\r\n";
  for (; it != this->m_headers_.end(); ++it)
    message += it->first + ": " + it->second + "\r\n";
  if (m_connection_type_ == CLOSE || m_status_code_ < 200 || m_status_code_ > 299)
    message += "Connection: close\r\n";
  else
    message += "Connection: Keep-Alive\r\n";
  if (m_transfer_type_ == CHUNKED) {
    message += "Transfer-Encoding: chunked\r\n\r\n";
    int size = this->m_content_.size();
    int count;
    std::string data = m_content_;
    int added = 0;
    while (size > 0) {
      if (size > BUFFER_SIZE)
        count = BUFFER_SIZE;
      else
        count = size;
      message += ft::itos(ft::to_string(count), 10, 16) + "\r\n";
      message += data.substr(added, count) + "\r\n";
      size -= count;
      added += count;
    }
    data.clear();
    message += "0\r\n\r\n";
  }
  else {
    message += "\r\n";
    message += this->m_content_;
  }
  return message;
}