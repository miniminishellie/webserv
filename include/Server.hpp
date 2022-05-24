/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:57:18 by bylee             #+#    #+#             */
/*   Updated: 2022/05/24 14:30:06 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

# include "Webserv.hpp"
# include "Config.hpp"
# include "ServerManager.hpp"

class ServerManager;

class Server
{
private:
  ServerManager*  m_manager;
  std::string     m_server_name;
  std::string     m_host;
  int             m_port;
  int             m_fd;
  size_t          m_request_uri_limit_size;
  size_t          m_request_header_limit_size;
  size_t          m_limit_client_body_size;
  std::string     m_default_error_page;
  Config*         m_config;
public:
  /*
    Server constructor & destructor declaration
  */
  Server();
  Server(ServerManager* server_manager, const std::string& server_block, std::vector<std::string>& location_blocks, Config* config);
  Server(const Server& ref);
  Server& operator=(const Server& ref);
  virtual ~Server();
};

#endif
