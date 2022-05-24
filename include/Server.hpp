/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:57:18 by bylee             #+#    #+#             */
/*   Updated: 2022/05/24 21:28:58 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

# include "Webserv.hpp"
# include "Config.hpp"
# include "Location.hpp"
# include "ServerManager.hpp"

class ServerManager;

class Server
{
private:
  ServerManager*        m_manager;
  Config*               m_config;
  std::string           m_server_name;
  std::string           m_host;
  int                   m_port;
  size_t                m_request_uri_limit_size;
  size_t                m_request_header_limit_size;
  size_t                m_limit_client_body_size;
  std::string           m_default_error_page;
  std::vector<Location> m_locations;

public:
  /*
    Server constructor & destructor declaration
  */
  Server();
  Server(ServerManager* server_manager, const std::string& server_block,\
    std::vector<std::string>& location_blocks, Config* config);
  Server(const Server& ref);
  Server& operator=(const Server& ref);
  virtual ~Server();

  /*
    Server getter declaration
  */
  const std::string& get_m_server_name() const;
  const std::string& get_m_host() const;
  int get_m_port() const;
  size_t get_m_request_uri_limit_size() const;
  size_t get_m_request_header_limit_size() const;
  size_t get_m_limit_client_body_size() const;
  const std::string& get_m_default_error_page() const;
  Config* get_m_config() const;
  const std::vector<Location>& get_m_locations() const;
};

std::ostream&	operator<<(std::ostream& out, const Server& server);

#endif
