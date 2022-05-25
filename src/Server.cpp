/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:59:53 by bylee             #+#    #+#             */
/*   Updated: 2022/05/25 17:52:37 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "ServerManager.hpp"
#include "Libft.hpp"

/*
  Server constructor & destructor implementation
*/
Server::Server(void) {}

Server::Server(ServerManager* sm, WebservConfig* wc,
                const std::string& server_block,
                std::vector<std::string>& location_blocks)
    : m_server_manager_(sm),
      m_webserv_config_(wc),
      m_config_(server_block) {
  std::string uri;
  std::string block;

  for (std::vector<std::string>::iterator it = location_blocks.begin();
        it != location_blocks.end();
        ++it) {
    uri = ft::splitStringByChar(ft::splitStringByChar(*it).front(), ' ')[1];
    block = *it;
    block = block.substr(block.find('\n') + 1);
    m_locations_.push_back(LocationConfig(uri, block,
        m_config_.get_m_client_body_size_limit()));
  }
}

Server::Server(const Server& ref)
    : m_server_manager_(ref.m_server_manager_),
      m_webserv_config_(ref.m_webserv_config_),
      m_config_(ref.m_config_),
      m_locations_(ref.m_locations_),
      m_connections_(ref.m_connections_) { }

Server::~Server() {}

Server& Server::operator=(const Server& ref){
  if (this == &ref)
    return (*this);
  m_server_manager_ = ref.m_server_manager_;
  m_webserv_config_ = ref.m_webserv_config_;
  m_config_ = ref.m_config_;
  m_locations_ = ref.m_locations_;
  m_connections_ = ref.m_connections_;
  return (*this);
}

ServerManager* Server::get_m_server_manager() const {
  return m_server_manager_;
}
WebservConfig* Server::get_m_webserv_config() const {
  return m_webserv_config_;
}
ServerConfig Server::get_m_config() const {
  return m_config_;
}
const std::vector<LocationConfig>& Server::get_m_locations() const {
  return m_locations_;
}
const std::map<int, Connection>& Server::get_m_connections() const {
  return m_connections_;
}

std::ostream& operator<<(std::ostream& out, const Server& server) {
  out << server.get_m_config() << std::endl;
  out << "--------CONFIG----------" << std::endl;
  out << *(server.get_m_webserv_config()) << std::endl;
  out << "--------LOCATIONS----------" << std::endl;
  for (std::vector<LocationConfig>::const_iterator it = server.get_m_locations().begin();
        it != server.get_m_locations().end();
        ++it) {
    out << *it << std::endl;
  }
  return (out);
}

// void Server::initServerSocket(void) {
//   if ((m_socket_fd_ = socket(PF_INET, SOCK_STREAM, 0)) == -1)
//     throw std::runtime_error("socket() Error");

//   int opt = 1;
//   struct sockaddr_in serv_addr;

//   setsockopt(m_socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
//   memset(&serv_addr, 0, sizeof(serv_addr));
//   serv_addr.sin_family = AF_INET;
//   serv_addr.sin_addr.s_addr = inet_addr(m_config_.get_m_host().c_str());
//   serv_addr.sin_port = htons(m_config_.get_m_port());
//   if (bind(m_socket_fd_,
//             reinterpret_cast<struct sockaddr *>(&serv_addr),
//             sizeof(serv_addr)) == -1)
//     throw std::runtime_error("bind() Error");
//   if (listen(m_socket_fd_, 256) == -1)  //  backlog 크기는 나중에 test 해보면서 수정해봐야할듯?
//     throw std::runtime_error("listen() Error");
//   if (fcntl(m_socket_fd_, F_SETFL, O_NONBLOCK) == -1)
//     throw std::runtime_error("fcntl() Error");
// }
