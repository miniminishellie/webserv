/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:43:04 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/23 15:04:58 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP_
# define SERVER_HPP_

# include <vector>
# include <map>
# include <sys/select.h>
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "Connection.hpp"

class Server {
 public:
 private:
  ServerManager*              m_server_manager_;
  ServerConfig                m_config_;
  int                         m_socket_fd_;
  std::vector<LocationConfig> m_locations_;
  std::map<int, Connection>   m_connections_;
};
#endif  //  SERVER_MANAGER_HPP_
