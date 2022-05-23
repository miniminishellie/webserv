/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 18:42:34 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/23 20:10:02 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerManager.hpp"

ServerManager::ServerManager(void)
    : m_is_running_(false),
      m_config_(),
      m_servers_() { }

ServerManager::ServerManager(const ServerManager& sm) { }

ServerManager::~ServerManager(void) {}

ServerManager& ServerManager::operator=(const ServerManager& operand) {}

void ServerManager::createWebserv(const std::string& conf_file_path,
                                  char* env[]) {
  int server_count = 10;

  for (int idx = 0; idx < server_count; ++idx) {
    m_servers_.push_back(Server(this, &this->m_config_));
  }
}

void ServerManager::exitWebserv(const std::string& what) {
  std::cerr << what << std::endl;
  exit(EXIT_SUCCESS);
}
