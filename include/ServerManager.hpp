/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 22:17:14 by bylee             #+#    #+#             */
/*   Updated: 2022/05/23 17:08:13 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "Config.hpp"
# include "Location.hpp"
# include "Server.hpp"
# include "Libft.hpp"

class ServerManager
{
private:
  std::vector<Server> m_servers;
  Config m_config;

public:
  /*
  ServerManager Constructor & Destructor declaration
  */
  ServerManager();
  ServerManager(const ServerManager& ref);
  ServerManager& operator=(const ServerManager& ref);
  virtual ~ServerManager();

  /*
  ServerMananger Getter declaration
  */

  /*
  ServerManager Methods declaration
  */
  bool splitConfigString(std::string& config_string, std::string& config_block,\
    std::vector<std::string>& serveral_strings);
  bool splitServerString(std::string server_string, std::string& server_block,\
    std::vector<std::string>& location_blocks);
  bool isValidConfigBlock(std::string& config_block);
  bool isValidServerBlock(std::string& server_block);
  bool isValidLocationBlock(std::string& location_block);
  void exitServer(const std::string& error_message);
  void createServer(const std::string& config_file_path, char **env);
};

#endif
