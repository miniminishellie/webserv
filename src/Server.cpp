/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/23 19:56:05 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/23 21:17:42 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(ServerManager* sm, WebservConfig* wc)
    : m_socket_fd_(-1) {
  if ((m_socket_fd_ = socket(PF_INET, SOCK_STREAM, 0) == -1))
    throw std::runtime_error("Socket Error");
}

Server::~Server(void) {}

Server& Server::operator=(const Server& operand) {}

void Server::initServerSocket(void) {
  if ((m_socket_fd_ = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    throw std::runtime_error("Socket() Error");

  int opt = 1;
  struct sockaddr_in serv_addr;
  setsockopt(m_socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(m_config_.get_m_host().c_str());
  serv_addr.sin_port = htons(m_config_.get_m_port());
}
