/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bylee <bylee@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/17 17:15:09 by bylee             #+#    #+#             */
/*   Updated: 2022/05/20 22:11:56 by bylee            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# define DEFAULT_CONFIG_FILE_PATH "../conf/default.conf"

# include <string>
# include <iostream>

void exit_server(const std::string& error_message)
{
  std::cout << error_message << std::endl;
  exit(1);
}

void  parse_config(char *path, char **env)
{

}

#endif
