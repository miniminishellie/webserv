/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:07:18 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/23 17:36:17 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP_
# define SERVER_MANAGER_HPP_

# include <iostream>
# include <vector>
# include <string>
# include <exception>
# include "WebservConfig.hpp"
# include "Server.hpp"

# define DEFAULT_CONFIG_PATH  "config/default.conf"

class ServerManager {
 public:
  ServerManager(void);
  ServerManager(const ServerManager& sm);

  virtual ~ServerManager(void);

  ServerManager& operator=(const ServerManager& operand);

  void createWebserv(const std::string& conf_file_path, char* env[]);
  void runWebserv(void);
  void exitWebserv(const std::string& what);

 private:
  bool                m_is_running_;
  WebservConfig       m_config_;
  std::vector<Server> m_servers_;

};  //  class ServerManager

#endif  //  SERVER_MANAGER_HPP_
