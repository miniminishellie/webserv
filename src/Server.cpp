/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:59:53 by bylee             #+#    #+#             */
/*   Updated: 2022/05/24 21:37:45 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

/*
  Server constructor & destructor implementation
*/
Server::Server() {}

Server::Server(ServerManager* server_manager, const std::string& server_block,\
  std::vector<std::string>& location_blocks, Config* config){
  std::map<std::string, std::string> server_map = ft::stringVectorToMap(ft::splitStringByChar(server_block, '\n'), ' ');
  std::string uri;
  std::string block;
  std::string error_page;
  std::string line;

  m_manager = server_manager;
  m_config = config;
  if (ft::hasKey(server_map, "server_name"))
    m_server_name = server_map["server_name"];
  else
    m_server_name = "no_name";
  m_host = server_map["host"];
  m_port = ft::stoi(server_map["port"]);
  m_request_uri_limit_size = ft::stoi(server_map["REQUEST_URI_LIMIT_SIZE"]);
  m_request_header_limit_size = ft::stoi(server_map["REQUEST_HEADER_LIMIT_SIZE"]);
  m_limit_client_body_size = ft::stoi(server_map["LIMIT_CLIENT_BODY_SIZE"]);
  m_default_error_page = ft::getStringFromFile(server_map["DEFAULT_ERROR_PAGE"]);
  for (std::vector<std::string>::iterator it = location_blocks.begin(); it != location_blocks.end(); ++it){
    uri = ft::splitStringByChar(ft::splitStringByChar(*it).front(), ' ')[1];
    block = *it;
    block = block.substr(block.find('\n') + 1);
    m_locations.push_back(Location(uri, block, m_limit_client_body_size));
  }
}

Server::Server(const Server& ref){
  m_manager = ref.m_manager;
  m_server_name = ref.m_server_name;
  m_host = ref.m_host;
  m_port = ref.m_port;
  m_request_uri_limit_size = ref.m_request_uri_limit_size;
  m_request_header_limit_size = ref.m_request_header_limit_size;
  m_limit_client_body_size = ref.m_limit_client_body_size;
  m_default_error_page = ref.m_default_error_page;
  m_config = ref.m_config;
  m_locations = ref.m_locations;
}

Server& Server::operator=(const Server& ref){
  if (this == &ref)
    return (*this);
  m_manager = ref.m_manager;
  m_server_name = ref.m_server_name;
  m_host = ref.m_host;
  m_port = ref.m_port;
  m_request_uri_limit_size = ref.m_request_uri_limit_size;
  m_request_header_limit_size = ref.m_request_header_limit_size;
  m_limit_client_body_size = ref.m_limit_client_body_size;
  m_default_error_page = ref.m_default_error_page;
  m_config = ref.m_config;
  m_locations = ref.m_locations;
  return (*this);
}

Server::~Server() {}

const std::string&            Server::get_m_server_name() const { return (this->m_server_name); }
const std::string&            Server::get_m_host() const { return (this->m_host); }
int                           Server::get_m_port() const { return (this->m_port); }
size_t                        Server::get_m_request_uri_limit_size() const { return (this->m_request_uri_limit_size); }
size_t                        Server::get_m_request_header_limit_size() const { return (this->m_request_header_limit_size); }
size_t                        Server::get_m_limit_client_body_size() const { return (this->m_limit_client_body_size); }
const std::string&            Server::get_m_default_error_page() const { return (this->m_default_error_page); }
Config*                       Server::get_m_config() const { return (this->m_config); }
const std::vector<Location>&  Server::get_m_locations() const { return (this->m_locations); }

std::ostream& operator<<(std::ostream& out, const Server& server)
{
  out << "server_name: " << server.get_m_server_name() << std::endl;
  out << "host: " << server.get_m_host() << std::endl;
  out << "port: " << server.get_m_port() << std::endl;
  out << "request_uri_limit_size: " << server.get_m_request_uri_limit_size() << std::endl;
  out << "request_header_limit_size: " << server.get_m_request_header_limit_size() << std::endl;
  out << "limit_client_body_size: " << server.get_m_limit_client_body_size() << std::endl;
  //out << "default_error_page: " << server.get_m_default_error_page() << std::endl;
  out << "--------CONFIG----------" << std::endl;
  out << *(server.get_m_config()) << std::endl;
  out << "--------LOCATIONS----------" << std::endl;
  for (std::vector<Location>::const_iterator it = server.get_m_locations().begin(); it != server.get_m_locations().end(); ++it) {
    out << *it << std::endl;
  }
  return (out);
}