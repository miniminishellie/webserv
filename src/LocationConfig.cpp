/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationConfig.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/25 12:43:59 by jihoolee          #+#    #+#             */
/*   Updated: 2022/06/18 17:40:42 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "LocationConfig.hpp"
#include "Libft.hpp"

LocationConfig::LocationConfig(void) {}

LocationConfig::LocationConfig(const std::string& location_uri,
                                std::string location_block,
                                size_t server_limit_client_body_size)
    : m_uri_(location_uri) {
  std::map<std::string, std::string>map_block =
    ft::stringVectorToMap(ft::splitStringByChar(location_block, '\n'), ' ');
  this->m_root_path_ = map_block.find("root")->second;
  if (ft::hasKey(map_block, "auth_basic_realm"))
    this->m_auth_basic_realm_ = map_block.find("auth_basic_realm")->second;
  if (ft::hasKey(map_block, "auth_basic_file")) {
    std::vector<std::string> content =
      ft::splitStringByChar(
          ft::getStringFromFile(map_block.find("auth_basic_file")->second),
          '\n');
    for (size_t i = 0; i < content.size(); ++i) {
      std::vector<std::string> v = ft::splitStringByChar(content[i], ':');
      if (v.size() != 2 ||
          ft::trimString(v[0]).empty() ||
          ft::trimString(v[1]).empty())
        throw std::invalid_argument("auth_basic_file format is invalid");
      std::string key = ft::trimString(v[0]);
      std::string value = ft::trimString(v[1]);
      this->m_auth_basic_file_[key] = value;
    }
  }
  if (ft::hasKey(map_block, "allow_method")) {
    this->m_allow_method_ =
      ft::stringVectorToSet(
          ft::splitStringByChar(map_block.find("allow_method")->second, ' '));
  } else {
    this->m_allow_method_.insert("GET");
    this->m_allow_method_.insert("HEAD");
  }
  if (ft::hasKey(map_block, "index"))
    this->m_index_ =
      ft::stringVectorToSet(
          ft::splitStringByChar(map_block.find("index")->second, ' '));
  if (ft::hasKey(map_block, "cgi"))
    this->m_cgi_ =
      ft::stringVectorToSet(
          ft::splitStringByChar(map_block.find("cgi")->second, ' '));
  this->m_autoindex_ =
    ft::hasKey(map_block, "autoindex") &&
    map_block.find("autoindex")->second == "on";
  if (ft::hasKey(map_block, "limit_client_body_size"))
    this->m_limit_client_body_size_ =
      ft::stoi(map_block["limit_client_body_size"]);
  else
    this->m_limit_client_body_size_ = server_limit_client_body_size;
  if (ft::hasKey(map_block, "redirect"))
    this->m_redirect_uri_ = map_block.find("redirect")->second;
}

LocationConfig::LocationConfig(const LocationConfig& ref)
    : m_uri_(ref.m_uri_),
      m_root_path_(ref.m_root_path_),
      m_auth_basic_realm_(ref.m_auth_basic_realm_),
      m_auth_basic_file_(ref.m_auth_basic_file_),
      m_allow_method_(ref.m_allow_method_),
      m_index_(ref.m_index_),
      m_cgi_(ref.m_cgi_),
      m_limit_client_body_size_(ref.m_limit_client_body_size_),
      m_autoindex_(ref.m_autoindex_),
      m_redirect_uri_(ref.m_redirect_uri_) {}

LocationConfig::~LocationConfig() {}

LocationConfig& LocationConfig::operator=(const LocationConfig& ref) {
  if (this == &ref)
    return *this;
  m_uri_ = ref.m_uri_;
  m_root_path_ = ref.m_root_path_;
  m_auth_basic_realm_ = ref.m_auth_basic_realm_;
  m_auth_basic_file_ = ref.m_auth_basic_file_;
  m_allow_method_ = ref.m_allow_method_;
  m_index_ = ref.m_index_;
  m_cgi_ = ref.m_cgi_;
  m_limit_client_body_size_ = ref.m_limit_client_body_size_;
  m_autoindex_ = ref.m_autoindex_;
  m_redirect_uri_ = ref.m_redirect_uri_;
  return *this;
}

std::string                               LocationConfig::get_m_uri() const {
  return m_uri_;
}
std::string                               LocationConfig::get_m_root_path() const {
  return m_root_path_;
}
std::string                               LocationConfig::get_m_auth_basic_realm() const {
  return m_auth_basic_realm_;
}
const std::map<std::string, std::string>& LocationConfig::get_m_auth_basic_file() const {
  return m_auth_basic_file_;
}
std::set<std::string>                     LocationConfig::get_m_allow_method() const {
  return m_allow_method_;
}
const std::set<std::string>&              LocationConfig::get_m_index() const {
  return m_index_;
}
const std::set<std::string>&              LocationConfig::get_m_cgi() const {
  return m_cgi_;
}
const bool&                               LocationConfig::get_m_autoindex() const {
  return m_autoindex_;
}
size_t                                    LocationConfig::get_m_limit_client_body_size() const {
  return m_limit_client_body_size_;
}
std::string                               LocationConfig::get_m_redirect_uri() const {
  return m_redirect_uri_;
}

std::ostream&	operator<<(std::ostream& out, const LocationConfig& location) {
  out << "URI: " << location.get_m_uri() << std::endl;
  out << "ROOT_PATH: " << location.get_m_root_path() << std::endl;
  out << "AUTH_BASIC_REALM: " << location.get_m_auth_basic_realm() << std::endl;
  out << "ALLOW_METHOD: " << ft::containerToString(location.get_m_allow_method(), "\n") << std::endl;
  out << "INDEX: " << ft::containerToString(location.get_m_index(), "\n") << std::endl;
  out << "CGI: " << ft::containerToString(location.get_m_cgi(), "\n") << std::endl;
  out << "AUTOINDEX: " << std::boolalpha << location.get_m_autoindex()<< std::endl;
  out << "REDIRECT: " << location.get_m_redirect_uri() << std::endl;
  return out;
}
