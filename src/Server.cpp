/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:59:53 by bylee             #+#    #+#             */
/*   Updated: 2022/05/28 20:08:34 by jihoolee         ###   ########.fr       */
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

int Server::get_m_socket_fd() const {
  return m_socket_fd_;
}

const std::vector<LocationConfig>& Server::get_m_locations() const {
  return m_locations_;
}
const std::map<int, Connection>& Server::get_m_connections() const {
  return m_connections_;
}

bool Server::acceptNewConnection(void) {
  struct sockaddr_in  client_addr;
  socklen_t           client_addr_size = sizeof(struct sockaddr);
  int                 client_fd;
  std::string         client_ip;
  int                 client_port;

  bzero(&client_addr, client_addr_size);
  if ((client_fd = accept(m_socket_fd_,
        (struct sockaddr*)&client_addr, &client_addr_size)) == -1) {
    std::cerr << "accpt() function for client_fd failed" << std::endl;
    return false;
  }
  if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    return false;
  // setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
  // setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, (struct timeval*)&tv, sizeof(struct timeval));
  m_server_manager_->addEvent(client_fd, EVFILT_READ, EV_ADD | EV_ENABLE,
                                0, 0, NULL);
  m_server_manager_->addEvent(client_fd, EVFILT_WRITE, EV_ADD | EV_ENABLE,
                                0, 0, NULL);
  client_ip = ft::inet_ntoa(client_addr.sin_addr.s_addr);
  client_port = static_cast<int>(client_addr.sin_port);
  m_connections_[client_fd] = Connection(client_fd, client_ip, client_port);
  m_server_manager_->insertFd(client_fd, ServerManager::FD_CLIENT);
  return true;
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
