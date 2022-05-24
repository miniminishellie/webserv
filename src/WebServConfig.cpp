/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 22:09:15 by bylee             #+#    #+#             */
/*   Updated: 2022/05/24 22:13:29 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservConfig.hpp"
#include "Libft.hpp"

/*
  WebservConfig Constructor & Destructor implementation
*/
WebservConfig::WebservConfig()
    : m_software_name_(),
      m_software_version_(),
      m_http_version_(),
      m_cgi_version_(),
      m_base_env_(NULL) {}

WebservConfig::WebservConfig(const WebservConfig& ref)
    : m_software_name_(ref.get_m_software_name()),
      m_software_version_(ref.get_m_software_version()),
      m_http_version_(ref.get_m_http_version()),
      m_cgi_version_(ref.get_m_cgi_version()),
      m_base_env_(ref.get_m_base_env()) {}

WebservConfig::WebservConfig(std::string config_block, char **env){
  std::map<std::string, std::string> map_block =
      ft::stringVectorToMap(ft::splitStringByChar(config_block, '\n'), ' ');

  this->m_software_name_ = map_block.find("SOFTWARE_NAME")->second;
  this->m_software_version_ = map_block.find("SOFTWARE_VERSION")->second;
  this->m_http_version_ = map_block.find("HTTP_VERSION")->second;
  this->m_cgi_version_ = map_block.find("CGI_VERSION")->second;
  this->m_base_env_ = env;
}

WebservConfig& WebservConfig::operator=(const WebservConfig& ref){
  if (this == &ref)
    return (*this);
  this->m_software_name_ = ref.get_m_software_name();
  this->m_software_version_ = ref.get_m_software_version();
  this->m_http_version_ = ref.get_m_http_version();
  this->m_cgi_version_ = ref.get_m_cgi_version();
  this->m_base_env_ = ref.get_m_base_env();
  return (*this);
}

WebservConfig::~WebservConfig(){
  this->m_software_name_.clear();
  this->m_software_version_.clear();
  this->m_http_version_.clear();
  this->m_cgi_version_.clear();
  this->m_base_env_ = NULL;
}

/*
  Getter implementation
*/
std::string WebservConfig::get_m_software_name() const { 
  return (this->m_software_name_);
}
std::string WebservConfig::get_m_software_version() const { 
  return (this->m_software_version_);
}
std::string WebservConfig::get_m_http_version() const { 
  return (this->m_http_version_);
}
std::string WebservConfig::get_m_cgi_version() const { 
  return (this->m_cgi_version_);
}
char      **WebservConfig::get_m_base_env() const { 
  return (this->m_base_env_);
}

std::ostream& operator<<(std::ostream& out, const WebservConfig& config) {
  out << "SOFTWARE_NAME: " << config.get_m_software_name() << std::endl;
  out << "SOFTWARE_VERSION: " << config.get_m_software_version() << std::endl;
  out << "HTTP_VERSION: " << config.get_m_http_version() << std::endl;
  out << "CGI_VERSION: " << config.get_m_cgi_version() << std::endl;
  // out << "CGI_ENVARIABLE:" << std::endl;
  // char **env = config.get_m_base_env();
  // while (env && *env) {
  //   std::cout << *env << std::endl;
  //   env++;
  // }
  return (out);
}