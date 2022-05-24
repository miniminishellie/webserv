/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 19:56:05 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/24 14:50:05 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"
#include "Server.hpp"

Server::Server(ServerManager* sm, WebservConfig* wc)
    : m_server_manager_(sm),
      m_webserv_config_(wc),
      m_socket_fd_(-1) {
  initServerSocket();
}

Server::~Server(void) {}

Server& Server::operator=(const Server& operand) {}

void Server::initServerSocket(void) {
  if ((m_socket_fd_ = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    throw std::runtime_error("socket() Error");

  int opt = 1;
  struct sockaddr_in serv_addr;

  setsockopt(m_socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(m_config_.get_m_host().c_str());
  serv_addr.sin_port = htons(m_config_.get_m_port());
  if (bind(m_socket_fd_,
            reinterpret_cast<struct sockaddr *>(&serv_addr),
            sizeof(serv_addr)) == -1)
    throw std::runtime_error("bind() Error");
  if (listen(m_socket_fd_, 256) == -1)  //  backlog 크기는 나중에 test 해보면서 수정해봐야할듯?
    throw std::runtime_error("listen() Error");
  if (fcntl(m_socket_fd_, F_SETFL, O_NONBLOCK) == -1)
    throw std::runtime_error("fcntl() Error");
}
