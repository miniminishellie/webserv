/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jihoolee <jihoolee@student.42SEOUL.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:07:18 by jihoolee          #+#    #+#             */
/*   Updated: 2022/05/17 18:50:57 by jihoolee         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_MANAGER_HPP_
# define SERVER_MANAGER_HPP_

# include <iostream>
# include <vector>
# include <string>
# include "Config.hpp"
# include "Server.hpp"

class ServerManager {
 public:

  void exitServer(const std::string& what);
 private:
  Config              m_config_;
  std::vector<Server> m_servers_;
};

#endif  //  SERVER_MANAGER_HPP_
