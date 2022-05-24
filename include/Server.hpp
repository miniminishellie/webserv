/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:43:04 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/24 16:59:46 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <vector>
# include <map>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <exception>
# include <unistd.h>
# include <fcntl.h>
# include "WebservConfig.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "Connection.hpp"

class ServerManager;

class Server {
 public:
  Server(ServerManager* sm, WebservConfig* wc);

  virtual ~Server(void);

  Server& operator=(const Server& operand);

  void initServerSocket(void);

  int get_m_socket_fd(void) { return m_socket_fd_; }
 private:
  ServerManager*              m_server_manager_;
  WebservConfig*              m_webserv_config_;
  ServerConfig                m_config_;
  int                         m_socket_fd_;
  std::vector<LocationConfig> m_locations_;
  std::map<int, Connection>   m_connections_;
};
#endif  //  SERVER_MANAGER_HPP_
