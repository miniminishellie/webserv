/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 22:09:15 by bylee             #+#    #+#             */
/*   Updated: 2022/05/31 20:22:47 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservConfig.hpp"
#include "Libft.hpp"

namespace {
void make_mime_type(std::map<std::string, std::string>& m_mime_types_) {
	m_mime_types_["avi"] = "video/x-msvivdeo";
	m_mime_types_["bin"] = "application/octet-stream";
	m_mime_types_["bmp"] = "image/bmp";
	m_mime_types_["css"] = "text/css";
	m_mime_types_["csv"] = "text/csv";
	m_mime_types_["doc"] = "application/msword";
	m_mime_types_["docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	m_mime_types_["gz"] = "application/gzip";
	m_mime_types_["gif"] = "image/gif";
	m_mime_types_["htm"] = "text/html";
	m_mime_types_["html"] = "text/html";
	m_mime_types_["ico"] = "image/vnd.microsoft.icon";
	m_mime_types_["jepg"] = "image/jepg";
	m_mime_types_["jpg"] = "image/jepg";
	m_mime_types_["js"] = "text/javascript";
	m_mime_types_["json"] = "application/json";
	m_mime_types_["mp3"] = "audio/mpeg";
	m_mime_types_["mpeg"] = "video/mpeg";
	m_mime_types_["png"] = "image/png";
	m_mime_types_["pdf"] = "apllication/pdf";
	m_mime_types_["php"] = "application/x-httpd-php";
	m_mime_types_["ppt"] = "application/vnd.ms-powerpoint";
	m_mime_types_["pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	m_mime_types_["rar"] = "application/vnd.rar";
	m_mime_types_["sh"] = "application/x-sh";
	m_mime_types_["svg"] = "image/svg+xml";
	m_mime_types_["tar"] = "application/x-tar";
	m_mime_types_["tif"] = "image/tiff";
	m_mime_types_["txt"] = "text/plain";
	m_mime_types_["wav"] = "audio/wav";
	m_mime_types_["xls"] = "application/xhtml+xml";
	m_mime_types_["xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	m_mime_types_["zip"] = "application/zip";
	m_mime_types_["bad_extension"] = "application/bad";
	m_mime_types_["bla"] = "application/42cgi";
	m_mime_types_["pouic"] = "application/pouic";
}
}  //  anonymous namesapce

/*
  WebservConfig Constructor & Destructor implementation
*/
WebservConfig::WebservConfig()
    : m_software_name_(),
      m_software_version_(),
      m_http_version_(),
      m_cgi_version_(),
      m_base_env_(NULL),
      m_mime_types_() {
  make_mime_type(m_mime_types_);
}

WebservConfig::WebservConfig(const WebservConfig& ref)
    : m_software_name_(ref.get_m_software_name()),
      m_software_version_(ref.get_m_software_version()),
      m_http_version_(ref.get_m_http_version()),
      m_cgi_version_(ref.get_m_cgi_version()),
      m_base_env_(ref.get_m_base_env()),
      m_mime_types_(ref.get_m_mime_types()) {}

WebservConfig::WebservConfig(std::string config_block, char **env){
  std::map<std::string, std::string> map_block =
      ft::stringVectorToMap(ft::splitStringByChar(config_block, '\n'), ' ');

  this->m_software_name_ = map_block.find("SOFTWARE_NAME")->second;
  this->m_software_version_ = map_block.find("SOFTWARE_VERSION")->second;
  this->m_http_version_ = map_block.find("HTTP_VERSION")->second;
  this->m_cgi_version_ = map_block.find("CGI_VERSION")->second;
  this->m_base_env_ = env;
  make_mime_type(this->m_mime_types_);
}

WebservConfig& WebservConfig::operator=(const WebservConfig& ref){
  if (this == &ref)
    return (*this);
  this->m_software_name_ = ref.get_m_software_name();
  this->m_software_version_ = ref.get_m_software_version();
  this->m_http_version_ = ref.get_m_http_version();
  this->m_cgi_version_ = ref.get_m_cgi_version();
  this->m_base_env_ = ref.get_m_base_env();
  this->m_mime_types_ = ref.get_m_mime_types();
  return (*this);
}

WebservConfig::~WebservConfig(){
  this->m_software_name_.clear();
  this->m_software_version_.clear();
  this->m_http_version_.clear();
  this->m_cgi_version_.clear();
  this->m_base_env_ = NULL;
  this->m_mime_types_.clear();
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

const std::map<std::string, std::string>& WebservConfig::get_m_mime_types() const {
  return this->m_mime_types_;
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
