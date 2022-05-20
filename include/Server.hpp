/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 16:57:18 by bylee             #+#    #+#             */
/*   Updated: 2022/05/20 21:35:35 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

# include "Webserv.hpp"

class Server
{
private:

public:
  Server();
  Server(const Server& ref);
  Server& operator=(const Server& ref);
  virtual ~Server();
};

#endif
