/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: plee <plee@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 16:26:22 by plee              #+#    #+#             */
/*   Updated: 2022/05/23 16:26:22 by plee             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request() {
  // m_connection = NULL;
	// m_server = NULL;
	// m_location = NULL;
	// m_start_at.tv_sec = 0;
	// m_start_at.tv_usec = 0;
	m_phase_ = READY;
	m_method_ = DEFAULT;
	m_uri_type_ = FILE;
	m_transfer_type_ = GENERAL;
	m_special_header_count_ = 0;
}

Request::Request(const Request &request) {

}

// Request &Request::operator=(const Request &request) {

// }

Request::~Request() {

}

//getter
Request::Phase Request::get_m_phase() const { return m_phase_; }
Request::Method Request::get_m_method() const { return m_method_; }
Request::URIType Request::get_m_uri_type() const { return m_uri_type_; }
Request::TransferType Request::get_m_transfer_type() const { return m_transfer_type_; }
std::string Request::get_m_uri() const { return m_uri_; }
std::string Request::get_m_protocol() const { return m_protocol_; }
std::map<std::string, std::string>& Request::get_m_headers() { return  m_headers_; }
std::string Request::get_m_content() const { return m_content_; }
int Request::get_m_content_length() const { return m_content_length_; }

//setter
void Request::set_m_phase(Phase phase) { m_phase_ = phase; }
void Request::set_m_method(Method method) { m_method_ = method; }
void Request::set_m_uri_type(URIType uritype) { m_uri_type_ = uritype; }
void Request::set_m_transfer_type(TransferType transfertype) { m_transfer_type_ = transfertype; }
void Request::set_m_uri(std::string uri) { m_uri_ = uri; }
void Request::set_m_protocol(std::string protocol) { m_protocol_ = protocol; }


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
  if ((read_size = read(fd, buf, BUFFER_SIZE)) > 0) {
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