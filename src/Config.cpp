/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:59:39 by bylee             #+#    #+#             */
/*   Updated: 2022/05/22 20:02:36 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Config.hpp"
#include "../include/Libft.hpp"

/*
  Config Constructor & Destructor implementation
*/
Config::Config()
  : m_software_name(),
  m_software_version(),
  m_http_version(),
  m_cgi_version(),
  m_base_env(NULL){}

Config::Config(const Config& ref)
  : m_software_name(ref.get_m_software_name()),
  m_software_version(ref.get_m_software_version()),
  m_http_version(ref.get_m_http_version()),
  m_cgi_version(ref.get_m_cgi_version()),
  m_base_env(ref.get_m_base_env()) {}

Config::Config(std::string config_block, char **env){
  std::vector<std::string> str = splitStringByChar(config_block, '\n');
  // for (std::vector<std::string>::iterator it = str.begin(); it != str.end(); it++)
  //   std::cout << *it << std::endl;
  std::map<std::string, std::string> map_block = stringVectorToMap(splitStringByChar(config_block, '\n'), ' ');

  // for (std::map<std::string, std::string>::iterator it = map_block.begin(); it != map_block.end(); it++)
  //   std::cout << "[" << (*it).first << ", " << (*it).second << "]" << std::endl;
  this->m_software_name = map_block.find("SOFTWARE_NAME")->second;
  this->m_software_version = map_block.find("SOFTWARE_VERSION")->second;
  this->m_http_version = map_block.find("HTTP_VERSION")->second;
  this->m_cgi_version = map_block.find("CGI_VERSION")->second;
  this->m_base_env = env;
}

Config& Config::operator=(const Config& ref){
  if (this == &ref)
    return (*this);
  this->m_software_name = ref.get_m_software_name();
  this->m_software_version = ref.get_m_software_version();
  this->m_http_version = ref.get_m_http_version();
  this->m_cgi_version = ref.get_m_cgi_version();
  this->m_base_env = ref.get_m_base_env();
  return (*this);
}

Config::~Config(){
  this->m_software_name.clear();
  this->m_software_version.clear();
  this->m_http_version.clear();
  this->m_cgi_version.clear();
  this->m_base_env = NULL;
}

/*
  Getter implementation
*/
std::string Config::get_m_software_name() const { return (this->m_software_name); }
std::string Config::get_m_software_version() const { return (this->m_software_version); }
std::string Config::get_m_http_version() const { return (this->m_http_version); }
std::string Config::get_m_cgi_version() const { return (this->m_cgi_version); }
char **Config::get_m_base_env() const { return (this->m_base_env); }

std::ostream& operator<<(std::ostream& out, const Config& config){
  out << "SOFTWARE_NAME: " << config.get_m_software_name() << std::endl;
  out << "SOFTWARE_VERSION: " << config.get_m_software_version() << std::endl;
  out << "HTTP_VERSION: " << config.get_m_http_version() << std::endl;
  out << "CGI_VERSION: " << config.get_m_cgi_version() << std::endl;
  out << "CGI_ENVARIABLE:" << std::endl;
  char **env = config.get_m_base_env();
  while (env && *env) {
    std::cout << *env << std::endl;
    env++;
  }
  return (out);
}