/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:59:53 by bylee             #+#    #+#             */
/*   Updated: 2022/05/24 14:29:40 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(){
  
}

Server::Server(ServerManager* server_manager, const std::string& server_block, std::vector<std::string>& location_blocks, Config* config){
  	std::map<std::string, std::string> server_map = ft::stringVectorToMap(ft::splitStringByChar(server_block, '\n'), ' ');
  // struct sockaddr_in server_addr;
  // std::string uri;
  // std::string block;
  // std::string error_page;
  // std::string line;

  // m_manager = server_manager;
  // m_config = config;
  // if (ft::hasKey(server_map, "server_name"))
  //   m_server_name = server_map["server_name"];
  // else
  //   m_server_name = "noname";
  // m_host = server_map["host"];
  // m_port = ft::stoi(server_map["port"]);
  // m_request_uri_limit_size = ft::stoi(server_map["REQUEST_URI_LIMIT_SIZE"]);
  // m_request_header_limit_size = ft::stoi(server_map["REQUEST_HEADER_LIMIT_SIZE"]);
  // m_limit_client_body_size = ft::stoi(server_map["LIMIT_CLIENT_BODY_SIZE"]);
  // m_default_error_page = ft::getStringFromFile(server_map["DEFAULT_ERROR_PAGE"]);

}

Server::Server(const Server& ref){
  
}

Server& Server::operator=(const Server& ref){
  
}

Server::~Server(){
  
}