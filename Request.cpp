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

Request::Request(Connection *connection, Server *server, std::string start_line)
: m_connection_(connection), m_server_(server), m_transfer_type_(GENERAL)
{
  m_request_.set_m_phase(Request::ON_HEADER);
  if (gettimeofday(&m_request_.get_m_start_at(), NULL) == -1)
    throw std::runtime_error("gettimeofday function failed in request generator");

  std::vector<std::string> parsed = split(start_line, ' ');
  if (parsed.size() != 3)
    std::cout << "StartLine TOKEN _NUM ERROR" << std::endl; //throw(40000)
  if (!ParseMethod(parsed[0]))
    std::cout << "StartLine METHOD ERROR" << std::endl; //throw(40001)
  if (parsed[1].length() > REQUEST_URI_LIMIT_SIZE)
    std::cout << "StartLine URI_LENGTH ERROR" << std::endl;//throw(41410)
  m_request_.set_m_uri(parsed[1]);
  m_request_.set_m_uri_type(Request::FILE);
  if (!m_request_.AssignLocationMatchingUri(m_request_.get_m_uri()))
    throw (40401);
  std::string translated_path = ParseUri();
  if (translated_path.empty())
    throw (40002);
  if (isFile(translated_path) && m_uri_type != Request::CGI)
    m_uri_type = Request::FILE;
  else if (isDirectory(translated_path))
    m_uri_type = DIRECTORY;
  else if (m_uri_type != Request::CGI)
    throw (40402);
  m_request_.set_m_protocol(parsed[2]);
  if (m_request_.get_m_protocol != "HTTP/1.1")
    throw (50501);
  //m_special_header_count_ = 0;
  //URI parsing 해야함
}

// Request &Request::operator=(const Request &request) {

// }

Request::~Request() {

}

void Request::AddContent(std::string added_content) {
  if (m_content_.size() + added_content.size() > LIMIT_CLIENT_BODY_SIZE)
    throw (41301);
  m_content_.append(added_content);
}

bool Request::AssignLocationMatchingUri(std::string uri)
{
  size_t max_uri_match = 0;
  for (std::vector<Location>::const_iterator it = m_server->get_m_locations().begin() ; it != m_server->get_m_locations().end() ; ++it) {
    if (std::strncmp(it->get_m_uri().c_str(), uri.c_str(), it->get_m_uri().length()) == 0 && it->get_m_uri().length() > max_uri_match) {
      m_location = const_cast<Location *>(&(*it));
      max_uri_match = it->get_m_uri().length();
    }
  }
  if (!max_uri_match)
    return false;
  return true;
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
timeval& get_m_start_at() { return m_start_at_ }

//setter
void Request::set_m_phase(Phase phase) { m_phase_ = phase; }
void Request::set_m_method(Method method) { m_method_ = method; }
void Request::set_m_uri_type(URIType uritype) { m_uri_type_ = uritype; }
void Request::set_m_transfer_type(TransferType transfertype) { m_transfer_type_ = transfertype; }
void Request::set_m_uri(std::string uri) { m_uri_ = uri; }
void Request::set_m_protocol(std::string protocol) { m_protocol_ = protocol; }
void Request::set_m_content(std::string content) { m_content_ = content; }
void Request::set_m_content_length(int length) { m_content_length_ = length; }
