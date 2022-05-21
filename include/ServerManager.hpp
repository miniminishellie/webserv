/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/20 22:17:14 by bylee             #+#    #+#             */
/*   Updated: 2022/05/21 14:42:03 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERMANAGER_HPP
# define SERVERMANAGER_HPP

# include "../include/Config.hpp"
# include "../include/Server.hpp"
# include "../include/Location.hpp"

class ServerManager
{
private:

public:
  
  void exit_server(const std::string& error_message)
  {
    std::cout << error_message << std::endl;
    exit(1);
  }
  
  void  parse_config(char *path, char **env)
  {
    
  }
};

#endif
