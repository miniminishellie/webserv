/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 20:24:20 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/28 19:01:04 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "ServerConfig.hpp"
#include "Libft.hpp"

ServerConfig::ServerConfig()
    : m_webserv_config_(NULL),
      m_server_name_(),
      m_host_(),
      m_port_(-1),
      m_request_uri_size_limit_(0),
      m_request_header_size_limit_(0),
      m_client_body_size_limit_(0),
      m_default_error_page_path_(),
      m_locations_() {}

ServerConfig::ServerConfig(WebservConfig* wc,
                            const std::string& server_block,
                            const std::vector<std::string>& location_blocks)
    : m_webserv_config_(wc),
      m_server_name_(),
      m_host_(),
      m_port_(-1),
      m_request_uri_size_limit_(0),
      m_request_header_size_limit_(0),
      m_client_body_size_limit_(0),
      m_default_error_page_path_(),
      m_locations_() {
  std::map<std::string, std::string> server_map =
    ft::stringVectorToMap(ft::splitStringByChar(server_block, '\n'), ' ');
  std::string uri;
  std::string block;

  if (ft::hasKey(server_map, "server_name"))
    m_server_name_ = server_map["server_name"];
  else
    m_server_name_ = "no_name";
  m_host_ = server_map["host"];
  m_port_ = ft::stoi(server_map["port"]);
  m_request_uri_size_limit_ = ft::stoi(server_map["REQUEST_URI_LIMIT_SIZE"]);
  m_request_header_size_limit_ = ft::stoi(server_map["REQUEST_HEADER_LIMIT_SIZE"]);
  m_client_body_size_limit_ = ft::stoi(server_map["LIMIT_CLIENT_BODY_SIZE"]);
  m_default_error_page_path_ = ft::getStringFromFile(server_map["DEFAULT_ERROR_PAGE"]);
  for (std::vector<std::string>::const_iterator it = location_blocks.begin();
        it != location_blocks.end();
        ++it) {
    uri = ft::splitStringByChar(ft::splitStringByChar(*it).front(), ' ')[1];
    block = *it;
    block = block.substr(block.find('\n') + 1);
    m_locations_.push_back(LocationConfig(uri, block,
        m_client_body_size_limit_));
  }
}

ServerConfig::ServerConfig(const ServerConfig& sc)
    : m_webserv_config_(sc.m_webserv_config_),
      m_server_name_(sc.m_server_name_),
      m_host_(sc.m_server_name_),
      m_port_(sc.m_port_),
      m_request_uri_size_limit_(sc.m_request_uri_size_limit_),
      m_request_header_size_limit_(sc.m_request_header_size_limit_),
      m_client_body_size_limit_(sc.m_client_body_size_limit_),
      m_default_error_page_path_(sc.m_default_error_page_path_),
      m_locations_(sc.m_locations_) {}

ServerConfig::~ServerConfig() {
  m_server_name_.clear();
  m_host_.clear();
  m_default_error_page_path_.clear();
  m_locations_.clear();
}

ServerConfig& ServerConfig::operator=(const ServerConfig& operand) {
  if (this == &operand)
    return *this;
  m_webserv_config_ = operand.m_webserv_config_;
  m_server_name_ = operand.m_server_name_;
  m_host_ = operand.m_host_;
  m_port_ = operand.m_port_;
  m_request_uri_size_limit_ = operand.m_request_uri_size_limit_;
  m_request_header_size_limit_ = operand.m_request_header_size_limit_;
  m_client_body_size_limit_ = operand.m_client_body_size_limit_;
  m_default_error_page_path_ = operand.m_default_error_page_path_;
  m_locations_ = operand.m_locations_;
  return *this;
}

WebservConfig* ServerConfig::get_m_webserv_config(void) const {
  return m_webserv_config_;
}

std::string ServerConfig::get_m_server_name(void) const {
  return m_server_name_;
}

std::string ServerConfig::get_m_host(void) const {
  return m_host_;
}

int ServerConfig::get_m_port(void) const {
  return m_port_;
}

size_t ServerConfig::get_m_request_uri_size_limit(void) const {
  return m_request_uri_size_limit_;
}

size_t ServerConfig::get_m_request_header_size_limit(void) const {
  return m_request_header_size_limit_;
}

size_t ServerConfig::get_m_client_body_size_limit(void) const {
  return m_client_body_size_limit_;
}

const std::string& ServerConfig::get_m_default_error_page_path(void) const {
  return m_default_error_page_path_;
}

const std::vector<LocationConfig>& ServerConfig::get_m_locations(void) const {
  return m_locations_;
}

std::ostream& operator<<(std::ostream& out, const ServerConfig& config) {
  out << "server_name: " << config.get_m_server_name() << std::endl;
  out << "host: " << config.get_m_host() << std::endl;
  out << "port: " << config.get_m_port() << std::endl;
  out << "request_uri_limit_size: " << config.get_m_request_uri_size_limit() << std::endl;
  out << "request_header_limit_size: " << config.get_m_request_header_size_limit() << std::endl;
  out << "limit_client_body_size: " << config.get_m_client_body_size_limit() << std::endl;
  // out << "default_error_page: " << server.get_m_default_error_page() << std::endl;
  return out;
}
