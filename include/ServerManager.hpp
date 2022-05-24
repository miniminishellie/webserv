/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:07:18 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/24 17:22:27 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP_
# define SERVER_MANAGER_HPP_

# include <iostream>
# include <vector>
# include <string>
# include <exception>
# include <sys/time.h>
# include <sys/event.h>
# include <sys/types.h>
# include "WebservConfig.hpp"
# include "Server.hpp"

# define DEFAULT_CONFIG_PATH  "config/default.conf"

class ServerManager {
 public:
  ServerManager(void);
  ServerManager(const ServerManager& sm);

  virtual ~ServerManager(void);

  ServerManager& operator=(const ServerManager& operand);

  void createServers(const std::string& conf_file_path, char* env[]);
  void runServers(void);
  void exitWebserv(const std::string& what);

 private:
  void  changeSignal_(int sig);
  void  changeEvents_(std::vector<struct kevent>& change_list,
                      uintptr_t ident,
                      int16_t filter,
                      uint16_t flags,
                      uint32_t fflags,
                      intptr_t data,
                      void* udata);

  bool                        m_is_running_;
  WebservConfig               m_config_;
  std::map<int, Server>       m_servers_;

  int                         m_kqueue_;
  struct kevent*              m_returned_events_;
  std::vector<struct kevent>  m_change_list_;
};  //  class ServerManager

#endif  //  SERVER_MANAGER_HPP_
