/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 22:17:14 by bylee             #+#    #+#             */
/*   Updated: 2022/05/31 17:08:14 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP_
# define SERVERMANAGER_HPP_

# include <sys/time.h>
# include <sys/event.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <iostream>
# include <map>
# include <vector>
# include <string>
# include <exception>
# include "WebservConfig.hpp"
# include "ServerConfig.hpp"
# include "Connection.hpp"
// # include "Server.hpp"

# define DEFAULT_CONFIG_PATH  "config/default.conf"
# define PORT_HTTP 80
# define PORT_HTTPS 443
# define PORT_REGISTERED_MIN 1024
# define PORT_REGISTERED_MAX 49151

# define REQUEST_URI_LIMIT_SIZE_MIN 64
# define REQUEST_URI_LIMIT_SIZE_MAX 8192
# define REQUEST_HEADER_LIMIT_SIZE_MIN 64
# define REQUEST_HEADER_LIMIT_SIZE_MAX 8192
# define LIMIT_CLIENT_BODY_SIZE_MAX 200000000

class ServerManager {
 public:
  static int log_fd;
  /*
  ServerManager constructor & destructor declaration
  */
  enum FdType { FD_SERVER, FD_CLIENT };
  ServerManager();
  ServerManager(const ServerManager& ref);

  virtual ~ServerManager();

  ServerManager& operator=(const ServerManager& ref);

  void  createServers(const std::string& config_file_path, char* env[]);
  void  exitWebserv(const std::string& error_message);
  void  runServers(void);
  void  insertFd(int fd, FdType type);
  void  addEvent(uintptr_t ident,
                    int16_t filter,
                    uint16_t flags,
                    uint32_t fflags,
                    intptr_t data,
                    void* udata);

 private:
  void  addServer_(ServerConfig new_server);
  void  changeSignal_(int sig);
  void  changeEvents_(std::vector<struct kevent>& change_list,
                      uintptr_t ident,
                      int16_t filter,
                      uint16_t flags,
                      uint32_t fflags,
                      intptr_t data,
                      void* udata);
  bool  acceptNewConnection_(int server_socket_fd);

  bool  splitConfigString_(std::string& config_string,
          std::string& config_block,
          std::vector<std::string>& serveral_strings);
  bool splitServerString_(std::string server_string, std::string& server_block,
          std::vector<std::string>& location_blocks);
  bool isValidConfigBlock_(std::string& config_block);
  bool isValidServerBlock_(std::string& server_block);
  bool isValidLocationBlock_(std::string& location_block);

  bool                                m_is_running_;
  std::map<std::string, std::string>  m_mime_types_;
  WebservConfig                       m_config_;
  std::map<int, ServerConfig>         m_server_configs_;
  std::map<int, Connection>           m_connections_;
//   std::map<int, Server>       m_servers_;
//   std::vector<Server>         m_servers_;

  int                                 m_kqueue_;
  std::map<int, FdType>               m_fd_set_;  //  ->필요한지 한번 생각해보자
  struct kevent                       m_returned_events_[1024];
  std::vector<struct kevent>          m_change_list_;
};  // class ServerManager

#endif  //  SERVER_MANGER_HPP_
