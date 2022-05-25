/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:57:18 by bylee             #+#    #+#             */
/*   Updated: 2022/05/25 17:51:08 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <fcntl.h>
# include <iostream>
# include <string>
# include <exception>
# include <vector>
# include <map>
# include "ServerConfig.hpp"
# include "WebservConfig.hpp"
# include "LocationConfig.hpp"
# include "Connection.hpp"

class ServerManager;

class Server {
 public:
  /*
    Server constructor & destructor declaration
  */
  Server(void);
  Server(ServerManager* sm, WebservConfig* wc,
          const std::string& server_block,
          std::vector<std::string>& location_blocks);
  Server(const Server& ref);

  virtual ~Server(void);

  Server& operator=(const Server& ref);

  /*
    Server getter declaration
  */
  ServerManager*                      get_m_server_manager() const;
  WebservConfig*                      get_m_webserv_config() const;
  ServerConfig                        get_m_config() const;
  const std::vector<LocationConfig>&  get_m_locations() const;
  const std::map<int, Connection>&    get_m_connections() const;

 private:
  ServerManager*              m_server_manager_;
  WebservConfig*              m_webserv_config_;
  ServerConfig                m_config_;
  std::vector<LocationConfig> m_locations_;
  std::map<int, Connection>   m_connections_;
};  //  class Server

std::ostream& operator<<(std::ostream& out, const Server& server);

#endif  //  SERVER_HPP_
