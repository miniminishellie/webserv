/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/26 14:54:39 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/26 15:57:21 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Server.hpp"
#include "Connection.hpp"
#include "Libft.hpp"

Request::Request(void)
    : m_server_(NULL),
      m_connection_(NULL),
      m_phase_(READY),
      m_method_(DEFAULT),
      m_uri_type_(FILE),
      m_transfer_type_(GENERAL),
      m_uri_(),
      m_protocol_(),
      m_headers_(),
      m_special_header_count_(0),
      m_query_(),
      m_script_translated_(),
      m_parth_translated_(),
      m_path_info_() {
  m_start_at_.tv_sec = 0;
  m_start_at_.tv_usec = 0;
}

Request::Request(Connection* connection, Server* server, std::string start_line)
    : m_connection_(connection),
      m_server_(server),
      m_transfer_type_(GENERAL),
      m_phase_(ON_HEADER) {
  if (gettimeofday(&m_start_at_, NULL) == -1)
    throw std::runtime_error("gettimeofday function failed in request generator");
  std::vector<std::string> parsed = split(start_line, ' ');
  if (parsed.size() != 3)
    std::cout << "StartLine TOKEN _NUM ERROR" << std::endl; //throw(40000)
  if (!ParseMethod(parsed[0]))
    std::cout << "StartLine METHOD ERROR" << std::endl; //throw(40001)
  if (parsed[1].length() > REQUEST_URI_LIMIT_SIZE)
    std::cout << "StartLine URI_LENGTH ERROR" << std::endl;//throw(41410)
  m_uri_ = parsed[1];
  m_uri_type_ = FILE;
  if (!AssignLocationMatchingUri(m_uri_))
    throw (40401);
  std::string translated_path = ParseUri();
  if (translated_path.empty())
    throw (40002);
  if (isFile(translated_path) && m_uri_type_ != Request::CGI)
    m_uri_type_ = Request::FILE;
  else if (isDirectory(translated_path))
    m_uri_type_ = DIRECTORY;
  else if (m_uri_type_ != Request::CGI)
    throw (40402);
  m_protocol_ = parsed[2];
  if (m_protocol_ != "HTTP/1.1")
    throw (50501);
  //m_special_header_count_ = 0;
  //URI parsing 해야함
}

Request::Request(const Request &r)
    : m_server_(r.m_server_),
      m_connection_(r.m_connection_),
      m_phase_(r.m_phase_),
      m_method_(r.m_method_),
      m_uri_type_(r.m_uri_type_),
      m_transfer_type_(r.m_transfer_type_),
      m_uri_(r.m_uri_),
      m_protocol_(r.m_protocol_),
      m_headers_(r.m_headers_),
      m_special_header_count_(r.m_special_header_count_),
      m_query_(r.m_query_),
      m_script_translated_(r.m_script_translated_),
      m_parth_translated_(r.m_parth_translated_),
      m_path_info_(r.m_path_info_) { }

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

std::string Request::GetTranslatedPath(std::string root, std::string uri) {
  if (uri.empty())
    return root;
  if (root[root.size() - 1] == '/' && uri[0] == '/')
    uri.erase(uri.begin());
  else if (root[root.size() - 1] != '/' && uri[0] != '/')
    uri.insert(0, 1, '/');
  return root + uri;
}
/*
** if uri is directory, find resource that actually exists in the server
** @param1: list of index files
** @param2: directory of index file exists
** @return: path of location index file
*/
std::string Request::GetIndexPath(const std::set<std::string>& index_set, std::string base_path) {
  std::set<std::string>::const_iterator it = index_set.begin();
  struct stat buf;
  std::string path;

  for (; it != index_set.end(); ++it) {
    path = GetTranslatedPath(base_path, *it);
    stat(path.c_str(), &buf);
    if (S_ISREG(buf.st_mode))
      return *it;
  }
  return "";
}

std::string Request::ParseUri() {
  std::string root = m_location->get_m_uri();
  std::string uri = (root == "/") ? m_uri_ : m_uri_.substr(m_uri_.find(root) + root.size());
  std::string main_path = uri;
  std::string refer_path = uri;

  if (isDirectory(GetTranslatedPath(m_location->get_m_root_path(), uri)) && !m_location->get_m_autoindex())
    uri = m_uri_ = GetIndexPath(get_m_location()->get_m_index(), GetTranslatedPath(m_location->get_m_root_path(), uri));
  for (std::set<std::string>::const_iterator it = m_location->get_m_cgi().begin() ; it != m_location->get_m_cgi().end() ; ++it) {
    if (uri.find(*it) != std::string::npos) {
      int idx = uri.find(*it);
      m_uri_type_ = CGI;
      if (uri.find("?") != std::string::npos) {
        m_query_ = uri.substr(uri.find("?") + 1);
        uri = uri.substr(0, uri.find("?"));
        m_path_info_ = m_uri_.substr(0, uri.find("?"));
      } else
        m_path_info_ = m_uri_;
      main_path = uri.substr(0, idx + it->size());
      refer_path = uri.substr(idx + it->size());
      break ;
    }
  }
  m_script_translated_ = GetTranslatedPath(m_location->get_m_root_path(), main_path);
  m_path_translated_ = GetTranslatedPath(m_location->get_m_root_path(), refer_path);
  if (m_uri_type_ == CGI && !isFile(m_script_translated_)) {
    if (!m_location->get_m_index().empty()) {
      m_method_ = GET;
      m_uri_ = m_location->get_m_uri();
      m_script_translated_ = m_location->get_m_root_path();
    }
    else
      throw (40404);
  }
  return m_script_translated_;
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
timeval& Request::get_m_start_at() { return m_start_at_; }

//setter
void Request::set_m_phase(Phase phase) { m_phase_ = phase; }
void Request::set_m_method(Method method) { m_method_ = method; }
void Request::set_m_uri_type(URIType uritype) { m_uri_type_ = uritype; }
void Request::set_m_transfer_type(TransferType transfertype) { m_transfer_type_ = transfertype; }
void Request::set_m_uri(std::string uri) { m_uri_ = uri; }
void Request::set_m_protocol(std::string protocol) { m_protocol_ = protocol; }
void Request::set_m_content(std::string content) { m_content_ = content; }
void Request::set_m_content_length(int length) { m_content_length_ = length; }
