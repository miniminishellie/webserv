/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 18:42:34 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/24 22:22:18 by jihoolee         ###   ########.fr       */
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

void ServerManager::createServers(const std::string& conf_file_path,
                                  char* env[]) {
  int server_count = 10;

  if ((m_kqueue_ = kqueue()) == -1)
    std::runtime_error("kqueue() Error");

  for (int idx = 0; idx < server_count; ++idx) {
    Server new_server(this, &this->m_config_);

    changeEvents_(m_change_list_, new_server.get_m_socket_fd(), EVFILT_READ,
                  EV_ADD | EV_ENABLE, 0, 0, NULL);
    m_servers_[new_server.get_m_socket_fd()] = new_server;
  }
}

void ServerManager::runServers(void) {
  signal(SIGINT, this->changeSignal_);

  struct timeval  timeout;

  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  m_is_running_ = true;

  int new_events;
  struct kevent* curr_event;

  while (m_is_running_) {
    if ((new_events = kevent(m_kqueue_, &m_change_list_[0],
                              m_change_list_.size(), m_returned_events_,
                              1024, NULL)) == -1)
      throw std::runtime_error("kevent() Error");
    m_change_list_.clear();
    for (int i = 0; i < new_events; ++i) {
      curr_event = &m_returned_events_[i];
      if (curr_event->flags & EV_ERROR) {
      }
    }
  }
}

void ServerManager::exitWebserv(const std::string& what) {
  std::cerr << what << std::endl;
  exit(EXIT_SUCCESS);
}

void ServerManager::changeSignal_(int sig) {
  (void)sig;
  m_is_running_ = false;
}

void ServerManager::changeEvents_(std::vector<struct kevent>& change_list,
                                  uintptr_t ident,
                                  int16_t filter,
                                  uint16_t flags,
                                  uint32_t fflags,
                                  intptr_t data,
                                  void* udata) {
  struct kevent temp_event;

  EV_SET(&temp_event, ident, filter, flags, fflags, data, udata);
  change_list.push_back(temp_event);
}
